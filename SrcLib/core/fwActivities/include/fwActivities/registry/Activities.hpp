#ifndef _FWACTIVITIES_REGISTRY_ACTIVITIES_HPP_
#define _FWACTIVITIES_REGISTRY_ACTIVITIES_HPP_

#include <map>
#include <string>

#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

#include <fwCore/BaseObject.hpp>
#include <fwCore/mt/types.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwActivities/config.hpp"

namespace fwData
{
class Vector;
}

namespace fwRuntime
{
struct Extension;
}


namespace fwActivities
{

namespace registry
{

typedef ::boost::property_tree::ptree ConfigType;

struct FWACTIVITIES_CLASS_API ActivityAppConfigParam
{
    ActivityAppConfigParam(){};
    FWACTIVITIES_API ActivityAppConfigParam(const ConfigType &config);

    bool isSeshat() const
    {
        return (by.substr(0,1) == "@") || (by.substr(0,1) == "!");
    }

    std::string replace;
    std::string by;
};

struct FWACTIVITIES_CLASS_API ActivityAppConfig
{
    typedef std::vector< ActivityAppConfigParam > ActivityAppConfigParamsType;

    ActivityAppConfig(){};
    FWACTIVITIES_API ActivityAppConfig(const ConfigType &config);

    std::string id;
    ActivityAppConfigParamsType parameters;
};

struct FWACTIVITIES_CLASS_API ActivityRequirementKey
{
    ActivityRequirementKey(){};
    FWACTIVITIES_API ActivityRequirementKey(const ConfigType &config);

    std::string key;
    std::string path;
};

struct FWACTIVITIES_CLASS_API ActivityRequirement
{
    ActivityRequirement(){};
    FWACTIVITIES_API ActivityRequirement(const ConfigType &config);

    typedef std::vector< ActivityRequirementKey > KeyType;

    std::string name;
    std::string type;
    unsigned int minOccurs;
    unsigned int maxOccurs;
    KeyType keys;
};

/**
 * @brief Holds Activities configuration.
 *
 * Example :
 * @verbatim
    <extension implements="::fwActivities::registry::Activities">
        <id>3DVisualization</id>
        <title>3D Visu</title>
        <tabinfo>3D MPR - !values.modelSeries.patient.name</tabinfo>
        <desc>Activity description ...</desc>
        <icon>Bundles/media_0-1/icons/icon-3D.png</icon>
        <requirements>
            <requirement name="param1" type="::fwData::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
            <requirement name="param2" type="::fwData::Mesh" maxOccurs="3" >
                <key>Item1</key>
                <key>Item2</key>
                <key>Item3</key>
            </requirement>
            <requirement name="param3" type="::fwData::Mesh" maxOccurs="*" />
            <requirement name="imageSeries" type="::fwMedData::ImageSeries" minOccurs="0" maxOccurs="2" />
            <requirement name="modelSeries" type="::fwMedData::ModelSeries" minOccurs="1" maxOccurs="1" />
            <!--# ...-->
        </requirements>
        <builder impl="::fwMedData::ActivitySeriesBuilder" />
        <appConfig id="3DVisualization">
            <parameters>
                <parameter replace="registeredImageUid" by="@values.param1" />
                <parameter replace="orientation" by="frontal" />
                <!--# ...-->
            </parameters>
        </appConfig>
    </extension>
 * @endverbatim
 */
struct FWACTIVITIES_CLASS_API ActivityInfo
{
    typedef std::vector< ActivityRequirement > RequirementsType;
    typedef std::pair<unsigned int, unsigned int> MinMaxType;
    typedef std::map< std::string, MinMaxType > RequirementsMinMaxCount;
    typedef std::map< std::string, unsigned int > DataCountType;

    ActivityInfo(){};

    FWACTIVITIES_API ActivityInfo(const SPTR(::fwRuntime::Extension) &ext);

    FWACTIVITIES_API bool usableWith(DataCountType dataCount) const;

    std::string id;
    std::string title;
    std::string description;
    std::string icon;
    std::string tabInfo;
    RequirementsType requirements;
    std::string builderImpl;
    std::string validatorImpl;
    ActivityAppConfig appConfig;

protected:

    RequirementsMinMaxCount m_requirementCount;
};


/**
 * @class Activities
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::activityReg::registry::Activities".
 *
 * @date 2012
 */
class FWACTIVITIES_CLASS_API Activities : public ::fwCore::BaseObject
{

public:
    typedef std::vector< ActivityInfo > ActivitiesType;

    fwCoreClassDefinitionsWithFactoryMacro( (Activities)(::fwCore::BaseObject), (()), new Activities ) ;

    /// Return the default global instance of Activities
    FWACTIVITIES_API static Activities::sptr getDefault();

    /// Destructor
    FWACTIVITIES_API virtual ~Activities();

    /**
     * @brief Parse bundle information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     * @{
     **/
    FWACTIVITIES_API void parseBundleInformation();
    FWACTIVITIES_API void parseBundleInformation(const std::vector< SPTR( ::fwRuntime::Extension ) > &extensions);
    ///@}

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    FWACTIVITIES_API const ActivityInfo getInfo( const std::string & extensionId ) const;

    /**
     * @brief Tests if we have information about operator
     * @note This method is thread safe.
     */
    FWACTIVITIES_API bool hasInfo( const std::string & extensionId ) const;

    /**
     * @brief Get the number of vector objects in the same type.
     */
    FWACTIVITIES_API ActivityInfo::DataCountType getDataCount( const SPTR(::fwData::Vector) &data ) const;

    /**
     * @brief Get all infos
     * @note This method is thread safe.
     */
    FWACTIVITIES_API ActivitiesType getInfos() const;

    /**
     * @brief Get available activities for given data.
     * @note This method is thread safe.
     */
    FWACTIVITIES_API ActivitiesType getInfos( const SPTR(::fwData::Vector) &data ) const;

    /**
     * @brief Get all keys
     * @note This method is thread safe.
     */
    FWACTIVITIES_API std::vector< std::string > getKeys() const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWACTIVITIES_API void clearRegistry();

protected :

    typedef std::map< std::string, ActivityInfo > Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Constructor
    FWACTIVITIES_API Activities();

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the app config parameters.
    static Activities::sptr s_activities;


};

} // namespace registry

} // namespace fwActivities

#endif // _FWACTIVITIES_REGISTRY_ACTIVITIES_HPP_


