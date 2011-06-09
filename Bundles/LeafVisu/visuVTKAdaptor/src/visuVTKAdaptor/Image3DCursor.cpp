/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/format.hpp>


#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/Base.hpp>

#include <vtkActor.h>
#include <vtkParametricBoy.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

#include "fwRenderVTK/vtk/Helpers.hpp"

#include "visuVTKAdaptor/Image3DCursor.hpp"

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Image3DCursor, ::fwData::Image ) ;


namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Image3DCursor::Image3DCursor() throw()
: m_priority(.6)
, m_cursorPolyData( vtkPolyData::New() )
, m_cursorMapper  ( vtkPolyDataMapper::New() )
, m_cursorActor(    vtkActor::New() )
{
    //handlingEventOff();
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
}

//------------------------------------------------------------------------------

Image3DCursor::~Image3DCursor() throw()
{
}

//------------------------------------------------------------------------------
void Image3DCursor::setVisibility( bool visibility )
{
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void Image3DCursor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
}


//------------------------------------------------------------------------------

void Image3DCursor::doStart() throw(fwTools::Failed)
{

    buildPolyData();
    m_cursorMapper->SetInput( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);
    m_cursorActor->GetProperty()->SetColor(1,1,1);
    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);


}

//------------------------------------------------------------------------------

void Image3DCursor::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    int index[3] = {
        *m_sagittalIndex,
        *m_frontalIndex,
        *m_axialIndex
    };
    double center[3];
    sliceIndexToWorld(index, center);
    updateCursorPosition(center);
}

//------------------------------------------------------------------------------

void Image3DCursor::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void Image3DCursor::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void Image3DCursor::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if ( msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) )
    {
        ::fwComEd::ImageMsg::dynamicConstCast(msg)->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
        int index[3] = {
            *m_sagittalIndex,
            *m_frontalIndex,
            *m_axialIndex
        };
        double center[3];
        sliceIndexToWorld(index, center);
        updateCursorPosition(center);
    }
}

//------------------------------------------------------------------------------

void Image3DCursor::updateCursorPosition( double world[3] )
{
    m_cursorActor->SetPosition(world);

    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void Image3DCursor::buildPolyData()
{
    // point are stored Left,right,up,down
    vtkSmartPointer<vtkSphereSource> polySource = vtkSmartPointer<vtkSphereSource>::New();
    polySource->SetCenter(0.0, 0.0, 0.0);
    polySource->SetRadius(8.0);
    polySource->SetPhiResolution(4);
    polySource->SetThetaResolution(8);

    //vtkSmartPointer<vtkParametricBoy> boyFunc = vtkSmartPointer<vtkParametricBoy>::New();
    //vtkSmartPointer<vtkParametricFunctionSource> polySource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    //polySource->SetParametricFunction(boyFunc);

    polySource->SetOutput(m_cursorPolyData);
    polySource->Update();

    this->setVtkPipelineModified();
}


} //namespace visuVTKAdaptor