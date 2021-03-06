/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiWx/container/WxToolBarContainer.hpp"

namespace fwGuiWx
{
namespace container
{

//-----------------------------------------------------------------------------

WxToolBarContainer::WxToolBarContainer(::fwGui::GuiBaseObject::Key key) throw() : m_toolBar(0)
{}

//-----------------------------------------------------------------------------

WxToolBarContainer::~WxToolBarContainer() throw()
{}

//-----------------------------------------------------------------------------

void WxToolBarContainer::clean()
{
    SLM_ASSERT("Sorry, wxToolBar not yet initialized, cleaning impossible", m_toolBar);
    m_toolBar->ClearTools();
}

//-----------------------------------------------------------------------------

void WxToolBarContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxToolBar not yet initialized, cleaning impossible", m_toolBar);
    OSLM_ASSERT("ToolBar container must be empty ( " << m_toolBar->GetToolsCount() << " children).", m_toolBar->GetToolsCount() == 0);
    m_toolBar->Destroy();
    m_toolBar = 0;
}

//-----------------------------------------------------------------------------

void WxToolBarContainer::setWxToolBar(wxToolBar* toolBar)
{
    this->m_toolBar = toolBar;
}

//-----------------------------------------------------------------------------

wxToolBar* WxToolBarContainer::getWxToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiWx
