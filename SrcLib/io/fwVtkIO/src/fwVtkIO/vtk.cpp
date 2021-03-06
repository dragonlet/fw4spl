/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <cstring>
#include <functional>
#include <numeric>
#include <stdexcept>

#include <boost/assign/list_of.hpp>
#include <boost/cast.hpp>

#include <vtkImageImport.h>
#include <vtkSetGet.h>
#include <vtkType.h>
//Required for proper object factory initialization
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingOpenGL);

// for mesh
#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkImageExport.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataNormals.h>
#include <vtkLookupTable.h>

#include <vtkMassProperties.h>

#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageAccumulate.h>

#include <vtkDataSetAttributes.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <fwMath/MeshFunctions.hpp>

#include <fwData/Image.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwComEd/helper/Image.hpp>

#include "fwVtkIO/vtk.hpp"


namespace fwVtkIO
{


TypeTranslator::fwToolsToVtkMap::mapped_type TypeTranslator::translate( const TypeTranslator::fwToolsToVtkMap::key_type &key )
{
    fwToolsToVtkMap::const_iterator it = s_toVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_toVtk.end() );
    return it->second;
}

TypeTranslator::VtkTofwToolsMap::mapped_type TypeTranslator::translate( const TypeTranslator::VtkTofwToolsMap::key_type &key )
{
    VtkTofwToolsMap::const_iterator it = s_fromVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_fromVtk.end() );
    return it->second;
}




const TypeTranslator::fwToolsToVtkMap TypeTranslator::s_toVtk
        = boost::assign::map_list_of
            // char and signed char are treated as the same type.
            // and plain char is used when writing an int8 image
            ( fwTools::Type::create("int8" )         , VTK_CHAR )
            ( fwTools::Type::create("uint8" )        , VTK_UNSIGNED_CHAR )

            ( fwTools::Type::create("int16")         , VTK_SHORT )
            ( fwTools::Type::create("uint16")        , VTK_UNSIGNED_SHORT )

            ( fwTools::Type::create("int32")         , VTK_INT )
            ( fwTools::Type::create("uint32")        , VTK_UNSIGNED_INT )

            ( fwTools::Type::create("float" )        , VTK_FLOAT )
            ( fwTools::Type::create("double")        , VTK_DOUBLE )

#if ( INT_MAX < LONG_MAX )
            ( fwTools::Type::create("int64")         , VTK_LONG )
            ( fwTools::Type::create("uint64")        , VTK_UNSIGNED_LONG )
#endif
            ;




const TypeTranslator::VtkTofwToolsMap TypeTranslator::s_fromVtk
    = boost::assign::map_list_of

        // char and signed char are treated as the same type.
        // and plain char is used when writing an int8 image
        ( VTK_SIGNED_CHAR        , fwTools::Type::create("int8" )  )
        ( VTK_CHAR               , fwTools::Type::create("int8" )  )
        ( VTK_UNSIGNED_CHAR      , fwTools::Type::create("uint8" ) )

        ( VTK_SHORT              , fwTools::Type::create("int16")  )
        ( VTK_UNSIGNED_SHORT     , fwTools::Type::create("uint16") )

        ( VTK_INT                , fwTools::Type::create("int32")  )
        ( VTK_UNSIGNED_INT       , fwTools::Type::create("uint32") )

        ( VTK_FLOAT              , fwTools::Type::create("float" ) )
        ( VTK_DOUBLE             , fwTools::Type::create("double") )

#if ( INT_MAX < LONG_MAX )
        ( VTK_LONG               , fwTools::Type::create("int64")  )
        ( VTK_UNSIGNED_LONG      , fwTools::Type::create("uint64") )

        ( VTK___INT64            , fwTools::Type::create("int64")  )
        ( VTK_LONG_LONG          , fwTools::Type::create("int64")  )

        ( VTK_UNSIGNED___INT64   , fwTools::Type::create("uint64") )
        ( VTK_UNSIGNED_LONG_LONG , fwTools::Type::create("uint64") )
#else
        ( VTK_LONG               , fwTools::Type::create("int32")  )
        ( VTK_UNSIGNED_LONG      , fwTools::Type::create("uint32") )
#endif
        ;


