/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>

#include <fwGui/registry/macros.hpp>

#include "fwGuiQt/Application.hpp"


fwGuiRegisterMacro(::fwGuiQt::Application, ::fwGui::IApplication::REGISTRY_KEY );

namespace fwGuiQt
{
//-----------------------------------------------------------------------------

void Application::exit(int returncode)
{
    qApp->exit(returncode);
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQt



