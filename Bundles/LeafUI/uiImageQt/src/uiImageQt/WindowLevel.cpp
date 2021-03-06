/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSignalMapper>
#include <QToolButton>
#include <QWidget>

#include <boost/math/special_functions/fpclassify.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGuiQt/widget/QRangeSlider.hpp>


#include "uiImageQt/WindowLevel.hpp"

namespace uiImage
{

fwServicesRegisterMacro( ::gui::editor::IEditor , ::uiImage::WindowLevel , ::fwData::Image ) ;

//------------------------------------------------------------------------------

WindowLevel::WindowLevel() throw()
{
    m_widgetDynamicRangeMin   = -1024.;
    m_widgetDynamicRangeWidth =  4000.;
    m_autoWindowing = false;
    m_imageMin = -200;
    m_imageMax = 300;
    m_isNotifying = false;
    m_useImageGreyLevelTF = false;

    //this->installTFSelectionEventHandler(this);
    //this->addNewHandledEvent(::fwComEd::ImageMsg::BUFFER);
    //this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
}

//------------------------------------------------------------------------------

WindowLevel::~WindowLevel() throw()
{}

//------------------------------------------------------------------------------

void WindowLevel::starting() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget * const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QGridLayout* layout = new QGridLayout();

    m_valueTextMin = new QLineEdit( container );
    QDoubleValidator *minValidator = new QDoubleValidator(m_valueTextMin);
    m_valueTextMin->setValidator(minValidator);

    m_valueTextMax = new QLineEdit( container );
    QDoubleValidator*  maxValidator = new QDoubleValidator(m_valueTextMax);
    m_valueTextMax->setValidator(maxValidator);

    m_rangeSlider = new ::fwGuiQt::widget::QRangeSlider(container);

    m_toggleTFButton = new QToolButton(container);
    QIcon ico;
    QString squareIcon("Bundles/uiImageQt_" UIIMAGEQT_VER "/square.png");
    QString rampIcon("Bundles/uiImageQt_" UIIMAGEQT_VER "/ramp.png");
    ico.addPixmap(QPixmap(squareIcon), QIcon::Normal,QIcon::On);
    ico.addPixmap(QPixmap(rampIcon), QIcon::Normal,QIcon::Off);
    m_toggleTFButton->setIcon(ico);
    m_toggleTFButton->setCheckable(true);

    m_toggleAutoButton = new QToolButton(container);
    QIcon icon;
    QString windo("Bundles/uiImageQt_" UIIMAGEQT_VER "/windowing.svg");
    icon.addFile(windo, QSize(), QIcon::Normal,QIcon::On);
    QString nowindo("Bundles/uiImageQt_" UIIMAGEQT_VER "/nowindowing.svg");
    icon.addFile(nowindo, QSize(), QIcon::Normal,QIcon::Off);
    m_toggleAutoButton->setIcon(icon);
    m_toggleAutoButton->setToolTip("Automatic Windowing");
    m_toggleAutoButton->setCheckable(true);
    m_toggleAutoButton->setChecked(m_autoWindowing);

    m_dynamicRangeSelection = new QToolButton(container);
    m_dynamicRangeSelection->setPopupMode(QToolButton::InstantPopup);

    m_dynamicRangeMenu = new QMenu(m_dynamicRangeSelection);
    QAction *action1 = m_dynamicRangeMenu->addAction( "-1024; 1023" );
    QAction *action2 = m_dynamicRangeMenu->addAction( "-100; 300" );
    QAction *action3 = m_dynamicRangeMenu->addAction( "Fit W/L" );
    QAction *action4 = m_dynamicRangeMenu->addAction( "Fit Data" ); // TODO
    //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
    m_dynamicRangeSelection->setMenu(m_dynamicRangeMenu);

    action1->setData(QVariant(1));
    action2->setData(QVariant(2));
    action3->setData(QVariant(3));
    action4->setData(QVariant(4));
    //action5->setData(QVariant(5));


    layout->addWidget( m_rangeSlider,  0, 0, 1, -1 );
    layout->addWidget( m_valueTextMin, 1, 0 );
    layout->addWidget( m_toggleTFButton, 1, 1 );
    layout->addWidget( m_toggleAutoButton, 1, 2 );
    layout->addWidget( m_dynamicRangeSelection, 1, 3 );
    layout->addWidget( m_valueTextMax, 1, 4 );


    container->setLayout( layout );
    this->updating();

    m_dynamicRangeSignalMapper = new QSignalMapper(this);

