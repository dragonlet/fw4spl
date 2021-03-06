/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QString>
#include <QVBoxLayout>
#include <QGroupBox>

#include <fwCore/base.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/ModelSeriesMsg.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiMedDataQt/editor/SModelSeriesList.hpp"

namespace uiMedData
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor , ::uiMedData::editor::SModelSeriesList , ::fwMedData::ModelSeries) ;


SModelSeriesList::SModelSeriesList() throw() : m_enableHideAll(true)
{}

//------------------------------------------------------------------------------

SModelSeriesList::~SModelSeriesList() throw()
{}

//------------------------------------------------------------------------------

void SModelSeriesList::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);


    if (m_enableHideAll)
    {
        m_showCheckBox = new QCheckBox( tr("Hide all organs"));
        m_showCheckBox->setToolTip(tr("Show or hide all organs"));
        layout->addWidget( m_showCheckBox, 0 );
        QObject::connect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    }

    m_organChoice = new QListWidget();

    layout->addWidget( m_organChoice, 1 );

    container->setLayout( layout );

    QObject::connect(m_organChoice, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onOrganChoiceVisibility(QListWidgetItem*)));
    QObject::connect(m_organChoice, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    this->updating();
}

//------------------------------------------------------------------------------

void SModelSeriesList::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if(m_showCheckBox)
    {
        QObject::disconnect(m_showCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowReconstructions(int)));
    }

    QObject::disconnect(m_organChoice, SIGNAL(itemClicked (QListWidgetItem * )),
            this, SLOT(onOrganChoiceVisibility(QListWidgetItem *)));
    QObject::disconnect(m_organChoice, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SModelSeriesList::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    if( m_configuration->findConfigurationElement( "enable_hide_all" ) )
    {
        const std::string& hide = m_configuration->findConfigurationElement("enable_hide_all")->getValue();
        SLM_ASSERT("'enable_hide_all' attribute value must be 'true' or 'false' (found '" + hide + "')",
                hide == "true" || hide == "false");
        m_enableHideAll = (hide == "true");
    }

}

//------------------------------------------------------------------------------

void SModelSeriesList::updating() throw(::fwTools::Failed)
{
    this->updateReconstructions();
    this->refreshVisibility();
}

//------------------------------------------------------------------------------

void SModelSeriesList::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void SModelSeriesList::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::ModelSeriesMsg::csptr acquisitionMsg = ::fwComEd::ModelSeriesMsg::dynamicConstCast( msg ) ;
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent(::fwComEd::ModelSeriesMsg::SHOW_RECONSTRUCTIONS) )
        {
            this->updating();
        }
        else if ( acquisitionMsg->hasEvent(::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION) )
        {
            this->updating();
        }
        else if ( acquisitionMsg->hasEvent(::fwComEd::ModelSeriesMsg::REMOVED_RECONSTRUCTIONS) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SModelSeriesList::updateReconstructions()
{
    m_organChoice->blockSignals(true);
    m_organChoice->clear();
    m_map.clear();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    SLM_ASSERT("container not instanced", container);
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    bool hasReconstructions = !modelSeries->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    if(hasReconstructions)
    {
        BOOST_FOREACH(::fwData::Reconstruction::sptr rec, modelSeries->getReconstructionDB())
        {
            m_map[ rec->getOrganName() ] = rec;
        }

        for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter )
        {
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString((*iter).first), m_organChoice);
            item->setCheckState(Qt::Unchecked);
            m_organChoice->addItem(item);
        }

        const bool showAllRec
            = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

        if(m_showCheckBox)
        {
            m_showCheckBox->setCheckState(showAllRec ? Qt::Unchecked : Qt::Checked );
            m_organChoice->setEnabled(m_showCheckBox->checkState() == Qt::Unchecked);
        }
    }

    m_organChoice->blockSignals(false);
}

//------------------------------------------------------------------------------

void SModelSeriesList::onCurrentItemChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
    SLM_ASSERT( "Current selected item is null", current );

    QListWidgetItem * currentItem = m_organChoice->currentItem();
    std::string organSelected = currentItem->text().toStdString();

    if( m_map.find(organSelected) != m_map.end() )
    {
        ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
        ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;

        ::fwComEd::ModelSeriesMsg::sptr msg = ::fwComEd::ModelSeriesMsg::New();
        msg->addEvent( ::fwComEd::ModelSeriesMsg::NEW_RECONSTRUCTION_SELECTED, ::fwData::String::New(rec->getID()));
        ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onOrganChoiceVisibility(QListWidgetItem * item )
{
    std::string organSelected = item->text().toStdString();
    ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;
    SLM_ASSERT("rec not instanced", rec);

    bool itemIsChecked = (item->checkState() == Qt::Checked);

    if (rec->getIsVisible() != itemIsChecked)
    {
        rec->setIsVisible(item->checkState());

        ::fwComEd::ReconstructionMsg::sptr msg = ::fwComEd::ReconstructionMsg::New();
        msg->addEvent( ::fwComEd::ReconstructionMsg::VISIBILITY ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), rec, msg);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesList::onShowReconstructions(int state )
{
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
    modelSeries->setField("ShowReconstructions",  ::fwData::Boolean::New(state == Qt::Unchecked) );

    ::fwComEd::ModelSeriesMsg::sptr msg = ::fwComEd::ModelSeriesMsg::New();
    msg->addEvent( ::fwComEd::ModelSeriesMsg::SHOW_RECONSTRUCTIONS );
    ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);

    m_organChoice->setEnabled(state == Qt::Unchecked);
}

//------------------------------------------------------------------------------

void SModelSeriesList::refreshVisibility()
{
    int item = 0;
    for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter, ++item )
    {
        QListWidgetItem *  currentItem = m_organChoice->item (item);
        currentItem->setCheckState( iter->second->getIsVisible() ? Qt::Checked : Qt::Unchecked );
    }
}

} // namespace editor
} // namespace uiMedData

