#include "Configuration.h"

#include "Dxx/D3dx.h"
#include "Misc/Exceptions.h"
#include "Msxmlx/Msxmlx.h"
#include "Wx/Wx.h"

#include <atlbase.h>
#include <d3d11.h>
// #include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <iomanip>
#include <sstream>

using namespace DirectX;

namespace
{
DirectX::XMFLOAT3 GetVectorSubElement(IXMLDOMElement * pElement,
                                      char const * sName,
                                      DirectX::XMFLOAT3 const & defaultVec = { 0.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement)
    {
        //    <xsd:complexType name="vector3">
        //        <xsd:all>
        //            <xsd:element name="X" type="xsd:float" />
        //            <xsd:element name="Y" type="xsd:float" />
        //            <xsd:element name="Z" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        DirectX::XMFLOAT3 v;

        v.x = Msxmlx::GetFloatSubElement(pSubElement, "X", defaultVec.x);
        v.y = Msxmlx::GetFloatSubElement(pSubElement, "Y", defaultVec.y);
        v.z = Msxmlx::GetFloatSubElement(pSubElement, "Z", defaultVec.z);

        return v;
    }
    else
    {
        return defaultVec;
    }
}

DirectX::XMFLOAT4 GetPlaneSubElement(IXMLDOMElement * pElement,
                                     char const * sName,
                                     DirectX::XMFLOAT4 const & defaultPlane = { 0.0f, 1.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement)
    {
        //    <xsd:complexType name="plane">
        //        <xsd:all>
        //            <xsd:element name="A" type="xsd:float" />
        //            <xsd:element name="B" type="xsd:float" />
        //            <xsd:element name="C" type="xsd:float" />
        //            <xsd:element name="D" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        DirectX::XMFLOAT4 plane;

        plane.x = Msxmlx::GetFloatSubElement(pSubElement, "A", defaultPlane.x);
        plane.y = Msxmlx::GetFloatSubElement(pSubElement, "B", defaultPlane.y);
        plane.z = Msxmlx::GetFloatSubElement(pSubElement, "C", defaultPlane.z);
        plane.w = Msxmlx::GetFloatSubElement(pSubElement, "D", defaultPlane.w);

        return plane;
    }
    else
    {
        return defaultPlane;
    }
}

DirectX::XMFLOAT4 GetRgbaSubElement(IXMLDOMElement * pElement,
                                    char const * sName,
                                    DirectX::XMFLOAT4 const & defaultRgba = { 0.0f, 0.0f, 0.0f, 0.0f })
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement)
    {
        //    <xsd:complexType name="glcolor">
        //        <xsd:all>
        //            <xsd:element name="R" type="glcolorelement" />
        //            <xsd:element name="G" type="glcolorelement" />
        //            <xsd:element name="B" type="glcolorelement" />
        //            <xsd:element name="A" type="glcolorelement" minOccurs="0" default="1.0" />
        //        </xsd:all>
        //    </xsd:complexType>

        DirectX::XMFLOAT4 rgba;

        rgba.x = Msxmlx::GetFloatSubElement(pSubElement, "R", defaultRgba.x);
        rgba.y = Msxmlx::GetFloatSubElement(pSubElement, "G", defaultRgba.y);
        rgba.z = Msxmlx::GetFloatSubElement(pSubElement, "B", defaultRgba.z);
        rgba.w = Msxmlx::GetFloatSubElement(pSubElement, "A", defaultRgba.w);

        return rgba;
    }
    else
    {
        return defaultRgba;
    }
}

DirectX::XMFLOAT4 GetQuatSubElement(IXMLDOMElement *          pElement,
                                    char const *              sName,
                                    DirectX::XMFLOAT4 const & defaultQuat = Dxx::QuaternionIdentity())
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement)
    {
        //    <xsd:complexType name="quaternion">
        //        <xsd:all>
        //            <xsd:element name="X" type="xsd:float" />
        //            <xsd:element name="Y" type="xsd:float" />
        //            <xsd:element name="Z" type="xsd:float" />
        //            <xsd:element name="W" type="xsd:float" />
        //        </xsd:all>
        //    </xsd:complexType>

        DirectX::XMFLOAT4 q;

        q.x = Msxmlx::GetFloatSubElement(pSubElement, "X", defaultQuat.x);
        q.y = Msxmlx::GetFloatSubElement(pSubElement, "Y", defaultQuat.y);
        q.z = Msxmlx::GetFloatSubElement(pSubElement, "Z", defaultQuat.z);
        q.w = Msxmlx::GetFloatSubElement(pSubElement, "W", defaultQuat.w);

        return q;
    }
    else
    {
        return defaultQuat;
    }
}

#if defined(_DEBUG)

std::ostream & operator <<(std::ostream & s, DirectX::XMFLOAT4 const & q)
{
    s << q.x << ','
      << q.y << ','
      << q.z << ','
      << q.w;

    return s;
}

std::ostream & operator <<(std::ostream & s, DirectX::XMFLOAT3 const & v)
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
XmlConfiguration::XmlConfiguration(char const * sFilename /* = 0*/)
{
    if (sFilename != 0)
    {
        if (!Load(sFilename))
            throw ConstructorFailedException("Failed to load XML file");
    }
}

XmlConfiguration::XmlConfiguration(IXMLDOMDocument2 * pDocument)
{
    if (pDocument != 0)
    {
        if (!Load(pDocument))
            throw ConstructorFailedException("Failed to load XML document");
    }
}

bool XmlConfiguration::Load(char const * sFilename)
{
    Wx::ComInitializer cominit;

    CComPtr<IXMLDOMDocument2> pDocument;
    pDocument.CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER);

    HRESULT      hr;
    VARIANT_BOOL status;

    hr = pDocument->put_async(VARIANT_FALSE);
    if (FAILED(hr))
    {
//        OutputDebugString( "XmlConfiguration::Load: Failed to set async property\n" );
        return false;
    }

    hr = pDocument->load(CComVariant(sFilename), &status);
    if (FAILED(hr))
    {
//        trace( "XmlConfiguration::Load: load( \"%s\" ) failed.\n", sFilename );
        return false;
    }

    if (status != VARIANT_TRUE)
    {
#if defined(_DEBUG)
        CComPtr<IXMLDOMParseError> pObjError;
        CComBSTR reason;

        hr = pDocument->get_parseError(&pObjError);
        hr = pObjError->get_reason(&reason);
//        trace( "XmlConfiguration::Load: Failed to load DOM from '%s'\n%S\n", sFilename, reason );
#endif      // defined( _DEBUG )

        return false;
    }

    return Load(pDocument);
}

bool XmlConfiguration::Load(IXMLDOMDocument2 * pDocument)
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
    //            <xsd:element name="BouncePlaneList" type="bounceplanelist" />
    //            <xsd:element name="ClipPlaneList" type="clipplanelist" />
    //        </xsd:choice>
    //    </xsd:complexType>

    CComPtr<IXMLDOMElement> pRoot;
    hr = pDocument->get_documentElement(&pRoot);
    if (FAILED(hr))
    {
//        trace( "XmlConfiguration::Load: Failed to get document element\n" );
        return false;
    }

    Msxmlx::ForEachSubElement(pRoot, ProcessBouncePlaneList,   (uintptr_t)this);   // Process all the BouncePlaneLists
    Msxmlx::ForEachSubElement(pRoot, ProcessClipPlaneList,     (uintptr_t)this);   // Process all the ClipPlaneLists
    Msxmlx::ForEachSubElement(pRoot, ProcessEnvironment,       (uintptr_t)this);   // Process all the Environments
    Msxmlx::ForEachSubElement(pRoot, ProcessAppearance,        (uintptr_t)this);   // Process all the Appearances
    Msxmlx::ForEachSubElement(pRoot, ProcessVolume,            (uintptr_t)this);   // Process all the Volumes
    Msxmlx::ForEachSubElement(pRoot, ProcessEmitter,           (uintptr_t)this);   // Process all the Emitters

    return true;
}

bool XmlConfiguration::ProcessBouncePlaneList(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("BouncePlaneList"))
    {
        //    <xsd:complexType name="bounceplanelist">
        //        <xsd:sequence>
        //            <xsd:element name="BouncePlane" type="bounceplane" minOccurs="0" maxOccurs="unbounded" />
        //        </xsd:sequence>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        BouncePlaneList bouncePlanes;

        bouncePlanes.name_ =   Msxmlx::GetStringAttribute(pElement, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "BouncePlaneList: " << bouncePlanes.name_ << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the BouncePlanes in the list

        Msxmlx::ForEachSubElement(pElement, ProcessBouncePlane, (uintptr_t)&bouncePlanes);

        // Save this BouncePlaneList

        pConfiguration->bouncePlaneLists_.insert(BouncePlaneListMap::value_type(bouncePlanes.name_, bouncePlanes));
    }
    return true;
}

bool XmlConfiguration::ProcessBouncePlane(IXMLDOMElement * pElement, uintptr_t context)
{
    BouncePlaneList * const pBouncePlanes = reinterpret_cast<BouncePlaneList *>(context);

//    <xsd:complexType name="bounceplane">
//        <xsd:all>
//            <xsd:element name="Plane" type="plane" />
//            <xsd:element name="Dampening" type="xsd:float" />
//        </xsd:all>
//    </xsd:complexType>

    BouncePlane bouncePlane;

    bouncePlane.plane_     = GetPlaneSubElement(pElement, "Plane");
    bouncePlane.dampening_ = Msxmlx::GetFloatSubElement(pElement, "Dampening");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    BouncePlane: ["
            << bouncePlane.plane_.x << " "
            << bouncePlane.plane_.y << " "
            << bouncePlane.plane_.z << " "
            << bouncePlane.plane_.w << "], "
            << bouncePlane.dampening_
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this BouncePlane to the list

    pBouncePlanes->bouncePlanes_.push_back(bouncePlane);

    return true;
}

bool XmlConfiguration::ProcessClipPlaneList(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("ClipPlaneList"))
    {
        //    <xsd:complexType name="clipplanelist">
        //        <xsd:sequence>
        //            <xsd:element name="ClipPlane" type="plane" minOccurs="0" maxOccurs="unbounded" />
        //        </xsd:sequence>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        ClipPlaneList clipPlanes;

        clipPlanes.name_ = Msxmlx::GetStringAttribute(pElement, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "ClipPlaneList: " << clipPlanes.name_ << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the ClipPlanes in the list

        Msxmlx::ForEachSubElement(pElement, ProcessClipPlane, (uintptr_t)&clipPlanes);

        // Save this ClipPlaneList

        pConfiguration->clipPlaneLists_.insert(ClipPlaneListMap::value_type(clipPlanes.name_, clipPlanes));
    }

    return true;
}

bool XmlConfiguration::ProcessClipPlane(IXMLDOMElement * pElement, uintptr_t context)
{
    ClipPlaneList * const pClipPlanes =   reinterpret_cast<ClipPlaneList *>(context);

    //    <xsd:complexType name="plane">
    //        <xsd:all>
    //            <xsd:element name="A" type="xsd:float" />
    //            <xsd:element name="B" type="xsd:float" />
    //            <xsd:element name="C" type="xsd:float" />
    //            <xsd:element name="D" type="xsd:float" />
    //        </xsd:all>
    //    </xsd:complexType>

    DirectX::XMFLOAT4 clipPlane;

    clipPlane.x = Msxmlx::GetFloatSubElement(pElement, "A");
    clipPlane.y = Msxmlx::GetFloatSubElement(pElement, "B");
    clipPlane.z = Msxmlx::GetFloatSubElement(pElement, "C");
    clipPlane.w = Msxmlx::GetFloatSubElement(pElement, "D");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    ClipPlane: ["
            << clipPlane.x << " "
            << clipPlane.y << " "
            << clipPlane.z << " "
            << clipPlane.w << ']'
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this ClipPlane to the list

    pClipPlanes->clipPlanes_.push_back(clipPlane);

    return true;
}

bool XmlConfiguration::ProcessEnvironment(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("Environment"))
    {
        //    <xsd:complexType name="environment">
        //        <xsd:all>
        //            <xsd:element name="Gravity" type="vector3" minOccurs="0" />
        //            <xsd:element name="WindVelocity" type="vector3" minOccurs="0" />
        //            <xsd:element name="Gustiness" type="vector3" minOccurs="0" />
        //            <xsd:element name="AirFriction" type="xsd:float" minOccurs="0" />
        //            <xsd:element name="Bounce" type="xsd:string" minOccurs="0" />
        //            <xsd:element name="Clip" type="xsd:string" minOccurs="0" />
        //        </xsd:all>
        //        <xsd:attribute name="name" type="xsd:string" use="required" />
        //    </xsd:complexType>

        Environment environment;

        environment.name_         = Msxmlx::GetStringAttribute(pElement, "name");
        environment.gravity_      = GetVectorSubElement(pElement, "Gravity");
        environment.windVelocity_ = GetVectorSubElement(pElement, "WindVelocity");
        environment.gustiness_    = GetVectorSubElement(pElement, "Gustiness");
        environment.airFriction_  = Msxmlx::GetFloatSubElement(pElement, "AirFriction");
        environment.bounce_       = Msxmlx::GetStringSubElement(pElement, "Bounce");
        environment.clip_         = Msxmlx::GetStringSubElement(pElement, "Clip");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Environment: " << environment.name_ << "( "
                << "[" << environment.gravity_ << "], "
                << "[" << environment.windVelocity_ << "], "
                << environment.airFriction_ << ", "
                << "[" << environment.gustiness_ << "], "
                << "\"" << environment.bounce_ << "\", "
                << "\"" << environment.clip_ << "\" )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Save this Environment

        pConfiguration->environments_.insert(EnvironmentMap::value_type(environment.name_, environment));
    }

    return true;
}

bool XmlConfiguration::ProcessAppearance(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

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

        appearance.name_           = Msxmlx::GetStringAttribute(pElement, "name");
        appearance.colorChange_    = GetRgbaSubElement(pElement, "ColorChange");
        appearance.radiusChange_   = Msxmlx::GetFloatSubElement(pElement, "RadiusChange");
        appearance.radialVelocity_ = Msxmlx::GetFloatSubElement(pElement, "RadialVelocity");
        appearance.texture_        = Msxmlx::GetStringSubElement(pElement, "Texture");
        appearance.size_           = Msxmlx::GetFloatSubElement(pElement, "Size", 1.0f);

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

        pConfiguration->appearances_.insert(AppearanceMap::value_type(appearance.name_, appearance));
    }

    return true;
}

bool XmlConfiguration::ProcessVolume(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

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

        volume.name_   = Msxmlx::GetStringAttribute(pElement, "name");
        volume.type_   = Msxmlx::GetStringAttribute(pElement, "type");
        volume.length_ = Msxmlx::GetFloatSubElement(pElement, "Length", 1.);
        volume.width_  = Msxmlx::GetFloatSubElement(pElement, "Width", 1.);
        volume.height_ = Msxmlx::GetFloatSubElement(pElement, "Height", 1.);
        volume.depth_  = Msxmlx::GetFloatSubElement(pElement, "Depth", 1.);
        volume.radius_ = Msxmlx::GetFloatSubElement(pElement, "Radius", 1.);

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

        // Save this Volume

        pConfiguration->emitterVolumes_.insert(EmitterVolumeMap::value_type(volume.name_, volume));
    }

    return true;
}