    QObject::connect(m_valueTextMin, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_valueTextMax, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_rangeSlider, SIGNAL(sliderRangeEdited( double, double )) , this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::connect(m_toggleTFButton, SIGNAL(toggled( bool )), this, SLOT(onToggleTF( bool )));
    QObject::connect(m_toggleAutoButton, SIGNAL(toggled( bool )), this, SLOT(onToggleAutoWL( bool )));
    QObject::connect(m_dynamicRangeSelection, SIGNAL(triggered( QAction * )), this, SLOT(onDynamicRangeSelectionChanged( QAction * )));

    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void WindowLevel::stopping() throw(::fwTools::Failed)
{
    this->removeTFObserver();
    QObject::disconnect(m_dynamicRangeSelection, SIGNAL(triggered( QAction * )), this, SLOT(onDynamicRangeSelectionChanged( QAction * )));
    QObject::disconnect(m_toggleTFButton, SIGNAL(toggled( bool )), this, SLOT(onToggleTF( bool )));
    QObject::disconnect(m_rangeSlider, SIGNAL(sliderRangeEdited( double, double )), this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::disconnect(m_valueTextMin, SIGNAL(editingFinished( QString )), this, SLOT(onTextEditingFinished( QString )));
    QObject::disconnect(m_valueTextMax, SIGNAL(editingFinished( QString )), this, SLOT(onTextEditingFinished( QString )));

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    // deletes contained widgets
    this->getContainer()->clean();

    this->destroy();
}

//------------------------------------------------------------------------------

void WindowLevel::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("config");
    SLM_ASSERT("WindowLevel config is empty.", configs.size() == 1);

    ::fwRuntime::ConfigurationElement::sptr config = configs.front();
    if (config->hasAttribute("autoWindowing"))
    {
        std::string autoWindowing = config->getExistingAttributeValue("autoWindowing");
        SLM_ASSERT("Bad value for 'autoWindowing' attribute. It must be 'yes' or 'no'!", autoWindowing == "yes" || autoWindowing == "no");
        m_autoWindowing = (autoWindowing == "yes");
    }

    if ( config->hasAttribute("useImageGreyLevelTF") )
    {
        std::string useImageGreyLevelTF = config->getExistingAttributeValue("useImageGreyLevelTF");
        SLM_ASSERT("Bad value for 'useImageGreyLevelTF' attribute. It must be 'yes' or 'no'!", useImageGreyLevelTF == "yes" || useImageGreyLevelTF == "no");
        m_useImageGreyLevelTF = (useImageGreyLevelTF == "yes");
    }

    this->parseTFConfig(config);
}

//------------------------------------------------------------------------------

void WindowLevel::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    this->setEnabled(imageIsValid);

    this->updateTransferFunction(image, this->getSptr());
    if(imageIsValid)
    {
        this->updateImageInfos(image);

        // test if service must use image grey level tf ( when another tf pool is defined )
        if( m_useImageGreyLevelTF &&
            ! this->getTFSelectionFwID().empty() )
        {
            ::fwData::TransferFunction::sptr newTF = this->getImageGreyLevelTF();
            this->swapCurrentTFAndNotify( newTF );
            m_toggleTFButton->setCheckable(true);
        }

        if(m_autoWindowing)
        {
            double min, max;
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            this->updateImageWindowLevel(min, max);
        }

        ::fwData::TransferFunction::sptr pTF = this->getTransferFunction();
        SLM_ASSERT("TransferFunction null pointer", pTF);
        ::fwData::TransferFunction::TFValuePairType minMax = pTF->getWLMinMax();
        this->onImageWindowLevelChanged( minMax.first, minMax.second );
    }
}

//------------------------------------------------------------------------------

void WindowLevel::swapping() throw(::fwTools::Failed)
{
    this->removeTFObserver();
    this->updating();
    this->installTFObserver( this->getSptr() );
}
//------------------------------------------------------------------------------

void WindowLevel::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->upadteTFObserver(msg, this->getSptr());

    if (msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ))
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

        bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        if (imageIsValid)
        {
            this->updateImageInfos(image);
            this->updateTransferFunction(image, this->getSptr());


            if(m_autoWindowing)
            {
                double min, max;
                ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
                this->updateImageWindowLevel(min, max);
            }

            ::fwData::TransferFunction::sptr pTF = this->getTransferFunction();
            SLM_ASSERT("TransferFunction null pointer", pTF);
            ::fwData::TransferFunction::TFValuePairType minMax = pTF->getWLMinMax();
            this->onImageWindowLevelChanged( minMax.first, minMax.second );
        }
        this->setEnabled(imageIsValid);

    }
    if (msg->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ))
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

        bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        SLM_ASSERT("Image is not valid",imageIsValid);
        this->updateTransferFunction(image, this->getSptr());

        ::fwData::TransferFunction::sptr pTF = this->getTransferFunction();
        SLM_ASSERT("TransferFunction null pointer", pTF);
        ::fwData::TransferFunction::TFValuePairType minMax = pTF->getWLMinMax();
        this->onImageWindowLevelChanged( minMax.first, minMax.second );
    }
}

