/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IMENUITEMCALLBACK_HPP_
#define _FWGUI_IMENUITEMCALLBACK_HPP_

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Defines the menu item callback.
 * @class   IMenuItemCallback
 * 
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IMenuItemCallback : public ::fwGui::GuiBaseObject
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro ( (IMenuItemCallback)(::fwGui::GuiBaseObject) ) ;

    /**
     * @brief  Called method when callback is executed.
     */
    FWGUI_API virtual void execute() =0;

};

} // namespace fwGui

#endif /*_FWGUI_IMENUITEMCALLBACK_HPP_*/