//-----------------------------------------------------------------------------

void toVTKImage( ::fwData::Image::sptr data,  vtkImageData *dst)
{
    vtkSmartPointer< vtkImageImport > importer = vtkSmartPointer< vtkImageImport >::New();

    configureVTKImageImport( importer, data );

    importer->Update();

    dst->ShallowCopy(importer->GetOutput());
}

//-----------------------------------------------------------------------------


template< typename IMAGETYPE >
void *newBuffer(size_t size)
{
    IMAGETYPE *destBuffer;
    try
    {
        destBuffer = new IMAGETYPE[ size ];
    }
    catch (std::exception &e)
    {
        OSLM_ERROR ("No enough memory to allocate an image of type "
                << fwTools::makeDynamicType<IMAGETYPE>().string()
                << " and of size "<< size << "." << std::endl
                << e.what() );
        throw;
    }
    return destBuffer;
}

//-----------------------------------------------------------------------------

template< typename IMAGETYPE >
void fromRGBBuffer( void *input, size_t size, void *&destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE *destBufferTyped = (IMAGETYPE*)destBuffer;
    IMAGETYPE *inputTyped      = (IMAGETYPE*)input;
    IMAGETYPE *finalPtr        = ((IMAGETYPE*)destBuffer) + size;
    IMAGETYPE valR, valG,valB;

    while (destBufferTyped < finalPtr)
    {
        valR = (IMAGETYPE)(float((*(inputTyped++)) * 0.30));
        valG = (IMAGETYPE)(float((*(inputTyped++)) * 0.59));
        valB = (IMAGETYPE)(float((*(inputTyped++)) * 0.11));
        (*destBufferTyped++) = valR + valG + valB;
    }
}
//-----------------------------------------------------------------------------


void fromVTKImage( vtkImageData* source, ::fwData::Image::sptr destination )
{
    SLM_ASSERT("vtkImageData source and/or ::fwData::Image destination are not correct", destination && source );

    ::fwComEd::helper::Image imageHelper(destination);

    // ensure image size correct
//    source->UpdateInformation();
//    source->PropagateUpdateExtent();

    int dim = source->GetDataDimension() ;
    OSLM_TRACE("source->GetDataDimension() : " << dim);

    SLM_WARN_IF("2D Vtk image are not yet correctly managed", dim == 2);


    destination->setSize( ::fwData::Image::SizeType(source->GetDimensions(), source->GetDimensions()+dim) );
    destination->setSpacing( ::fwData::Image::SpacingType(source->GetSpacing(), source->GetSpacing()+dim) );
    destination->setOrigin( ::fwData::Image::OriginType(source->GetOrigin(), source->GetOrigin()+dim) );


    size_t size = std::accumulate(source->GetDimensions(), source->GetDimensions()+dim, 1, std::multiplies<size_t>() );
    void *input = source->GetScalarPointer();

    if (size != 0)
    {
        void *destBuffer;
        int nbBytePerPixel = source->GetScalarSize();
        int nbComponents = source->GetNumberOfScalarComponents();
        OSLM_TRACE("image size : " << size << " - nbBytePerPixel : " << nbBytePerPixel );

        destination->setNumberOfComponents(1);
        if (nbComponents == 3 && nbBytePerPixel == 2)
        {
            SLM_TRACE ("RGB 16bits");

            destination->setType( "uint16" );
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBuffer< unsigned short >(input, size, destBuffer);
        }
        else if (nbComponents == 3 && nbBytePerPixel == 1)
        {
            SLM_TRACE ("RGB 8bits");

            destination->setType( "uint8" );
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            SLM_ASSERT("Image allocation error", destBuffer != NULL);
            fromRGBBuffer< unsigned char >(input, size, destBuffer);
        }
        else
        {
            SLM_TRACE ("Luminance image");
            destination->setType( TypeTranslator::translate( source->GetScalarType() ) );
            destination->setNumberOfComponents(nbComponents);
            destination->allocate();
            ::fwData::ObjectLock lock(destination);
            destBuffer = imageHelper.getBuffer();
            size_t sizeInBytes = destination->getSizeInBytes();
            std::memcpy(destBuffer, input, sizeInBytes);
        }
    }

}