//------------------------------------------------------------------------------

void WindowLevel::info( std::ostream & _sstream )
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

WindowLevel::WindowLevelMinMaxType WindowLevel::getImageWindowMinMax()
{
    ::fwData::TransferFunction::sptr pTF = this->getTransferFunction();
    SLM_ASSERT("TransferFunction null pointer", pTF);

    return pTF->getWLMinMax();
}

//------------------------------------------------------------------------------
void WindowLevel::updateWidgetMinMax(double _imageMin, double _imageMax)
{
    double rangeMin = this->fromWindowLevel(_imageMin);
    double rangeMax = this->fromWindowLevel(_imageMax);

    m_rangeSlider->setPos(rangeMin, rangeMax);
}

//------------------------------------------------------------------------------

double WindowLevel::fromWindowLevel(double val)
{
    double valMin = m_widgetDynamicRangeMin;
    double valMax = valMin + m_widgetDynamicRangeWidth;

    valMin = std::min(val, valMin);
    valMax = std::max(val, valMax);

    this->setWidgetDynamicRange(valMin, valMax);

    double res = (val - m_widgetDynamicRangeMin) / m_widgetDynamicRangeWidth;
    return res;
}

//------------------------------------------------------------------------------

double WindowLevel::toWindowLevel(double _val)
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------

void  WindowLevel::updateImageWindowLevel(double _imageMin, double _imageMax)
{
    m_imageMin = _imageMin;
    m_imageMax = _imageMax;

    if (!m_isNotifying)
    {
        this->notifyWindowLevel(_imageMin, _imageMax);
    }
}

//------------------------------------------------------------------------------

void  WindowLevel::onWindowLevelWidgetChanged(double _min, double _max)
{
    double imageMin = this->toWindowLevel(_min);
    double imageMax = this->toWindowLevel(_max);
    this->updateImageWindowLevel(imageMin, imageMax);
    this->updateTextWindowLevel (imageMin, imageMax);
}

//------------------------------------------------------------------------------

void WindowLevel::onDynamicRangeSelectionChanged(QAction *action)
{
    WindowLevelMinMaxType wl = this->getImageWindowMinMax();
    double min = m_widgetDynamicRangeMin;
    double max = m_widgetDynamicRangeWidth + min;
    int index = action->data().toInt();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    switch (index)
    {
        case 0:
            break;
        case 1: // -1024; 1023
            min = -1024;
            max =  1023;
            break;
        case 2: // -100; 300
            min = -100;
            max =  300;
            break;
        case 3: // Fit Window/Level
            min = std::min(wl.first, wl.second);
            max = std::max(wl.first, wl.second);
            break;
        case 4: // Fit Image Range
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            break;
        case 5: // Custom : TODO
            break;
        default:
            SLM_ASSERT("Unknown range selector index", 0);
    }

    this->setWidgetDynamicRange(min, max);
    this->updateWidgetMinMax(wl.first, wl.second);
}

//------------------------------------------------------------------------------

void  WindowLevel::onImageWindowLevelChanged(double _imageMin, double _imageMax)
{
    this->updateWidgetMinMax( _imageMin, _imageMax );
    this->updateTextWindowLevel( _imageMin, _imageMax );
}

//------------------------------------------------------------------------------

// Check if service that registered the callback is still alive
struct WLCallback
{
    typedef void result_type;

    WLCallback(WindowLevel::sptr wl)
    {
        m_wl = wl;
    }

    void operator()()
    {
        if ( !m_wl.expired() )
        {
            m_wl.lock()->notifyWindowLevelCallback();
        }
    }

    WindowLevel::wptr m_wl;
};

void  WindowLevel::notifyWindowLevel(double _imageMin, double _imageMax)
{
    m_notifiedImageMin = _imageMin;
    m_notifiedImageMax = _imageMax;

    this->setWindowLevel(m_imageMin, m_imageMax);
    ::fwComEd::TransferFunctionMsg::sptr msg = this->notifyTFWindowing(this->getSptr());
    msg->setMessageCallback(::boost::bind( WLCallback(WindowLevel::dynamicCast(this->getSptr())) ));

    m_isNotifying = true;
}

//------------------------------------------------------------------------------

void  WindowLevel::notifyWindowLevelCallback()
{
    m_isNotifying = false;

    if (m_notifiedImageMin != m_imageMin || m_notifiedImageMax != m_imageMax)
    {
        this->updateImageWindowLevel(m_imageMin, m_imageMax);
    }
}

