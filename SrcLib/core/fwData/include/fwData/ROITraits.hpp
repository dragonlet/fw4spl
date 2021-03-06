/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_ROITRAITS_HPP__
#define __FWDATA_ROITRAITS_HPP__

#include "fwData/Node.hpp"
#include "fwData/StructureTraits.hpp"
#include "fwData/factory/new.hpp"


fwCampAutoDeclareDataMacro((fwData)(ROITraits), FWDATA_API);

namespace fwData
{
/**
 * @class   ROITraits
 * @brief   This class defines ROI traits.
 * 
 * @date    2007-2011.
 *
 * A ROI traits contains :
 *  - an identifier
 *  - an evaluated expression : ex. inter(skin,liver)
 *  - a ROI mask node used for ROI, the only case where sptr can be null is when evaluated expression correspond to "W" ( W <=> World <=> no roi constraint ).
 *  - a structure traits associated to the ROI
 *
 * @note Several ROITraits can be associated to the same structure.
 * @see StructureTraits
 */
class FWDATA_CLASS_API ROITraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ROITraits)(::fwData::Object), (()), ::fwData::factory::New< ROITraits >) ;
    fwCampMakeFriendDataMacro((fwData)(ROITraits));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ROITraits(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ROITraits();

    fwGettersSettersDocMacro(Identifier, identifier, std::string, the ROIs identifier);

    fwGettersSettersDocMacro(EvaluatedExp, evaluatedExp, StructureTraits::ROIExpression, the ROI evaluated expression);

    /// Set the ROI mask node used for ROI
    FWDATA_API void setMaskOpNode( ::fwData::Node::sptr maskOpNode );

    /// Get the ROI mask node used for ROI, the only case where sptr can be null is when m_evaluatedExp correspond to "W" ( W <=> World <=> no roi constraint ).
    FWDATA_API ::fwData::Node::sptr getMaskOpNode();

    /// Set the associated structure traits
    FWDATA_API void setStructureTraits( ::fwData::StructureTraits::sptr structureTraits );

    /// Get the associated structure traits
    FWDATA_API ::fwData::StructureTraits::sptr getStructureTraits();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

private:

    /// ROI identifier
    std::string m_identifier;

    /// mask node used for ROITraits
    ::fwData::Node::sptr m_maskOpNode;

    /// associated structure traits
    ::fwData::StructureTraits::sptr m_structureTraits;

    /// roi evaluated expression
    StructureTraits::ROIExpression m_evaluatedExp;
};

} // namespace fwData

#endif // __FWDATA_ROITRAITS_HPP__
