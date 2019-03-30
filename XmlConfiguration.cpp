#include "XmlConfiguration.h"

#if defined(_WIN32)

#include <Misc/Exceptions.h>
#include <Msxmlx/Msxmlx.h>
#include <Vkx/Vkx.h>
#include <Wx/Wx.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cassert>
#include <iomanip>
#include <sstream>

namespace
{
glm::vec3 GetVectorSubElement(IXMLDOMElement * element,
                              char const * name,
                              glm::vec3 const & defaultVec = { 0.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> subElement;

    hr = Msxmlx::GetSubElement(element, name, &subElement);
    if (subElement)
    {
        //    <xsd:complexType name="vector3">
        //        <xsd:all>
        //            <xsd:element name="X" type="xsd:float" />
        //            <xsd:element name="Y" type="xsd:float" />
        //            <xsd:element name="Z" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        glm::vec3 v;

        v.x = Msxmlx::GetFloatSubElement(subElement, "X", defaultVec.x);
        v.y = Msxmlx::GetFloatSubElement(subElement, "Y", defaultVec.y);
        v.z = Msxmlx::GetFloatSubElement(subElement, "Z", defaultVec.z);

        return v;
    }
    else
    {
        return defaultVec;
    }
}

glm::vec4 GetPlaneSubElement(IXMLDOMElement * element,
                             char const * name,
                             glm::vec4 const & defaultPlane = { 0.0f, 1.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> subElement;

    hr = Msxmlx::GetSubElement(element, name, &subElement);
    if (subElement)
    {
        //    <xsd:complexType name="plane">
        //        <xsd:all>
        //            <xsd:element name="A" type="xsd:float" />
        //            <xsd:element name="B" type="xsd:float" />
        //            <xsd:element name="C" type="xsd:float" />
        //            <xsd:element name="D" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        glm::vec4 plane;

        plane.x = Msxmlx::GetFloatSubElement(subElement, "A", defaultPlane.x);
        plane.y = Msxmlx::GetFloatSubElement(subElement, "B", defaultPlane.y);
        plane.z = Msxmlx::GetFloatSubElement(subElement, "C", defaultPlane.z);
        plane.w = Msxmlx::GetFloatSubElement(subElement, "D", defaultPlane.w);

        return plane;
    }
    else
    {
        return defaultPlane;
    }
}

glm::vec4 GetRgbaSubElement(IXMLDOMElement * element,
                            char const * name,
                            glm::vec4 const & defaultRgba = { 0.0f, 0.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> subElement;

    hr = Msxmlx::GetSubElement(element, name, &subElement);
    if (subElement)
    {
        //    <xsd:complexType name="glcolor">
        //        <xsd:all>
        //            <xsd:element name="R" type="glcolorelement" />
        //            <xsd:element name="G" type="glcolorelement" />
        //            <xsd:element name="B" type="glcolorelement" />
        //            <xsd:element name="A" type="glcolorelement" minOccurs="0" default="1.0" />
        //        </xsd:all>
        //    </xsd:complexType>

        glm::vec4 rgba;

        rgba.x = Msxmlx::GetFloatSubElement(subElement, "R", defaultRgba.x);
        rgba.y = Msxmlx::GetFloatSubElement(subElement, "G", defaultRgba.y);
        rgba.z = Msxmlx::GetFloatSubElement(subElement, "B", defaultRgba.z);
        rgba.w = Msxmlx::GetFloatSubElement(subElement, "A", defaultRgba.w);

        return rgba;
    }
    else
    {
        return defaultRgba;
    }
}

glm::quat GetQuatSubElement(IXMLDOMElement *  element,
                            char const *      name,
                            glm::quat const & defaultQuat = glm::quat())
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> subElement;

    hr = Msxmlx::GetSubElement(element, name, &subElement);
    if (subElement)
    {
        //    <xsd:complexType name="quaternion">
        //        <xsd:all>
        //            <xsd:element name="X" type="xsd:float" />
        //            <xsd:element name="Y" type="xsd:float" />
        //            <xsd:element name="Z" type="xsd:float" />
        //            <xsd:element name="W" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        glm::quat q;

        q.x = Msxmlx::GetFloatSubElement(subElement, "X", defaultQuat.x);
        q.y = Msxmlx::GetFloatSubElement(subElement, "Y", defaultQuat.y);
        q.z = Msxmlx::GetFloatSubElement(subElement, "Z", defaultQuat.z);
        q.w = Msxmlx::GetFloatSubElement(subElement, "W", defaultQuat.w);

        return q;
    }
    else
    {
        return defaultQuat;
    }
}

glm::vec4 GetPackedColorSubElement(IXMLDOMElement * element,
                                   char const *     name,
                                   uint32_t         defaultRgba = 0xffffffff)
{
    uint32_t rgba = Msxmlx::GetHexSubElement(element, name, defaultRgba);

    glm::vec4 color;
    for (int i = 3; i >= 0; --i)
    {
        color[i] = float(rgba & 0xff) / 255.0f;
        rgba   >>= 8;
    }

    return color;
}

#if defined(_DEBUG)

std::ostream & operator <<(std::ostream & s, glm::vec4 const & q)
{
    s << q.x << ','
      << q.y << ','
      << q.z << ','
      << q.w;

    return s;
}
std::ostream & operator <<(std::ostream & s, glm::quat const & q)
{
    s << q.x << ','
      << q.y << ','
      << q.z << ','
      << q.w;

    return s;
}

std::ostream & operator <<(std::ostream & s, glm::vec3 const & v)
{
    s << v.x << ','
      << v.y << ','
      << v.z;

    return s;
}

#endif // defined( _DEBUG )
} // anonymous namespace

namespace Confetti
{
//! @param  path    Path to the file containing the configuration source
XmlConfiguration::XmlConfiguration(char const * path)
{
    assert(path);
    if (!load(path))
        throw ConstructorFailedException("XmlConfiguration:: Failed to load XML file");
}

//! @param  document    Configuration source
XmlConfiguration::XmlConfiguration(IXMLDOMDocument2 * document)
{
    assert(document);
    if (!load(document))
        throw ConstructorFailedException("XmlConfiguration:: Failed to load XML document");
}

//! @param  document    XML document to be filled with the configuration
//!
//! @return     true, if successful
bool XmlConfiguration::toXml(IXMLDOMDocument2 * document)
{
    return false;
}

bool XmlConfiguration::load(char const * path)
{
    Wx::ComInitializer cominit;

    CComPtr<IXMLDOMDocument2> document;
    document.CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER);

    HRESULT      hr;
    VARIANT_BOOL status;

    hr = document->put_async(VARIANT_FALSE);
    if (FAILED(hr))
    {
//        OutputDebugString( "XmlConfiguration::Load: Failed to set async property\n" );
        return false;
    }

    hr = document->load(CComVariant(path), &status);
    if (FAILED(hr))
    {
//        trace( "XmlConfiguration::Load: load( \"%s\" ) failed.\n", path );
        return false;
    }

    if (status != VARIANT_TRUE)
    {
#if defined(_DEBUG)
        CComPtr<IXMLDOMParseError> error;
        CComBSTR reason;

        hr = document->get_parseError(&error);
        hr = error->get_reason(&reason);
//        trace( "XmlConfiguration::Load: Failed to load DOM from '%s'\n%S\n", path, reason );
#endif      // defined( _DEBUG )

        return false;
    }

    return load(document);
}

bool XmlConfiguration::load(IXMLDOMDocument2 * document)
{
    HRESULT hr;

    // Parse the XML

    //    <xsd:element name="ParticleSystem" type="particlesystem" />
    //    <xsd:complexType name="particlesystem">
    //        <xsd:choice minOccurs="0" maxOccurs="unbounded">
    //            <xsd:element name="Emitter" type="emitter" />
    //            <xsd:element name="Environment" type="environment" />
    //            <xsd:element name="Appearance" type="appearance" />
    //            <xsd:element name="Volume" type="volume" />
    //            <xsd:element name="SurfaceList" type="surfacelist" />
    //            <xsd:element name="ClipPlaneList" type="clipplanelist" />
    //        </xsd:choice>
    //    </xsd:complexType>

    CComPtr<IXMLDOMElement> root;
    hr = document->get_documentElement(&root);
    if (FAILED(hr))
    {
//        trace( "XmlConfiguration::Load: Failed to get document element\n" );
        return false;
    }

    Msxmlx::ForEachSubElement(root,
                              [this] (IXMLDOMElement * element) { return processSurfaceList(element, surfaceLists_); });
    Msxmlx::ForEachSubElement(root, [this] (IXMLDOMElement * element) { return processClipPlaneList(element, clipPlaneLists_); });
    Msxmlx::ForEachSubElement(root, [this] (IXMLDOMElement * element) { return processEnvironment(element, environments_); });
    Msxmlx::ForEachSubElement(root, [this] (IXMLDOMElement * element) { return processAppearance(element, appearances_); });
    Msxmlx::ForEachSubElement(root, [this] (IXMLDOMElement * element) { return processVolume(element, emitterVolumes_); });
    Msxmlx::ForEachSubElement(root, [this] (IXMLDOMElement * element) { return processEmitter(element, emitters_); });

    return true;
}

bool XmlConfiguration::processSurfaceList(IXMLDOMElement * element, SurfaceListMap & lists)
{
    HRESULT  hr;
    CComBSTR tag;
    hr = element->get_tagName(&tag);

    if (tag == CComBSTR("SurfaceList"))
    {
        //    <xsd:complexType name="surfacelist">
        //        <xsd:sequence>
        //            <xsd:element name="Surface" type="surface" minOccurs="0" maxOccurs="unbounded" />
        //        </xsd:sequence>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        SurfaceList planes;

        planes.name_ =   Msxmlx::GetStringAttribute(element, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "SurfaceList: " << planes.name_ << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the Surfaces in the list
        Msxmlx::ForEachSubElement(element, [this, &planes] (IXMLDOMElement * element) {
                                      return processSurface(element, planes);
                                  });

        lists.emplace(planes.name_, planes);
    }
    return true;
}

bool XmlConfiguration::processSurface(IXMLDOMElement * element, SurfaceList & list)
{
//    <xsd:complexType name="surface">
//        <xsd:all>
//            <xsd:element name="Plane" type="plane" />
//            <xsd:element name="Dampening" type="xsd:float" />
//        </xsd:all>
//    </xsd:complexType>

    Surface surface;
    surface.plane_     = GetPlaneSubElement(element, "Plane");
    surface.dampening_ = Msxmlx::GetFloatSubElement(element, "Dampening");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    Surface: ["
            << surface.plane_.x << " "
            << surface.plane_.y << " "
            << surface.plane_.z << " "
            << surface.plane_.w << "], "
            << surface.dampening_
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this Surface to the list

    list.planes_.push_back(surface);

    return true;
}

bool XmlConfiguration::processClipPlaneList(IXMLDOMElement * element, ClipPlaneListMap & lists)
{
    HRESULT  hr;
    CComBSTR tag;
    hr = element->get_tagName(&tag);

    if (tag == CComBSTR("ClipPlaneList"))
    {
        //    <xsd:complexType name="clipplanelist">
        //        <xsd:sequence>
        //            <xsd:element name="ClipPlane" type="plane" minOccurs="0" maxOccurs="unbounded" />
        //        </xsd:sequence>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        ClipPlaneList planes;

        planes.name_ = Msxmlx::GetStringAttribute(element, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "ClipPlaneList: " << planes.name_ << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the ClipPlanes in the list

        Msxmlx::ForEachSubElement(element,
                                  [this, &planes] (IXMLDOMElement * element) { return processClipPlane(element, planes); });

        lists.emplace(planes.name_, planes);
    }

    return true;
}

bool XmlConfiguration::processClipPlane(IXMLDOMElement * element, ClipPlaneList & planes)
{
    //    <xsd:complexType name="plane">
    //        <xsd:all>
    //            <xsd:element name="A" type="xsd:float" />
    //            <xsd:element name="B" type="xsd:float" />
    //            <xsd:element name="C" type="xsd:float" />
    //            <xsd:element name="D" type="xsd:float" />
    //        </xsd:all>
    //    </xsd:complexType>

    glm::vec4 clip;

    clip.x = Msxmlx::GetFloatSubElement(element, "A");
    clip.y = Msxmlx::GetFloatSubElement(element, "B");
    clip.z = Msxmlx::GetFloatSubElement(element, "C");
    clip.w = Msxmlx::GetFloatSubElement(element, "D");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    ClipPlane: ["
            << clip.x << " "
            << clip.y << " "
            << clip.z << " "
            << clip.w << ']'
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this ClipPlane to the list

    planes.planes_.push_back(clip);

    return true;
}

bool XmlConfiguration::processEnvironment(IXMLDOMElement * element, EnvironmentMap & environments)
{
    HRESULT  hr;
    CComBSTR tag;
    hr = element->get_tagName(&tag);

    if (tag == CComBSTR("Environment"))
    {
        //    <xsd:complexType name="environment">
        //        <xsd:all>
        //            <xsd:element name="Gravity" type="vector3" minOccurs="0" />
        //            <xsd:element name="WindVelocity" type="vector3" minOccurs="0" />
        //            <xsd:element name="Gustiness" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="AirFriction" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Bounce" type="xsd:string" minOccurs="0" />
        //            <xsd:element name="Clip" type="xsd:string" minOccurs="0" />
        //        </xsd:all>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        Environment environment;

        environment.name_         = Msxmlx::GetStringAttribute(element, "name");
        environment.gravity_      = GetVectorSubElement(element, "Gravity");
        environment.windVelocity_ = GetVectorSubElement(element, "WindVelocity");
        environment.gustiness_    = Msxmlx::GetFloatSubElement(element, "Gustiness");
        environment.airFriction_  = Msxmlx::GetFloatSubElement(element, "AirFriction");
        environment.surface_      = Msxmlx::GetStringSubElement(element, "Bounce");
        environment.clip_         = Msxmlx::GetStringSubElement(element, "Clip");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Environment: " << environment.name_ << "( "
                << "[" << environment.gravity_ << "], "
                << "[" << environment.windVelocity_ << "], "
                << environment.airFriction_ << ", "
                << environment.gustiness_ << ", "
                << "\"" << environment.surface_ << "\", "
                << "\"" << environment.clip_ << "\" )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        environments.emplace(environment.name_, environment);
    }

    return true;
}

bool XmlConfiguration::processAppearance(IXMLDOMElement * element, AppearanceMap & appearances)
{
    HRESULT  hr;
    CComBSTR tag;
    hr = element->get_tagName(&tag);

    if (tag == CComBSTR("Appearance"))
    {
        //    <xsd:complexType name="appearance">
        //        <xsd:all>
        //            <xsd:element name="ColorChange" type="glcolorchange" minOccurs="0" />
        //            <xsd:element name="RadiusChange" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="RadialVelocity" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Texture" type="xsd:anyURI" minOccurs="0" />
        //            <xsd:element name="Size" type="xsd:float" minOccurs="0" />
        //        </xsd:all>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        Appearance appearance;

        appearance.name_           = Msxmlx::GetStringAttribute(element, "name");
        appearance.colorChange_    = GetRgbaSubElement(element, "ColorChange");
        appearance.radiusChange_   = Msxmlx::GetFloatSubElement(element, "RadiusChange");
        appearance.radialVelocity_ = Msxmlx::GetFloatSubElement(element, "RadialVelocity");
        appearance.texture_        = Msxmlx::GetStringSubElement(element, "Texture");
        appearance.size_           = Msxmlx::GetFloatSubElement(element, "Size", 1.0f);

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Appearance: " << appearance.name_ << "( "
                << "[" << appearance.colorChange_ << "], "
                << appearance.radiusChange_ << ", "
                << appearance.radialVelocity_ << ", "
                << "\"" << appearance.texture_ << "\", "
                << appearance.size_ << " )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        appearances.emplace(appearance.name_, appearance);
    }

    return true;
}

bool XmlConfiguration::processVolume(IXMLDOMElement * element, EmitterVolumeMap & volumes)
{
    HRESULT  hr;
    CComBSTR tag;
    hr = element->get_tagName(&tag);

    if (tag == CComBSTR("Volume"))
    {
        //    <xsd:complexType name="volume">
        //        <xsd:all>
        //            <xsd:element name="Length" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Width" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Height" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Depth" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Radius" type="xsd:float" minOccurs="0" />
        //        </xsd:all>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //        <xsd:attribute name="type" type="volumetype" use="required" />
        //    </xsd:complexType>

        EmitterVolume volume;

        volume.name_   = Msxmlx::GetStringAttribute(element, "name");
        volume.type_   = Msxmlx::GetStringAttribute(element, "type");
        volume.length_ = Msxmlx::GetFloatSubElement(element, "Length", 1.);
        volume.width_  = Msxmlx::GetFloatSubElement(element, "Width", 1.);
        volume.height_ = Msxmlx::GetFloatSubElement(element, "Height", 1.);
        volume.depth_  = Msxmlx::GetFloatSubElement(element, "Depth", 1.);
        volume.radius_ = Msxmlx::GetFloatSubElement(element, "Radius", 1.);

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Volume: " << volume.name_ << "( " << volume.type_ << "( "
                << volume.length_ << ", "
                << volume.width_ << ", "
                << volume.height_ << ", "
                << volume.depth_ << ", "
                << volume.radius_ << " ) )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        volumes.emplace(volume.name_, volume);
    }

    return true;
}

bool XmlConfiguration::processEmitter(IXMLDOMElement * element, EmitterMap & emitters)
{
    HRESULT  hr;
    CComBSTR emitterTag;

    hr = element->get_tagName(&emitterTag);

    if (emitterTag == CComBSTR("Emitter"))
    {
        //    <xsd:complexType name="emitter">
        //        <xsd:all>
        //            <xsd:element name="Volume" type="xsd:string" />
        //            <xsd:element name="Environment" type="xsd:string" />
        //            <xsd:element name="Appearance" type="xsd:string" />
        //            <xsd:element name="Position" type="vector3" minOccurs="0" />
        //            <xsd:element name="Orientation" type="quaternion" minOccurs="0" />
        //            <xsd:element name="Velocity" type="vector3" minOccurs="0" />
        //            <xsd:element name="MinSpeed" type="xsd:float" />
        //            <xsd:element name="MaxSpeed" type="xsd:float" />
        //            <xsd:element name="Count" type="xsd:integer" />
        //            <xsd:element name="Lifetime" type="xsd:float" />
        //            <xsd:element name="Spread" type="xsd:float" />
        //            <xsd:element name="Color" type="rgba" minOccurs="0" />
        //            <xsd:element name="Radius" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="ParticleList" type="particlelist" minOccurs="0" />
        //            <xsd:element name="Sorted" type="xsd:boolean" minOccurs="0" />
        //        </xsd:all>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //        <xsd:attribute name="type" type="emittertype" use="required" />
        //    </xsd:complexType>

        Emitter emitter;
        emitter.name_        = Msxmlx::GetStringAttribute(element, "name");
        emitter.type_        = Msxmlx::GetStringAttribute(element, "type");
        emitter.volume_      = Msxmlx::GetStringSubElement(element, "Volume");
        emitter.environment_ = Msxmlx::GetStringSubElement(element, "Environment");
        emitter.appearance_  = Msxmlx::GetStringSubElement(element, "Appearance");
        emitter.position_    = GetVectorSubElement(element, "Position");
        emitter.orientation_ = GetQuatSubElement(element, "Orientation");
        emitter.velocity_    = GetVectorSubElement(element, "Velocity");
        emitter.count_       = Msxmlx::GetIntSubElement(element, "Count");
        emitter.lifetime_    = Msxmlx::GetFloatSubElement(element, "Lifetime", 1.0f);
        emitter.spread_      = Msxmlx::GetFloatSubElement(element, "Spread");
        emitter.minSpeed_    = Msxmlx::GetFloatSubElement(element, "MinSpeed");
        emitter.maxSpeed_    = Msxmlx::GetFloatSubElement(element, "MaxSpeed");
        emitter.color_       = GetPackedColorSubElement(element, "Color", 0xffffffff);
        emitter.radius_      = Msxmlx::GetFloatSubElement(element, "Radius", 1.0f);
        emitter.sorted_      = Msxmlx::GetBoolSubElement(element, "Sorted");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Emitter: " << emitter.name_ << "( "
                << emitter.type_ << "( "
                << "\"" << emitter.volume_ << "\", "
                << "\"" << emitter.environment_ << "\", "
                << "\"" << emitter.appearance_ << "\", "
                << "[" << emitter.velocity_ << "], "
                << "[" << emitter.position_ << "], "
                << "[" << emitter.orientation_ << "], "
                << emitter.count_ << ", "
                << emitter.lifetime_ << ", "
                << emitter.spread_ << ", "
                << emitter.minSpeed_ << ", "
                << emitter.maxSpeed_ << ", "
                << "0x" << std::hex << std::setw(8) << std::setfill('0') << emitter.color_ << std::setfill(' ') << std::dec << ", "
                << emitter.radius_ << ", "
                << emitter.sorted_ << " ) )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // If there is a list of particles, use that count instead of the supplied count

        long actualCount;

        CComPtr<IXMLDOMNode>     particleListNode     = nullptr;
        CComPtr<IXMLDOMNodeList> particleListNodeList = nullptr;
        element->selectSingleNode(CComBSTR("ParticleList"), &particleListNode);
        if (particleListNode)
        {
            hr = element->get_childNodes(&particleListNodeList);
            hr = particleListNodeList->get_length(&actualCount);

#if defined(_DEBUG)
            if (emitter.count_ != 0 && emitter.count_ != actualCount)
            {
                std::ostringstream msg;
                msg << "ParticleSystem::processEmitter: "
                    << "Warning - " << emitter.name_ << ": "
                    << "Count is " << emitter.count_ << ", "
                    << "but the actual count is " << actualCount << "."
                    << std::endl;
                OutputDebugString(msg.str().c_str());
            }
#endif          // defined( _DEBUG )

            emitter.count_ = actualCount;
        }

        if (emitter.count_ == 0)
            emitter.count_ = 1;

        emitters.emplace(emitter.name_, emitter);
    }

    return true;
}

// void XmlConfiguration::processPointParticles( PointEmitter * pEmitter,
//                                              IXMLDOMNodeList * pNodeList,
//                                              int numParticles,
//                                              Configuration const * configuration,
//                                              float globalLifetime, uint32_t globalColor,
//                                              float spread, float minSpeed, float maxSpeed )
// {
//    RandomFloat                rng( timeGetTime()  ^ (uint32_t)pEmitter );
//    RandomDirection            rdir( ( timeGetTime() ^ (uint32_t)pEmitter ) + 1 );
//
//    PointParticle * const    paParticles        = pEmitter->GetParticles();
//
//    for ( int i = 0; i < numParticles; i++ )
//    {
//        float        lifetime    = globalLifetime;
//        float        age            = rng.Get( -lifetime, 0.0f );
//        uint32_t        color        = ( globalColor != 0 ) ? globalColor :
//                                                            ( i & 1 ? 0 : 0x000000ff ) |
//                                                            ( i & 2 ? 0 : 0x0000ff00 ) |
//                                                            ( i & 4 ? 0 : 0x00ff0000 ) |
//                                                                          0xff000000;
//        float        radius        = 1.0f;
//        float        orientation    = 0.0f;
//        glm::vec4    position    = pEmitter->GetEmitterVolume()->Next();
//        glm::vec4    velocity;
//
//        {
//            List3 const    v    = rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//            velocity.x    = v.Y_;
//            velocity.y    = v.X_;
//            velocity.z    = v.Z_;
//        }
//
//        // If we are processing a list of particle elements, then load non-default values from the next element
//
//        if ( pNodeList )
//        {
//            CComPtr< IXMLDOMNode >        pNode;
//            HRESULT                        hr;
//
//            hr = pNodeList->nextNode( &pNode );
//            ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//        }
//
//        // Initialize the particle
//
//        glm::vec4    rgba    = glm::vec4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//    }
// }
//
//
// void XmlConfiguration::processStreakParticles( StreakEmitter * pEmitter,
//                                               IXMLDOMNodeList * pNodeList,
//                                               int numParticles,
//                                               Configuration const * configuration,
//                                               float globalLifetime, uint32_t globalColor,
//                                               float spread, float minSpeed, float maxSpeed )
// {
//    StreakParticle * const    paParticles                = pEmitter->GetParticles();
//    RandomFloat                rng( ( timeGetTime() ^ (uint32_t)pEmitter ) );
//    RandomDirection            rdir( ( timeGetTime() ^ (uint32_t)pEmitter ) + 1 );
//
//    for ( int i = 0; i < numParticles; i++ )
//    {
//        float        lifetime    = globalLifetime;
//        float        age            = rng.Get( -lifetime, 0.0f );
//        uint32_t        color        = ( globalColor != 0 ) ? globalColor :
//                                                            ( i & 1 ? 0 : 0x000000ff ) |
//                                                            ( i & 2 ? 0 : 0x0000ff00 ) |
//                                                            ( i & 4 ? 0 : 0x00ff0000 ) |
//                                                                          0xff000000;
//        float        radius        = 1.0f;
//        float        orientation    = 0.0f;
//        glm::vec4        position    = pEmitter->GetEmitterVolume()->Next();
//        glm::vec4        velocity;
//
//        {
//            List3 const    v    = rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//            velocity.x    = v.Y_;
//            velocity.y    = v.X_;
//            velocity.z    = v.Z_;
//        }
//
//        // If we are processing a list of particle elements, then load non-default values from the next element
//
//        if ( pNodeList )
//        {
//            CComPtr< IXMLDOMNode >        pNode;
//            HRESULT                        hr;
//
//            hr = pNodeList->nextNode( &pNode );
//            ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//        }
//
//        // Initialize the particle
//
//        glm::vec4    rgba    = glm::vec4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//    }
// }
//
//
// void XmlConfiguration::processTexturedParticles( TexturedEmitter * pEmitter,
//                                                 IXMLDOMNodeList * pNodeList,
//                                                 int numParticles,
//                                                 Configuration const * configuration,
//                                                 float globalLifetime, uint32_t globalColor,
//                                                 float spread, float minSpeed, float maxSpeed )
// {
//    TexturedParticle * const    paParticles                = pEmitter->GetParticles();
//    RandomFloat                    rng( ( timeGetTime() ^ (uint32_t)pEmitter ) );
//    RandomDirection                rdir( ( timeGetTime() ^ (uint32_t)pEmitter ) + 1 );
//
//    for ( int i = 0; i < numParticles; i++ )
//    {
//        float        lifetime    = globalLifetime;
//        float        age            = rng.Get( -lifetime, 0.0f );
//        uint32_t        color        = ( globalColor != 0 ) ? globalColor :
//                                                            ( i & 1 ? 0 : 0x000000ff ) |
//                                                            ( i & 2 ? 0 : 0x0000ff00 ) |
//                                                            ( i & 4 ? 0 : 0x00ff0000 ) |
//                                                                          0xff000000;
//        float        radius        = 1.0f;
//        float        orientation    = 0.0f;
//        glm::vec4    position    = pEmitter->GetEmitterVolume()->Next();
//        std::swap( position.y, position.z );
//
//        glm::vec4    velocity;
//
//        {
//            List3 const    v    = rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//            velocity.x    = v.Y_;
//            velocity.y    = v.X_;
//            velocity.z    = v.Z_;
//        }
//
//        // If we are processing a list of particle elements, then load non-default values from the next element
//
//        if ( pNodeList )
//        {
//            CComPtr< IXMLDOMNode >        pNode;
//            HRESULT                        hr;
//
//            hr = pNodeList->nextNode( &pNode );
//            ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//        }
//
//        // Initialize the particle
//
//        glm::vec4    rgba    = glm::vec4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba, radius, orientation );
//    }
// }
//
//
// void XmlConfiguration::processSphereParticles( SphereEmitter * pEmitter,
//                                               IXMLDOMNodeList * pNodeList,
//                                               int numParticles,
//                                               Configuration const * configuration,
//                                               float globalLifetime, uint32_t globalColor,
//                                               float spread, float minSpeed, float maxSpeed )
// {
//    SphereParticle * const    paParticles                = pEmitter->GetParticles();
//    RandomFloat                rng( ( timeGetTime() ^ (uint32_t)pEmitter ) );
//    RandomDirection            rdir( ( timeGetTime() ^ (uint32_t)pEmitter ) + 1 );
//
//    for ( int i = 0; i < numParticles; i++ )
//    {
//        float        lifetime    = globalLifetime;
//        float        age            = rng.Get( -lifetime, 0.0f );
//        uint32_t        color        = ( globalColor != 0 ) ? globalColor :
//                                                            ( i & 1 ? 0 : 0x000000ff ) |
//                                                            ( i & 2 ? 0 : 0x0000ff00 ) |
//                                                            ( i & 4 ? 0 : 0x00ff0000 ) |
//                                                                          0xff000000;
//        float        radius        = 1.0f;
//        float        orientation    = 0.0f;
//        glm::vec4        position    = pEmitter->GetEmitterVolume()->Next();
//        glm::vec4        velocity;
//
//        {
//            List3 const    v    = rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//            velocity.x    = v.Y_;
//            velocity.y    = v.X_;
//            velocity.z    = v.Z_;
//        }
//
//        // If we are processing a list of particle elements, then load non-default values from the next element
//
//        if ( pNodeList )
//        {
//            CComPtr< IXMLDOMNode >        pNode;
//            HRESULT                        hr;
//
//            hr = pNodeList->nextNode( &pNode );
//            ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//        }
//
//        // Initialize the particle
//
//        glm::vec4    rgba    = glm::vec4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
////        paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
//    }
// }
//
// void XmlConfiguration::processParticle( IXMLDOMNode *        pNode,
//                                          float *            pLifetime,
//                                          float *            pAge,
//                                          glm::vec4 *        pVelocity,
//                                          glm::vec4 *        pPosition,
//                                          uint32_t *            pColor,
//                                          float *            pRadius,
//                                          float *            pRotation,
//                                          D3DXQUATERINION *    pOrientation )
// {
//    HRESULT        hr;
//
//    if ( Msxmlx::IsElementNode( pNode ) )
//    {
//        CComQIPtr< IXMLDOMElement >    element( pNode );
//        CComBSTR                    tag;
//
//        hr = element->get_tagName( &tag );
//
//        if ( tag == CComBSTR( "Particle" ) )
//        {
//            //    <xsd:complexType name="particle">
//            //        <xsd:all>
//            //            <xsd:element name="Lifetime" type="xsd:float" minOccurs="0" />
//            //            <xsd:element name="Age" type="xsd:float" minOccurs="0" />
//            //            <xsd:element name="Position" type="vector3" />
//            //            <xsd:element name="Velocity" type="vector3" />
//            //            <xsd:element name="Color" type="rgba" minOccurs="0" />
//            //            <xsd:element name="Radius" type="xsd:float" minOccurs="0" />
//            //            <xsd:element name="Rotation" type="xsd:float" minOccurs="0" />
//            //            <xsd:element name="Orientation" type="quaternion" minOccurs="0" />
//            //        </xsd:all>
//            //    </xsd:complexType>
//
//            *pPosition        = GetVectorSubElement(    element, "Position",    *pPosition );
//            *pVelocity        = GetVectorSubElement(    element, "Velocity",    *pVelocity );
//            *pLifetime        = Msxmlx::GetFloatSubElement(  element, "Lifetime",    *pLifetime );
//            *pAge            = Msxmlx::GetFloatSubElement(  element, "Age",            *pAge );
//             XMStoreFloat4(pColor,
//                 PackedVector::XMLoadColor(
//                    & PackedVector::XMCOLOR(
//                        Msxmlx::GetHexSubElement(element, "Color", 0xffffffff))));
//            *pRadius        = Msxmlx::GetFloatSubElement(  element, "Radius",        *pRadius );
//            *pRotation        = Msxmlx::GetFloatSubElement(  element, "Rotation",    *pRotation );
//            *pOrientation    = GetDirectX::XMVECTORSubElement( element, "Orientation",    *pOrientation );
//
// #if defined( _DEBUG )
//            {
//                std::ostringstream    msg;
//                msg << "    Particle: "
//                    << *pLifetime << ", "
//                    << *pAge << ", "
//                    << "[" << *pVelocity << "],"
//                    << "[" << *pPosition << "] )"
//                    << *pColor << ", "
//                    << *pRadius << ", "
//                    << *pRotation << ", "
//                    << *pOrientation << ", "
//                    << std::endl;
//                OutputDebugString( msg.str().c_str() );
//            }
// #endif // defined( _DEBUG )
//        }
//    }
// }
} // namespace Confetti

#endif // defined(_WIN32)
