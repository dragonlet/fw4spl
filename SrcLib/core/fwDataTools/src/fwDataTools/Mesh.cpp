/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/type_traits/make_unsigned.hpp>

#include <map>

#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>

#include <fwTools/NumericRoundCast.hxx>

#include "fwDataTools/thread/RegionThreader.hpp"
#include "fwDataTools/Mesh.hpp"


namespace fwDataTools
{

struct RandFloat
{
    float operator()()
    {
        return ((rand() % 101 - 50.f)) / 500.f;
    }
};

//------------------------------------------------------------------------------

void Mesh::initRand()
{
    std::srand(::fwTools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

void Mesh::toTriangularMesh(::fwData::Mesh::sptr mesh, ::fwData::TriangularMesh::sptr trian)
{
    FW_RAISE_IF("Can't convert this Mesh to TriangularMesh", !Mesh::hasUniqueCellType(mesh, ::fwData::Mesh::TRIANGLE));

    // Clear the container cells and set its capacity to 0
    trian->clearCells();
    // Clear the container points and set its capacity to 0
    trian->clearPoints();

    ::fwComEd::helper::Mesh meshHelper(mesh);

    size_t numberOfPoints = mesh->getNumberOfPoints();
    std::vector<float> vPoint(3, 0.0);
    trian->points().resize(numberOfPoints, vPoint);

    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    typedef boost::make_unsigned< ::fwData::Mesh::PointsMultiArrayType::index >::type PointTypesIndex;
    for (PointTypesIndex i = 0; i != numberOfPoints; ++i)
    {
        ::fwData::TriangularMesh::PointContainer::value_type &point = trian->points()[i];
        for (PointTypesIndex j = 0; j != 3; ++j)
        {
            point[j] = points[i][j];
        }
    }

    size_t numberOfCells = mesh->getNumberOfCells();
    std::vector<int> vCell(3, 0);
    trian->cells().resize(numberOfCells, vCell);

    ::fwData::Mesh::CellDataMultiArrayType cells = meshHelper.getCellData();
    typedef boost::make_unsigned< ::fwData::Mesh::CellDataMultiArrayType::index >::type CellTypesIndex;
    size_t cellsSize = numberOfCells*3;
    SLM_ASSERT("Wrong CellDataMultiArray size", cells.size() == cellsSize);
    for (CellTypesIndex i = 0; i < cellsSize; i+=3)
    {
        ::fwData::TriangularMesh::CellContainer::value_type &vCells = trian->cells()[i/3];
        vCells[0] = ::fwTools::numericRoundCast<int>(cells[i]);
        vCells[1] = ::fwTools::numericRoundCast<int>(cells[i+1]);
        vCells[2] = ::fwTools::numericRoundCast<int>(cells[i+2]);
    }
}

//------------------------------------------------------------------------------

void Mesh::fromTriangularMesh(::fwData::TriangularMesh::sptr trian, ::fwData::Mesh::sptr mesh)
{
    ::fwData::TriangularMesh::PointContainer& vPoints = trian->points();
    ::fwData::TriangularMesh::CellContainer& vCells = trian->cells();

    mesh->clear();
    mesh->allocate(vPoints.size(), vCells.size(), vCells.size()*3);

    ::fwComEd::helper::Mesh meshHelper(mesh);

    ::fwData::Array::sptr pointsArray = mesh->getPointsArray();
    ::fwData::Array::sptr cellsArray = mesh->getCellDataArray();
    ::fwData::Array::sptr cellDataOffsetArray = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellTypeArray = mesh->getCellTypesArray();

    SLM_ASSERT("Wrong CellTypeArray dim", cellTypeArray->getSize().size()==1);
    SLM_ASSERT("Wrong CellTypeArray size", cellTypeArray->getSize()[0]==vCells.size());
    SLM_ASSERT("Wrong pointsArray dim", pointsArray->getSize().size()==1);
    SLM_ASSERT("Wrong pointsArray size", pointsArray->getSize()[0]==vPoints.size());
    SLM_ASSERT("Wrong cellsArray dim", cellsArray->getSize().size()==1);
    SLM_ASSERT("Wrong cellsArray size", cellsArray->getSize()[0]==vCells.size()*3);
    SLM_ASSERT("Wrong cellDataOffsetArray dim", cellDataOffsetArray->getSize().size()==1);
    SLM_ASSERT("Wrong cellDataOffsetArray size", cellDataOffsetArray->getSize()[0]==vCells.size());

    //Initialized pointsArray
    for(size_t i=0; i<vPoints.size(); i++)
    {
        meshHelper.insertNextPoint(vPoints[i][0], vPoints[i][1], vPoints[i][2]);
    }

    //Initialized cellsArray
    for(size_t i=0; i<vCells.size(); i++)
    {
        meshHelper.insertNextCell(vCells[i][0], vCells[i][1], vCells[i][2]);
    }
    mesh->adjustAllocatedMemory();
}

//------------------------------------------------------------------------------

bool Mesh::hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell)
{
    bool res = true;
    ::fwComEd::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::CellTypesMultiArrayType cellTypes = meshHelper.getCellTypes();

    BOOST_FOREACH(::fwData::Mesh::CellTypes type, cellTypes)
    {
        if(type != cell)
        {
            return false;
        }
    }
    return res;
}

//------------------------------------------------------------------------------

typedef boost::multi_array_ref<Point, 1> PointsMultiArrayType;


Vector<float> &computeTriangleNormal(const Point &p1, const Point &p2, const Point &p3, Vector<float> &n)
{
    n = Vector<float>(p1, p2);
    Vector<float> v(p1, p3);
    n.crossWith(v);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

Vector<float> &computeTriangleNormal( const PointsMultiArrayType &points, const ::fwData::Mesh::CellValueType *cell, Vector<float> &n)
{
    const Point &p1 = points[cell[0]];
    const Point &p2 = points[cell[1]];
    const Point &p3 = points[cell[2]];

    computeTriangleNormal(p1, p2, p3, n);
    return n;
}

//------------------------------------------------------------------------------

Vector<float> &computeCellNormal( const PointsMultiArrayType &points, const ::fwData::Mesh::CellValueType *cell, size_t cellSize, Vector<float> &n)
{
    n = Vector<float>();
    Vector<float> v;

    for (size_t i=0; i< cellSize; ++i)
    {
        const Point &p1 = points[cell[i  ]];
        const Point &p2 = points[cell[(i+1)% cellSize]];
        const Point &p3 = points[cell[(i+2)% cellSize]];

        computeTriangleNormal(p1, p2, p3, n);

        n += v;
    }

    n /= ::fwTools::numericRoundCast<float>(cellSize);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

void generateRegionCellNormals(::fwComEd::helper::Mesh::sptr meshHelper, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    ::fwData::Mesh::csptr mesh = meshHelper->getMesh();
    ::fwComEd::helper::Array pointArrayHelper(mesh->getPointsArray());
    ::fwComEd::helper::Array cellNormalsArrayHelper(mesh->getCellNormalsArray());

    PointsMultiArrayType point = PointsMultiArrayType(
            static_cast<PointsMultiArrayType::element*>(pointArrayHelper.getBuffer()),
            ::boost::extents[mesh->getNumberOfPoints()]
            );


    ::fwData::Mesh::CellTypesMultiArrayType       cellTypes       = meshHelper->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType        cellData        = meshHelper->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = meshHelper->getCellDataOffsets();

    const Vector<float> vZero;
    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType *cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();

    Vector< ::fwData::Mesh::NormalValueType > *normals = cellNormalsArrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
    {
        Vector<float> &n = normals[i];

        type = cellTypes[i];
        offset = cellDataOffsets[i];
        cell = &cellData[offset];
        switch (type)
        {
            case 0:
            case 1:
            case 2:
                n = vZero;
                break;
            case 3:
                {
                    computeTriangleNormal(point, cell, n);
                }
                break;
            case 4:
            case 5:
                {
                    const ::fwData::Mesh::Id i1 = i+1;
                    cellLen = (( i1 < numberOfCells )? cellDataOffsets[i1]:cellDataSize) - cellDataOffsets[i];

                    computeCellNormal(point, cell, cellLen, n);
                }
        }
    }
}

//------------------------------------------------------------------------------


template <typename T>
void vectorSum( std::vector< std::vector<T> > &vectors, size_t regionMin, size_t regionMax )
{
    if (vectors.empty())
    {
        return;
    }

    typename std::vector< std::vector<T> >::iterator vIter = vectors.begin();

    std::vector<T> &res = vectors[0];

    for (++vIter; vIter != vectors.end(); ++vIter)
    {
        for (size_t i = regionMin; i<regionMax; ++i)
        {
            res[i] += (*vIter)[i];
        }
    }

}

//------------------------------------------------------------------------------

void Mesh::generateCellNormals(::fwData::Mesh::sptr mesh)
{
    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    if(numberOfCells > 0)
    {
        mesh->allocateCellNormals();
        ::fwComEd::helper::Mesh::sptr meshHelper;
        meshHelper = ::fwComEd::helper::Mesh::New(mesh);

        ::fwDataTools::thread::RegionThreader rt((numberOfCells >= 200000) ? 4 : 1);
        rt( ::boost::bind(&generateRegionCellNormals, meshHelper, _1, _2), numberOfCells );
    }
}

//------------------------------------------------------------------------------

typedef std::vector< std::vector< unsigned char > > CharVectors;
typedef std::vector< std::vector< float > > FloatVectors;

void generateRegionCellNormalsByPoints(FloatVectors &normalsData, CharVectors &normalCounts, size_t dataId,
        ::fwComEd::helper::Mesh::sptr meshHelper, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    ::fwData::Mesh::csptr mesh = meshHelper->getMesh();
    FloatVectors::value_type &normalsResults = normalsData[dataId];
    CharVectors::value_type  &normalCount    = normalCounts[dataId];

    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();
    normalsResults.resize(3*nbOfPoints, 0.f);
    normalCount.resize(nbOfPoints, 0);

    ::fwData::Mesh::CellTypesMultiArrayType       cellTypes       = meshHelper->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType        cellData        = meshHelper->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = meshHelper->getCellDataOffsets();


    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType *cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();

    ::fwComEd::helper::Array arrayHelper(mesh->getCellNormalsArray());
    Vector< ::fwData::Mesh::NormalValueType > *normals = arrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();
    Vector< ::fwData::Mesh::NormalValueType > *normalResults = reinterpret_cast< Vector< ::fwData::Mesh::NormalValueType > * >( &(*normalsResults.begin()));


    ::fwData::Mesh::CellValueType *pointId;
    ::fwData::Mesh::CellValueType *cellEnd;

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
    {
        type    = cellTypes[i];
        offset  = cellDataOffsets[i];
        cell    = &cellData[offset];
        cellLen = type;

        switch (type)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                cellLen = type;
                break;
            case 5:
                {
                    const ::fwData::Mesh::Id i1 = i+1;
                    cellLen = (( i1 < numberOfCells )? cellDataOffsets[i1]:cellDataSize) - cellDataOffsets[i];
                }
        }

        cellEnd = cell + cellLen;

        for(pointId = cell; pointId != cellEnd; ++pointId)
        {
            Vector< ::fwData::Mesh::NormalValueType > &res = normalResults[*pointId];
            res += normals[i];
            normalCount[*pointId] += 1;
        }

    }
}

//------------------------------------------------------------------------------

void normalizeRegionCellNormalsByPoints(FloatVectors::value_type &normalsData, CharVectors::value_type &normalCount,
        ::fwData::Mesh::sptr mesh, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    Vector< ::fwData::Mesh::NormalValueType > *normalSum = reinterpret_cast< Vector< ::fwData::Mesh::NormalValueType > * >( &(*normalsData.begin()) );

    ::fwComEd::helper::Array arrayHelper(mesh->getPointNormalsArray());
    Vector< ::fwData::Mesh::NormalValueType > *normals = arrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();

    for ( ::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        CharVectors::value_type::value_type count = normalCount[i];
        normals[i] = normalSum[i];

        if(count>1)
        {
            normals[i] /= count;
        }
    }
}


//------------------------------------------------------------------------------

void Mesh::generatePointNormals(::fwData::Mesh::sptr mesh)
{
    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();
    if(nbOfPoints > 0)
    {
        const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
        ::fwData::Array::sptr oldCellNormals = mesh->getCellNormalsArray();
        mesh->clearCellNormals();

        generateCellNormals(mesh);

        mesh->allocatePointNormals();

        ::fwComEd::helper::Mesh::sptr meshHelper;
        meshHelper = ::fwComEd::helper::Mesh::New(mesh);

        ::fwDataTools::thread::RegionThreader rt((nbOfPoints >= 100000) ? 4 : 1);


        FloatVectors normalsData(rt.numberOfThread());
        CharVectors  normalCounts(rt.numberOfThread());

        rt( ::boost::bind(&generateRegionCellNormalsByPoints,
                          ::boost::ref(normalsData),
                          ::boost::ref(normalCounts),
                          _3, meshHelper, _1, _2),
                          numberOfCells);

        rt( ::boost::bind(&vectorSum<FloatVectors::value_type::value_type>,
                          ::boost::ref(normalsData),
                          _1, _2),
                          nbOfPoints*3);


        rt( ::boost::bind(&vectorSum<CharVectors::value_type::value_type>,
                          ::boost::ref(normalCounts),
                          _1, _2),
                          nbOfPoints);


        rt( boost::bind( &normalizeRegionCellNormalsByPoints,
                         ::boost::ref(normalsData[0]),
                         ::boost::ref(normalCounts[0]),
                          mesh, _1, _2),
                         nbOfPoints);

        meshHelper.reset();
        mesh->setCellNormalsArray(oldCellNormals);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void regionShakeNormals(T normals, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    RandFloat randFloat;
    for (::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        normals[i] += v;
        normals[i].normalize();
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeNormals(::fwData::Array::sptr array)
{

    if(array
            && array->getType() == ::fwTools::Type::create<float>()
            && !array->empty()
            && array->getNumberOfComponents() == 3
            && array->getNumberOfDimensions() == 1
            )
    {
        ::fwComEd::helper::Array arrayHelper(array);
        void *buf;
        buf = arrayHelper.getBuffer();
                const ::fwData::Mesh::Id nbOfNormals = array->getSize().at(0);
        typedef boost::multi_array_ref<Vector<float>, 1> NormalsMultiArrayType;
        NormalsMultiArrayType normals = NormalsMultiArrayType(
                static_cast<NormalsMultiArrayType::element*>(buf),
                boost::extents[nbOfNormals]
                );

        ::fwDataTools::thread::RegionThreader rt((nbOfNormals >= 150000) ? 4 : 1);
        rt( ::boost::bind(&regionShakeNormals<NormalsMultiArrayType>,
                           boost::ref(normals),
                           _1, _2),
            nbOfNormals);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePointNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getPointNormalsArray());
}

//------------------------------------------------------------------------------

void Mesh::shakeCellNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getCellNormalsArray());
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(::fwData::Mesh::sptr mesh)
{
    mesh->allocatePointColors(::fwData::Mesh::RGB);

    ::fwComEd::helper::Mesh meshHelper(mesh);

    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfPoints = mesh->getNumberOfPoints();
    for(size_t i = 0; i<numberOfPoints; ++i)
    {
        color[0] = rand()%256;
        color[1] = 0;//rand()%256;
        color[2] = rand()%256;
        meshHelper.setPointColor(i, color);
    }
}
//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(::fwData::Mesh::sptr mesh)
{
    mesh->allocateCellColors(::fwData::Mesh::RGBA);

    ::fwComEd::helper::Mesh meshHelper(mesh);

    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfCells = mesh->getNumberOfCells();
    for(size_t i = 0; i<numberOfCells; ++i)
    {
        color[0] = rand()%256;
        color[1] = rand()%256;
        color[2] = 0;//rand()%256;
        color[3] = rand()%256;
        meshHelper.setCellColor(i, color);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePoint(::fwData::Mesh::sptr mesh)
{
    ::fwComEd::helper::Mesh meshHelper(mesh);

    size_t nbPts = mesh->getNumberOfPoints();
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    RandFloat randFloat;
    for(size_t i=0 ; i<nbPts ; ++i )
    {
        points[i][0] += randFloat()*5;
        points[i][1] += randFloat()*5;
        points[i][2] += randFloat()*5;
    }
}

//------------------------------------------------------------------------------

void Mesh::transform( ::fwData::Mesh::sptr mesh, ::fwData::TransformationMatrix3D::sptr t )
 {
    size_t nbPts = mesh->getNumberOfPoints();
    ::fwComEd::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    ::fwData::Mesh::PointValueType x, y, z, xp, yp, zp, factor;
    for(size_t i=0 ; i<nbPts ; ++i )
    {
        x = points[i][0];
        y = points[i][1];
        z = points[i][2];
        xp     = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(0,0) * x
                                                                               + t->getCoefficient(0,1) * y
                                                                               + t->getCoefficient(0,2) * z
                                                                               + t->getCoefficient(0,3));
        yp     = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(1,0) * x
                                                                               + t->getCoefficient(1,1) * y
                                                                               + t->getCoefficient(1,2) * z
                                                                               + t->getCoefficient(1,3));
        zp     = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(2,0) * x
                                                                               + t->getCoefficient(2,1) * y
                                                                               + t->getCoefficient(2,2) * z
                                                                               + t->getCoefficient(2,3));
        factor = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(3,0) * x
                                                                               + t->getCoefficient(3,1) * y
                                                                               + t->getCoefficient(3,2) * z
                                                                               + t->getCoefficient(3,3));
        points[i][0] = xp/factor;
        points[i][1] = yp/factor;
        points[i][2] = zp/factor;
    }
}

//------------------------------------------------------------------------------

} // namespace fwDataTools
