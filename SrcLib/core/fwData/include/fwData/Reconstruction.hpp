/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RECONSTRUCTION_HPP_
#define _FWDATA_RECONSTRUCTION_HPP_

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

#include "fwData/config.hpp"

fwCampAutoDeclareDataMacro((fwData)(Reconstruction), FWDATA_API);

namespace fwData
{

class Image;
class Material;
class Mesh;

/**
 * @class Reconstruction
 * @brief This class defines a reconstruction object
 *
 * A reconstruction is represented by a triangular mesh, a material and an image.
 *
 * @see ::fwData::Image, ::fwData::Mesh, ::fwData::Material
 */

class FWDATA_CLASS_API Reconstruction : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Reconstruction)(::fwData::Object), (()), ::fwData::factory::New< Reconstruction >) ;

    fwCoreAllowSharedFromThis()

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Reconstruction(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Reconstruction();

    fwCampMakeFriendDataMacro((fwData)(Reconstruction));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);


    // Generator result---------------------------------------------------------
    fwGettersSettersDocMacro(IsVisible, bIsVisible, bool, the visibility of the reconstruction (true if visible));

    fwGettersSettersDocMacro(OrganName, sOrganName, std::string, organ name);

    fwGettersSettersDocMacro(StructureType, sStructureType, std::string, structure type);

    /**
     * @brief Get/Set the image associated with the acquisition
     */
    fwDataGetSetSptrMacro(Image, SPTR(::fwData::Image));

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    fwDataGetSetSptrMacro(Mesh, SPTR(::fwData::Mesh));

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    fwDataGetSetSptrMacro(Material, SPTR(::fwData::Material));

protected :

    //! true if this reconstruction is visible
    bool m_bIsVisible;

    //! Organ name
    std::string m_sOrganName;

    //! Structure type
    std::string m_sStructureType;

    //--------------------------------------------------------------------------


    //! Reconstruction's material
    SPTR(::fwData::Material) m_attrMaterial;

    //! Reconstruction's mask
    SPTR(::fwData::Image) m_attrImage;

    //! Reconstruction's mesh
    SPTR(::fwData::Mesh) m_attrMesh;

};

} // namespace fwData

#endif // _FWDATA_RECONSTRUCTION_HPP_