//------------------------------------------------------------------------------

void configureVTKImageImport( ::vtkImageImport * _pImageImport, ::fwData::Image::sptr _pDataImage )
{
    ::fwComEd::helper::Image imageHelper(_pDataImage);

    _pImageImport->SetDataSpacing(  _pDataImage->getSpacing().at(0),
                                    _pDataImage->getSpacing().at(1),
                                    _pDataImage->getSpacing().at(2)
                                );

    _pImageImport->SetDataOrigin(   _pDataImage->getOrigin().at(0),
                                    _pDataImage->getOrigin().at(1),
                                    _pDataImage->getOrigin().at(2)
                                );

    _pImageImport->SetWholeExtent(  0, _pDataImage->getSize().at(0) - 1,
                                    0, _pDataImage->getSize().at(1) - 1,
                                    0, _pDataImage->getSize().at(2) - 1
                                );

    _pImageImport->SetNumberOfScalarComponents(static_cast<int>( _pDataImage->getNumberOfComponents() ));

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();
    // no copy, no buffer destruction/management
    _pImageImport->SetImportVoidPointer( imageHelper.getBuffer() );
    // used to set correct pixeltype to VtkImage
    _pImageImport->SetDataScalarType( TypeTranslator::translate(_pDataImage->getType()) );
}

//-----------------------------------------------------------------------------

// This method is written to be as fast as possible, take care when modifying it.
vtkPolyData*  updatePolyDataPoints(vtkPolyData* polyDataDst, ::fwData::TriangularMesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    vtkPoints *polyDataPoints = polyDataDst->GetPoints();
    ::fwData::TriangularMesh::PointContainer &points = meshSrc->points();

    ::fwData::TriangularMesh::PointContainer::iterator pointsIter = points.begin();
    ::fwData::TriangularMesh::PointContainer::iterator pointsEnd  = points.end();

    float *xyz = 0;
    vtkIdType id = 0;
    if (points.size() != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(points.size());
    }
    for( ; pointsIter != pointsEnd ; ++pointsIter )
    {
        xyz = &(pointsIter->front());
        polyDataPoints->SetPoint(id++, xyz);
    }
    polyDataPoints->Modified();
    return polyDataDst;
}

//-----------------------------------------------------------------------------

vtkPolyData*  toVTKMesh( ::fwData::TriangularMesh::sptr mesh )
{
    vtkPolyData *polygonGrid = vtkPolyData::New();

    if ( mesh && !mesh->points().empty() )
    {
        vtkSmartPointer< vtkPoints > trianPts = vtkSmartPointer< vtkPoints >::New();
        polygonGrid->SetPoints(trianPts);
        updatePolyDataPoints(polygonGrid, mesh);

        ::fwData::TriangularMesh::CellContainer &cells = mesh->cells();
        unsigned int nbCells = cells.size() ;
        assert( nbCells ) ;
        vtkIdType typeCell = VTK_TRIANGLE;
        polygonGrid->Allocate(typeCell,nbCells);
        for(unsigned int i=0 ; i<nbCells ; ++i )
        {
            vtkIdType cell[3];
            const std::vector<int> &meshCell = (cells[i]);
            cell[0] = meshCell[0] ;
            cell[1] = meshCell[1] ;
            cell[2] = meshCell[2] ;
            polygonGrid->InsertNextCell( typeCell, 3, cell );
        }
    }
    else
    {
        SLM_INFO("vtkPolyData build from empty mesh.");
    }

    return polygonGrid;
}

//-----------------------------------------------------------------------------