bool XmlConfiguration::ProcessEmitter(IXMLDOMElement * pElement, uintptr_t context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR emitterTag;

    hr = pElement->get_tagName(&emitterTag);

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
        emitter.name_        = Msxmlx::GetStringAttribute(pElement, "name");
        emitter.type_        = Msxmlx::GetStringAttribute(pElement, "type");
        emitter.volume_      = Msxmlx::GetStringSubElement(pElement, "Volume");
        emitter.environment_ = Msxmlx::GetStringSubElement(pElement, "Environment");
        emitter.appearance_  = Msxmlx::GetStringSubElement(pElement, "Appearance");
        emitter.position_    = GetVectorSubElement(pElement, "Position");
        emitter.orientation_ = GetQuatSubElement(pElement, "Orientation");
        emitter.velocity_    = GetVectorSubElement(pElement, "Velocity");
        emitter.count_       = Msxmlx::GetIntSubElement(pElement, "Count");
        emitter.lifetime_    = Msxmlx::GetFloatSubElement(pElement, "Lifetime", 1.0f);
        emitter.spread_      = Msxmlx::GetFloatSubElement(pElement, "Spread");
        emitter.minSpeed_    = Msxmlx::GetFloatSubElement(pElement, "MinSpeed");
        emitter.maxSpeed_    = Msxmlx::GetFloatSubElement(pElement, "MaxSpeed");
        DirectX::XMStoreFloat4(&emitter.color_,
                               DirectX::PackedVector::XMLoadColor(
                                   &DirectX::PackedVector::XMCOLOR(
                                       Msxmlx::GetHexSubElement(pElement, "Color", 0xffffffff))));
        emitter.radius_ = Msxmlx::GetFloatSubElement(pElement, "Radius", 1.0f);
        emitter.sorted_ = Msxmlx::GetBoolSubElement(pElement, "Sorted");

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

        CComPtr<IXMLDOMNode>     pParticleListNode     = nullptr;
        CComPtr<IXMLDOMNodeList> pParticleListNodeList = nullptr;
        pElement->selectSingleNode(CComBSTR("ParticleList"), &pParticleListNode);
        if (pParticleListNode)
        {
            hr = pElement->get_childNodes(&pParticleListNodeList);
            hr = pParticleListNodeList->get_length(&actualCount);

#if defined(_DEBUG)
            if (emitter.count_ != 0 && emitter.count_ != actualCount)
            {
                std::ostringstream msg;
                msg << "ParticleSystem::ProcessEmitter: "
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

        // Save this Emitter

        pConfiguration->emitters_.insert(EmitterMap::value_type(emitter.name_, emitter));
    }

    return true;
}

// void XmlConfiguration::ProcessPointParticles( PointEmitter * pEmitter,
//                                              IXMLDOMNodeList * pNodeList,
//                                              int numParticles,
//                                              Configuration const * pConfiguration,
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
//        DirectX::XMFLOAT4    position    = pEmitter->GetEmitterVolume()->Next();
//        DirectX::XMFLOAT4    velocity;
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
//        DirectX::XMFLOAT4    rgba    = DirectX::XMFLOAT4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//    }
// }
//
//
// void XmlConfiguration::ProcessStreakParticles( StreakEmitter * pEmitter,
//                                               IXMLDOMNodeList * pNodeList,
//                                               int numParticles,
//                                               Configuration const * pConfiguration,
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
//        DirectX::XMFLOAT4        position    = pEmitter->GetEmitterVolume()->Next();
//        DirectX::XMFLOAT4        velocity;
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
//        DirectX::XMFLOAT4    rgba    = DirectX::XMFLOAT4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//    }
// }
//
//
// void XmlConfiguration::ProcessTexturedParticles( TexturedEmitter * pEmitter,
//                                                 IXMLDOMNodeList * pNodeList,
//                                                 int numParticles,
//                                                 Configuration const * pConfiguration,
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
//        DirectX::XMFLOAT4    position    = pEmitter->GetEmitterVolume()->Next();
//        std::swap( position.y, position.z );
//
//        DirectX::XMFLOAT4    velocity;
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
//        DirectX::XMFLOAT4    rgba    = DirectX::XMFLOAT4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//        paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba, radius, orientation );
//    }
// }
//
//
// void XmlConfiguration::ProcessSphereParticles( SphereEmitter * pEmitter,
//                                               IXMLDOMNodeList * pNodeList,
//                                               int numParticles,
//                                               Configuration const * pConfiguration,
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
//        DirectX::XMFLOAT4        position    = pEmitter->GetEmitterVolume()->Next();
//        DirectX::XMFLOAT4        velocity;
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
//        DirectX::XMFLOAT4    rgba    = DirectX::XMFLOAT4( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//                                         float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//                                         float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//                                         float( color & 0xff000000U ) / float( 0xff000000U ) );
//
////        paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
//    }
// }
//
// void XmlConfiguration::ProcessParticle( IXMLDOMNode *        pNode,
//                                          float *            pLifetime,
//                                          float *            pAge,
//                                          DirectX::XMFLOAT4 *        pVelocity,
//                                          DirectX::XMFLOAT4 *        pPosition,
//                                          uint32_t *            pColor,
//                                          float *            pRadius,
//                                          float *            pRotation,
//                                          D3DXQUATERINION *    pOrientation )
// {
//    HRESULT        hr;
//
//    if ( Msxmlx::IsElementNode( pNode ) )
//    {
//        CComQIPtr< IXMLDOMElement >    pElement( pNode );
//        CComBSTR                    tag;
//
//        hr = pElement->get_tagName( &tag );
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
//            *pPosition        = GetVectorSubElement(    pElement, "Position",    *pPosition );
//            *pVelocity        = GetVectorSubElement(    pElement, "Velocity",    *pVelocity );
//            *pLifetime        = Msxmlx::GetFloatSubElement(  pElement, "Lifetime",    *pLifetime );
//            *pAge            = Msxmlx::GetFloatSubElement(  pElement, "Age",            *pAge );
//            DirectX::XMStoreFloat4(pColor,
//                DirectX::PackedVector::XMLoadColor(
//                    &DirectX::PackedVector::XMCOLOR(
//                        Msxmlx::GetHexSubElement(pElement, "Color", 0xffffffff))));
//            *pRadius        = Msxmlx::GetFloatSubElement(  pElement, "Radius",        *pRadius );
//            *pRotation        = Msxmlx::GetFloatSubElement(  pElement, "Rotation",    *pRotation );
//            *pOrientation    = GetDirectX::XMVECTORSubElement( pElement, "Orientation",    *pOrientation );
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