//------------------------------------------------------------------------------

void  WindowLevel::updateTextWindowLevel(double _imageMin, double _imageMax)
{
    m_valueTextMin->setText(QString("%1").arg(_imageMin));
    m_valueTextMax->setText(QString("%1").arg(_imageMax));
}

//------------------------------------------------------------------------------

void  WindowLevel::onToggleTF(bool squareTF)
{
    bool usedGreyLevelTF = false;

    ::fwData::TransferFunction::sptr oldTF = this->getTransferFunction();
    ::fwData::TransferFunction::sptr newTF;

    if( squareTF )
    {
        newTF = ::fwData::TransferFunction::New();
        ::fwData::TransferFunction::TFColor color(1.,1.,1.,1.);
        newTF->setName("SquareTF");
        newTF->addTFColor(0.0, color);
        newTF->addTFColor(1.0, color);
        newTF->setIsClamped(true);
    }
    else
    {
        // test if service must use image grey level tf ( when another tf pool is defined )
        if(     m_useImageGreyLevelTF &&
                ! this->getTFSelectionFwID().empty() )
        {
            newTF = this->getImageGreyLevelTF();
            usedGreyLevelTF = true;
        }
        else
        {
            newTF = ::fwData::TransferFunction::createDefaultTF();
        }
    }

    newTF->setWindow( oldTF->getWindow() );
    newTF->setLevel( oldTF->getLevel() );

    this->swapCurrentTFAndNotify( newTF );

    if ( usedGreyLevelTF )
    {
        this->notifyTFWindowing( this->getSptr() );
    }
}

//------------------------------------------------------------------------------

void  WindowLevel::onToggleAutoWL(bool autoWL)
{
     m_autoWindowing = autoWL;

     if (m_autoWindowing)
     {
         ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
         double min, max;
         ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
         this->updateImageWindowLevel(min, max);
         this->onImageWindowLevelChanged(min, max);
     }
}

//------------------------------------------------------------------------------

void  WindowLevel::onTextEditingFinished()
{
    double min, max;
    if(this->getWidgetDoubleValue(m_valueTextMin, min) && this->getWidgetDoubleValue(m_valueTextMax, max))
    {
        this->updateWidgetMinMax( min, max );
        this->updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------

bool WindowLevel::getWidgetDoubleValue(QLineEdit *widget, double &val)
{
    bool ok=false;
    val = widget->text().toDouble(&ok);

    QPalette palette;
    if (!ok)
    {
        palette.setBrush(QPalette::Base, QBrush(Qt::red));
    }
    else
    {
        palette.setBrush(QPalette::Base, QApplication::palette().brush(QPalette::Base));
    }
    widget->setPalette(palette);
    return ok;
}

//------------------------------------------------------------------------------

void WindowLevel::setEnabled(bool enable)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget * const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    container->setEnabled(enable);
}

//------------------------------------------------------------------------------

void WindowLevel::setWidgetDynamicRange(double min, double max)
{
    if(fabs(max - min) < 1.e-05)
    {
        max = min + 1.e-05;
    }
    m_widgetDynamicRangeMin = min;
    m_widgetDynamicRangeWidth = max - min;

    m_dynamicRangeSelection->setText(QString("%1, %2 ").arg(min).arg(max));
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr WindowLevel::getImageGreyLevelTF()
{
    ::fwData::TransferFunction::sptr defaultTF;

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    // Create pool
    ::fwComEd::helper::Image helper(image);
    helper.createTransferFunctionPool( this->getSptr() ); // do nothing if image tf pool already exist

    // Get pool
    const std::string poolFieldName = ::fwComEd::Dictionary::m_transferFunctionCompositeId;
    ::fwData::Composite::sptr pool = image->getField< ::fwData::Composite >(poolFieldName);

    // Get image default image tf
    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    defaultTF = ::fwData::TransferFunction::dynamicCast((*pool)[defaultTFName]);

    return defaultTF;
}

//------------------------------------------------------------------------------

void WindowLevel::swapCurrentTFAndNotify( ::fwData::TransferFunction::sptr newTF )
{
    // Change TF
    std::string tfSelectionFwID = this->getTFSelectionFwID();
    ::fwData::Composite::sptr pool = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( tfSelectionFwID ) );
    OSLM_ASSERT( "Sorry, object with fwID " << tfSelectionFwID << " doesn't exist.", pool );
    ::fwComEd::helper::Composite compositeHelper( pool );
    compositeHelper.swap( this->getSelectedTFKey(), newTF );

    // Notify change
    compositeHelper.notify( this->getSptr() );
}

//------------------------------------------------------------------------------

}