bool fromVTKMesh( vtkPolyData *polyData, ::fwData::TriangularMesh::sptr triangularMesh)
{
    SLM_TRACE_FUNC();
    bool res = false;
    vtkPoints *trianPts = polyData->GetPoints();

    if (trianPts)
    {
        // Clear the container cells and set its capacity to 0
        triangularMesh->clearCells();
        // Clear the container points and set its capacity to 0
        triangularMesh->clearPoints();

        vtkDataArray* points = trianPts->GetData();
        vtkIdType numberOfTuples = points->GetNumberOfTuples();
        std::vector<float> vPoint(3, 0.0);
        triangularMesh->points().resize(numberOfTuples, vPoint);
        double* tuple;
        for (vtkIdType i = 0; i < numberOfTuples; ++i)
        {
            tuple = points->GetTuple(i);
            ::fwData::TriangularMesh::PointContainer::value_type &vPoints = triangularMesh->points()[i];
            std::copy(tuple, tuple+3, vPoints.begin());
        }

        vtkIdType numberOfCells = polyData->GetNumberOfCells();
        std::vector<int> vCell(3, 0);
        triangularMesh->cells().resize(numberOfCells, vCell);
        vtkCell* cell;
        vtkIdList* idList;
        vtkIdType* idType;
        for (vtkIdType i = 0 ; i < numberOfCells ; ++i)
        {
            cell = polyData->GetCell(i);
            idList = cell->GetPointIds();
            idType = idList->GetPointer(0);
            ::fwData::TriangularMesh::CellContainer::value_type &vCells = triangularMesh->cells()[i];
            std::copy(idType, idType+3, vCells.begin());
        }
        res = true;
    }

    return res;
}

//-----------------------------------------------------------------------------

double computeVolume( ::fwData::TriangularMesh::sptr _triangularMesh )
{
    ::fwData::TriangularMesh::sptr closedMesh = ::fwData::Object::copy(_triangularMesh);

    ::fwMath::closeSurface(closedMesh->points(), closedMesh->cells());

    vtkPolyData*  vtkMeshRaw = toVTKMesh( closedMesh );

    vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInputData(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInputConnection( filter->GetOutputPort() );
    calculator->Update();
    double volume =  calculator->GetVolume();
    OSLM_DEBUG("GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "vtkMassProperties::GetVolume() - | vtkMassProperties::GetVolumeProjected() |";
        ss << ">  vtkMassProperties::GetVolume()/10000.0" << std::endl;
        ss << "vtkMassProperties::GetVolume() = " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected();
        throw (std::out_of_range( ss.str() ));
    }

    vtkMeshRaw->Delete();

    return volume;
}

//-----------------------------------------------------------------------------

double computeVolumeWithStencil(  ::fwData::TriangularMesh::sptr _triangularMesh )
{
    vtkPolyData*  vtkMesh = toVTKMesh( _triangularMesh );

    vtkImageData* vi = vtkImageData::New();
    vi->SetOrigin( 0,0,0 ); // adjust these to your needs
    vi->SetSpacing( 0.5, 0.5, 0.5 ); // adjust these to your needs
    vi->SetDimensions( vtkMesh->GetBounds()[1]*2,  vtkMesh->GetBounds()[3]*2,  vtkMesh->GetBounds()[5]*2 ); // adjust these to your needs
    vi->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    vi->GetPointData()->GetScalars()->FillComponent(0, 1.0);
    // outputMesh is of vtkPolyData* type and contains your mesh data
    vtkPolyDataToImageStencil* pti = vtkPolyDataToImageStencil::New();
    pti->SetInputData( vtkMesh );
    vtkImageAccumulate* ac = vtkImageAccumulate::New();
    ac->SetInputData( vi );
    ac->SetStencilData( pti->GetOutput() );
    ac->ReverseStencilOff();
    ac->Update();

    unsigned long nbVoxel = ac->GetVoxelCount();

    pti->Delete();
    ac->Delete();
    vi->Delete();
    vtkMesh->Delete();

    return nbVoxel;
}

//-----------------------------------------------------------------------------

vtkMatrix4x4 *  toVTKMatrix( ::fwData::TransformationMatrix3D::sptr _transfoMatrix )
{
    vtkMatrix4x4  *matrix = vtkMatrix4x4 ::New();
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            matrix->SetElement(l, c, _transfoMatrix->getCoefficient(l, c));
        }
    }
    return matrix;
}

//-----------------------------------------------------------------------------

bool fromVTKMatrix( vtkMatrix4x4* _matrix, ::fwData::TransformationMatrix3D::sptr _transfoMatrix)
{
    SLM_TRACE_FUNC();
    bool res = true;
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            _transfoMatrix->setCoefficient(l,c, _matrix->GetElement(l,c));
        }
    }
    return res;
}

//-----------------------------------------------------------------------------

} // namespace fwVtkIO
