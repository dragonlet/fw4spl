/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TRANSFORMATIONMATRIX3DMSG_HPP_
#define _FWCOMED_TRANSFORMATIONMATRIX3DMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for TransformationMatrix3D : store modification information
 * @class   TransformationMatrix3DMsg
 * 
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API TransformationMatrix3DMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((TransformationMatrix3DMsg)(::fwServices::ObjectMsg), ( ( ) ),
                                           ::fwServices::factory::message::New< TransformationMatrix3DMsg >);

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string MATRIX_IS_MODIFIED;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API TransformationMatrix3DMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~TransformationMatrix3DMsg() throw();

};


} // namespace fwComEd

#endif //_FWCOMED_TRANSFORMATIONMATRIX3DMSG_HPP_

