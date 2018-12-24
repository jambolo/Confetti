/** @file *//********************************************************************************************************

                                                  Configuration.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Configuration.cpp#15 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Configuration.h"

namespace
{
D3DXVECTOR3 GetD3DXVECTOR3SubElement(IXMLDOMElement *    pElement,
                                     char const *        sName,
                                     D3DXVECTOR3 const & defaultVec = Dxx::Vector3Origin());

D3DXPLANE GetD3DXPLANESubElement(IXMLDOMElement * pElement,
                                 char const * sName,
                                 D3DXPLANE const & defaultPlane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.0f));

D3DXCOLOR GetRgbaSubElement(IXMLDOMElement * pElement,
                            char const * sName,
                            D3DXCOLOR const & defaultRgba = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

D3DXQUATERNION GetQuatSubElement(IXMLDOMElement *       pElement,
                                 char const *           sName,
                                 D3DXQUATERNION const & defaultQuat = Dxx::QuaternionIdentity());

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

D3DXVECTOR3 GetD3DXVECTOR3SubElement(IXMLDOMElement *    pElement,
                                     char const *        sName,
                                     D3DXVECTOR3 const & defaultVec /* = Dxx::Vector3Origin()*/)
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement != NULL)
    {
        //	<xsd:complexType name="vector3">
        //		<xsd:all>
        //			<xsd:element name="X" type="xsd:float" />
        //			<xsd:element name="Y" type="xsd:float" />
        //			<xsd:element name="Z" type="xsd:float" />
        //		</xsd:all>
        //	</xsd:complexType>

        D3DXVECTOR3 v;

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

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

D3DXPLANE GetD3DXPLANESubElement(IXMLDOMElement *  pElement,
                                 char const *      sName,
                                 D3DXPLANE const & defaultPlane /* = D3DXPLANE( 0.0f, 1.0f, 0.0f, 0.0f )*/)
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement != NULL)
    {
        //	<xsd:complexType name="plane">
        //		<xsd:all>
        //			<xsd:element name="X" type="xsd:float" />
        //			<xsd:element name="Y" type="xsd:float" />
        //			<xsd:element name="Z" type="xsd:float" />
        //			<xsd:element name="D" type="xsd:float" />
        //		</xsd:all>
        //	</xsd:complexType>

        D3DXPLANE plane;

        plane.a = Msxmlx::GetFloatSubElement(pSubElement, "X", defaultPlane.a);
        plane.b = Msxmlx::GetFloatSubElement(pSubElement, "Y", defaultPlane.b);
        plane.c = Msxmlx::GetFloatSubElement(pSubElement, "Z", defaultPlane.c);
        plane.d = Msxmlx::GetFloatSubElement(pSubElement, "D", defaultPlane.d);

        return plane;
    }
    else
    {
        return defaultPlane;
    }
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

D3DXCOLOR GetRgbaSubElement(IXMLDOMElement *  pElement,
                            char const *      sName,
                            D3DXCOLOR const & defaultRgba /* = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f )*/)
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement != NULL)
    {
        //	<xsd:complexType name="glcolor">
        //		<xsd:all>
        //			<xsd:element name="R" type="glcolorelement" />
        //			<xsd:element name="G" type="glcolorelement" />
        //			<xsd:element name="B" type="glcolorelement" />
        //			<xsd:element name="A" type="glcolorelement" minOccurs="0" default="1.0" />
        //		</xsd:all>
        //	</xsd:complexType>

        D3DXCOLOR rgba;

        rgba.r = Msxmlx::GetFloatSubElement(pSubElement, "R", defaultRgba.r);
        rgba.g = Msxmlx::GetFloatSubElement(pSubElement, "G", defaultRgba.g);
        rgba.b = Msxmlx::GetFloatSubElement(pSubElement, "B", defaultRgba.b);
        rgba.a = Msxmlx::GetFloatSubElement(pSubElement, "A", defaultRgba.a);

        return rgba;
    }
    else
    {
        return defaultRgba;
    }
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

D3DXQUATERNION GetQuatSubElement(IXMLDOMElement *       pElement,
                                 char const *           sName,
                                 D3DXQUATERNION const & defaultQuat /* = Dxx::QuaternionIdentity()*/)
{
    HRESULT hr;
    CComPtr<IXMLDOMElement> pSubElement;

    hr = Msxmlx::GetSubElement(pElement, sName, &pSubElement);
    if (pSubElement != NULL)
    {
        //	<xsd:complexType name="quaternion">
        //		<xsd:all>
        //			<xsd:element name="X" type="xsd:float" />
        //			<xsd:element name="Y" type="xsd:float" />
        //			<xsd:element name="Z" type="xsd:float" />
        //			<xsd:element name="W" type="xsd:float" />
        //		</xsd:all>
        //	</xsd:complexType>

        D3DXQUATERNION q;

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

std::ostream & operator <<(std::ostream & s, D3DXQUATERNION const & q)
{
    s << q.x << ','
      << q.y << ','
      << q.z << ','
      << q.w;

    return s;
}

std::ostream & operator <<(std::ostream & s, D3DXVECTOR3 const & v)
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
/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Configuration::Configuration()
{
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Configuration::~Configuration()
{
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

XmlConfiguration::XmlConfiguration(char const * sFilename /* = 0*/)
{
    if (sFilename != 0)
    {
        if (!Load(sFilename))
            throw ConstructorFailedException("Failed to load XML file");
    }
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

XmlConfiguration::XmlConfiguration(IXMLDOMDocument2 * pDocument)
{
    if (pDocument != 0)
    {
        if (!Load(pDocument))
            throw ConstructorFailedException("Failed to load XML document");
    }
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

XmlConfiguration::~XmlConfiguration()
{
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

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
//		OutputDebugString( "XmlConfiguration::Load: Failed to set async property\n" );
        return false;
    }

    hr = pDocument->load(CComVariant(sFilename), &status);
    if (FAILED(hr))
    {
//		trace( "XmlConfiguration::Load: load( \"%s\" ) failed.\n", sFilename );
        return false;
    }

    if (status != VARIANT_TRUE)
    {
#if defined(_DEBUG)
        CComPtr<IXMLDOMParseError> pObjError;
        CComBSTR reason;

        hr = pDocument->get_parseError(&pObjError);
        hr = pObjError->get_reason(&reason);
//		trace( "XmlConfiguration::Load: Failed to load DOM from '%s'\n%S\n", sFilename, reason );
#endif      // defined( _DEBUG )

        return false;
    }

    return Load(pDocument);
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::Load(IXMLDOMDocument2 * pDocument)
{
    HRESULT hr;

    // Parse the XML

    //	<xsd:element name="ParticleSystem" type="particlesystem" />
    //	<xsd:complexType name="particlesystem">
    //		<xsd:choice minOccurs="0" maxOccurs="unbounded">
    //			<xsd:element name="Emitter" type="emitter" />
    //			<xsd:element name="Environment" type="environment" />
    //			<xsd:element name="Appearance" type="appearance" />
    //			<xsd:element name="Volume" type="volume" />
    //			<xsd:element name="BouncePlaneList" type="bounceplanelist" />
    //			<xsd:element name="ClipPlaneList" type="clipplanelist" />
    //		</xsd:choice>
    //	</xsd:complexType>

    CComPtr<IXMLDOMElement> pRoot;
    hr = pDocument->get_documentElement(&pRoot);
    if (FAILED(hr))
    {
//		trace( "XmlConfiguration::Load: Failed to get document element\n" );
        return false;
    }

    Msxmlx::ForEachSubElement(pRoot, ProcessBouncePlaneList,   (uint32)this);   // Process all the BouncePlaneLists
    Msxmlx::ForEachSubElement(pRoot, ProcessClipPlaneList,     (uint32)this);   // Process all the ClipPlaneLists
    Msxmlx::ForEachSubElement(pRoot, ProcessEnvironment,       (uint32)this);   // Process all the Environments
    Msxmlx::ForEachSubElement(pRoot, ProcessAppearance,        (uint32)this);   // Process all the Appearances
    Msxmlx::ForEachSubElement(pRoot, ProcessVolume,            (uint32)this);   // Process all the Volumes
    Msxmlx::ForEachSubElement(pRoot, ProcessEmitter,           (uint32)this);   // Process all the Emitters

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessBouncePlaneList(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("BouncePlaneList"))
    {
        //	<xsd:complexType name="bounceplanelist">
        //		<xsd:sequence>
        //			<xsd:element name="BouncePlane" type="bounceplane" minOccurs="0" maxOccurs="unbounded" />
        //		</xsd:sequence>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //	</xsd:complexType>

        BouncePlaneList bouncePlanes;

        bouncePlanes.m_Name =   Msxmlx::GetStringAttribute(pElement, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "BouncePlaneList: " << bouncePlanes.m_Name << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the BouncePlanes in the list

        Msxmlx::ForEachSubElement(pElement, ProcessBouncePlane, (uint32) & bouncePlanes);

        // Save this BouncePlaneList

        pConfiguration->m_BouncePlaneLists.insert(BouncePlaneListMap::value_type(bouncePlanes.m_Name, bouncePlanes));
    }
    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessBouncePlane(IXMLDOMElement * pElement, uint32 context)
{
    BouncePlaneList * const pBouncePlanes = reinterpret_cast<BouncePlaneList *>(context);

//	<xsd:complexType name="bounceplane">
//		<xsd:all>
//			<xsd:element name="Plane" type="plane" />
//			<xsd:element name="Dampening" type="xsd:float" />
//		</xsd:all>
//	</xsd:complexType>

    BouncePlane bouncePlane;

    bouncePlane.m_Plane     = GetD3DXPLANESubElement(pElement, "Plane");
    bouncePlane.m_Dampening = Msxmlx::GetFloatSubElement(pElement, "Dampening");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    BouncePlane: ["
            << bouncePlane.m_Plane.a << " "
            << bouncePlane.m_Plane.b << " "
            << bouncePlane.m_Plane.c << " "
            << bouncePlane.m_Plane.d << "], "
            << bouncePlane.m_Dampening
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this BouncePlane to the list

    pBouncePlanes->m_BouncePlanes.push_back(bouncePlane);

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessClipPlaneList(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("ClipPlaneList"))
    {
        //	<xsd:complexType name="clipplanelist">
        //		<xsd:sequence>
        //			<xsd:element name="ClipPlane" type="plane" minOccurs="0" maxOccurs="unbounded" />
        //		</xsd:sequence>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //	</xsd:complexType>

        ClipPlaneList clipPlanes;

        clipPlanes.m_Name = Msxmlx::GetStringAttribute(pElement, "name");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "ClipPlaneList: " << clipPlanes.m_Name << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Process all the ClipPlanes in the list

        Msxmlx::ForEachSubElement(pElement, ProcessClipPlane, (uint32) & clipPlanes);

        // Save this ClipPlaneList

        pConfiguration->m_ClipPlaneLists.insert(ClipPlaneListMap::value_type(clipPlanes.m_Name, clipPlanes));
    }

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessClipPlane(IXMLDOMElement * pElement, uint32 context)
{
    ClipPlaneList * const pClipPlanes =   reinterpret_cast<ClipPlaneList *>(context);

    //	<xsd:complexType name="plane">
    //		<xsd:all>
    //			<xsd:element name="X" type="xsd:float" />
    //			<xsd:element name="Y" type="xsd:float" />
    //			<xsd:element name="Z" type="xsd:float" />
    //			<xsd:element name="D" type="xsd:float" />
    //		</xsd:all>
    //	</xsd:complexType>

    D3DXPLANE clipPlane;

    clipPlane.a = Msxmlx::GetFloatSubElement(pElement, "X");
    clipPlane.b = Msxmlx::GetFloatSubElement(pElement, "Y");
    clipPlane.c = Msxmlx::GetFloatSubElement(pElement, "Z");
    clipPlane.d = Msxmlx::GetFloatSubElement(pElement, "D");

#if defined(_DEBUG)
    {
        std::ostringstream msg;
        msg << "    ClipPlane: [" << clipPlane.a << " "
            << clipPlane.b << " "
            << clipPlane.c << " "
            << clipPlane.d << ']'
            << std::endl;
        OutputDebugString(msg.str().c_str());
    }
#endif  // defined( _DEBUG )

    // Add this ClipPlane to the list

    pClipPlanes->m_ClipPlanes.push_back(clipPlane);

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessEnvironment(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("Environment"))
    {
        //	<xsd:complexType name="environment">
        //		<xsd:all>
        //			<xsd:element name="Gravity" type="vector3" minOccurs="0" />
        //			<xsd:element name="WindVelocity" type="vector3" minOccurs="0" />
        //			<xsd:element name="Gustiness" type="vector3" minOccurs="0" />
        //			<xsd:element name="AirFriction" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Bounce" type="xsd:string" minOccurs="0" />
        //			<xsd:element name="Clip" type="xsd:string" minOccurs="0" />
        //		</xsd:all>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //	</xsd:complexType>

        Environment environment;

        environment.m_Name         = Msxmlx::GetStringAttribute(pElement, "name");
        environment.m_Gravity      = GetD3DXVECTOR3SubElement(pElement, "Gravity");
        environment.m_WindVelocity = GetD3DXVECTOR3SubElement(pElement, "WindVelocity");
        environment.m_Gustiness    = GetD3DXVECTOR3SubElement(pElement, "Gustiness");
        environment.m_AirFriction  = Msxmlx::GetFloatSubElement(pElement, "AirFriction");
        environment.m_Bounce       = Msxmlx::GetStringSubElement(pElement, "Bounce");
        environment.m_Clip         = Msxmlx::GetStringSubElement(pElement, "Clip");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Environment: " << environment.m_Name << "( "
                << "[" << environment.m_Gravity << "], "
                << "[" << environment.m_WindVelocity << "], "
                << environment.m_AirFriction << ", "
                << "[" << environment.m_Gustiness << "], "
                << "\"" << environment.m_Bounce << "\", "
                << "\"" << environment.m_Clip << "\" )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Save this Environment

        pConfiguration->m_Environments.insert(EnvironmentMap::value_type(environment.m_Name, environment));
    }

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessAppearance(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("Appearance"))
    {
        //	<xsd:complexType name="appearance">
        //		<xsd:all>
        //			<xsd:element name="ColorChange" type="glcolorchange" minOccurs="0" />
        //			<xsd:element name="RadiusChange" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="RadialVelocity" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Texture" type="xsd:anyURI" minOccurs="0" />
        //			<xsd:element name="Size" type="xsd:float" minOccurs="0" />
        //		</xsd:all>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //	</xsd:complexType>

        Appearance appearance;

        appearance.m_Name           = Msxmlx::GetStringAttribute(pElement, "name");
        appearance.m_ColorChange    = GetRgbaSubElement(pElement, "ColorChange");
        appearance.m_RadiusChange   = Msxmlx::GetFloatSubElement(pElement, "RadiusChange");
        appearance.m_RadialVelocity = Msxmlx::GetFloatSubElement(pElement, "RadialVelocity");
        appearance.m_Texture        = Msxmlx::GetStringSubElement(pElement, "Texture");
        appearance.m_Size           = Msxmlx::GetFloatSubElement(pElement, "Size", 1.0f);

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Appearance: " << appearance.m_Name << "( "
                << "[" << appearance.m_ColorChange << "], "
                << appearance.m_RadiusChange << ", "
                << appearance.m_RadialVelocity << ", "
                << "\"" << appearance.m_Texture << "\", "
                << appearance.m_Size << " )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        pConfiguration->m_Appearances.insert(AppearanceMap::value_type(appearance.m_Name, appearance));
    }

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessVolume(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR tag;
    hr = pElement->get_tagName(&tag);

    if (tag == CComBSTR("Volume"))
    {
        //	<xsd:complexType name="volume">
        //		<xsd:all>
        //			<xsd:element name="Length" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Width" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Height" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Depth" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="Radius" type="xsd:float" minOccurs="0" />
        //		</xsd:all>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //		<xsd:attribute name="type" type="volumetype" use="required" />
        //	</xsd:complexType>

        EmitterVolume volume;

        volume.m_Name   = Msxmlx::GetStringAttribute(pElement, "name");
        volume.m_Type   = Msxmlx::GetStringAttribute(pElement, "type");
        volume.m_Length = Msxmlx::GetFloatSubElement(pElement, "Length", 1.);
        volume.m_Width  = Msxmlx::GetFloatSubElement(pElement, "Width", 1.);
        volume.m_Height = Msxmlx::GetFloatSubElement(pElement, "Height", 1.);
        volume.m_Depth  = Msxmlx::GetFloatSubElement(pElement, "Depth", 1.);
        volume.m_Radius = Msxmlx::GetFloatSubElement(pElement, "Radius", 1.);

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Volume: " << volume.m_Name << "( " << volume.m_Type << "( "
                << volume.m_Length << ", "
                << volume.m_Width << ", "
                << volume.m_Height << ", "
                << volume.m_Depth << ", "
                << volume.m_Radius << " ) )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // Save this Volume

        pConfiguration->m_EmitterVolumes.insert(EmitterVolumeMap::value_type(volume.m_Name, volume));
    }

    return true;
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

bool XmlConfiguration::ProcessEmitter(IXMLDOMElement * pElement, uint32 context)
{
    XmlConfiguration * const pConfiguration = reinterpret_cast<XmlConfiguration *>(context);

    HRESULT  hr;
    CComBSTR emitterTag;

    hr = pElement->get_tagName(&emitterTag);

    if (emitterTag == CComBSTR("Emitter"))
    {
        //	<xsd:complexType name="emitter">
        //		<xsd:all>
        //			<xsd:element name="Volume" type="xsd:string" />
        //			<xsd:element name="Environment" type="xsd:string" />
        //			<xsd:element name="Appearance" type="xsd:string" />
        //			<xsd:element name="Position" type="vector3" minOccurs="0" />
        //			<xsd:element name="Orientation" type="quaternion" minOccurs="0" />
        //			<xsd:element name="Velocity" type="vector3" minOccurs="0" />
        //			<xsd:element name="MinSpeed" type="xsd:float" />
        //			<xsd:element name="MaxSpeed" type="xsd:float" />
        //			<xsd:element name="Count" type="xsd:integer" />
        //			<xsd:element name="Lifetime" type="xsd:float" />
        //			<xsd:element name="Spread" type="xsd:float" />
        //			<xsd:element name="Color" type="rgba" minOccurs="0" />
        //			<xsd:element name="Radius" type="xsd:float" minOccurs="0" />
        //			<xsd:element name="ParticleList" type="particlelist" minOccurs="0" />
        //			<xsd:element name="Sorted" type="xsd:boolean" minOccurs="0" />
        //		</xsd:all>
        //		<xsd:attribute name="name" type="xsd:string" use="required" />
        //		<xsd:attribute name="type" type="emittertype" use="required" />
        //	</xsd:complexType>

        Emitter emitter;
        emitter.m_Name        = Msxmlx::GetStringAttribute(pElement, "name");
        emitter.m_Type        = Msxmlx::GetStringAttribute(pElement, "type");
        emitter.m_Volume      = Msxmlx::GetStringSubElement(pElement, "Volume");
        emitter.m_Environment = Msxmlx::GetStringSubElement(pElement, "Environment");
        emitter.m_Appearance  = Msxmlx::GetStringSubElement(pElement, "Appearance");
        emitter.m_Position    = GetD3DXVECTOR3SubElement(pElement, "Position");
        emitter.m_Orientation = GetQuatSubElement(pElement, "Orientation");
        emitter.m_Velocity    = GetD3DXVECTOR3SubElement(pElement, "Velocity");
        emitter.m_Count       = Msxmlx::GetIntSubElement(pElement, "Count");
        emitter.m_Lifetime    = Msxmlx::GetFloatSubElement(pElement, "Lifetime", 1.0f);
        emitter.m_Spread      = Msxmlx::GetFloatSubElement(pElement, "Spread");
        emitter.m_MinSpeed    = Msxmlx::GetFloatSubElement(pElement, "MinSpeed");
        emitter.m_MaxSpeed    = Msxmlx::GetFloatSubElement(pElement, "MaxSpeed");
        emitter.m_Color       = Msxmlx::GetHexSubElement(pElement, "Color", 0xffffffff);
        emitter.m_Radius      = Msxmlx::GetFloatSubElement(pElement, "Radius", 1.0f);
        emitter.m_Sorted      = Msxmlx::GetBoolSubElement(pElement, "Sorted");

#if defined(_DEBUG)
        {
            std::ostringstream msg;
            msg << "Emitter: " << emitter.m_Name << "( "
                << emitter.m_Type << "( "
                << "\"" << emitter.m_Volume << "\", "
                << "\"" << emitter.m_Environment << "\", "
                << "\"" << emitter.m_Appearance << "\", "
                << "[" << emitter.m_Velocity << "], "
                << "[" << emitter.m_Position << "], "
                << "[" << emitter.m_Orientation << "], "
                << emitter.m_Count << ", "
                << emitter.m_Lifetime << ", "
                << emitter.m_Spread << ", "
                << emitter.m_MinSpeed << ", "
                << emitter.m_MaxSpeed << ", "
                << "0x" << std::hex << std::setw(8) << std::setfill('0') << emitter.m_Color << std::setfill(' ') << std::dec << ", "
                << emitter.m_Radius << ", "
                << emitter.m_Sorted << " ) )"
                << std::endl;
            OutputDebugString(msg.str().c_str());
        }
#endif      // defined( _DEBUG )

        // If there is a list of particles, use that count instead of the supplied count

        long actualCount;

        CComPtr<IXMLDOMNode>     pParticleListNode     = NULL;
        CComPtr<IXMLDOMNodeList> pParticleListNodeList = NULL;
        pElement->selectSingleNode(CComBSTR("ParticleList"), &pParticleListNode);
        if (pParticleListNode)
        {
            hr = pElement->get_childNodes(&pParticleListNodeList);
            hr = pParticleListNodeList->get_length(&actualCount);

#if defined(_DEBUG)
            if (emitter.m_Count != 0 && emitter.m_Count != actualCount)
            {
                std::ostringstream msg;
                msg << "ParticleSystem::ProcessEmitter: "
                    << "Warning - " << emitter.m_Name << ": "
                    << "Count is " << emitter.m_Count << ", "
                    << "but the actual count is " << actualCount << "."
                    << std::endl;
                OutputDebugString(msg.str().c_str());
            }
#endif          // defined( _DEBUG )

            emitter.m_Count = actualCount;
        }

        if (emitter.m_Count == 0)
            emitter.m_Count = 1;


        // Save this Emitter

        pConfiguration->m_Emitters.insert(EmitterMap::value_type(emitter.m_Name, emitter));
    }

    return true;
}

/// ********************************************************************************************************************/
/// *																													*/
/// *																													*/
/// ********************************************************************************************************************/
//
// void XmlConfiguration::ProcessPointParticles( PointEmitter * pEmitter,
//											  IXMLDOMNodeList * pNodeList,
//											  int numParticles,
//											  Configuration const * pConfiguration,
//											  float globalLifetime, uint32 globalColor,
//											  float spread, float minSpeed, float maxSpeed )
// {
//	RandomFloat				rng( timeGetTime()  ^ (uint32)pEmitter );
//	RandomDirection			rdir( ( timeGetTime() ^ (uint32)pEmitter ) + 1 );
//
//	PointParticle * const	paParticles		= pEmitter->GetParticles();
//
//	for ( int i = 0; i < numParticles; i++ )
//	{
//		float		lifetime	= globalLifetime;
//		float		age			= rng.Get( -lifetime, 0.0f );
//		uint32		color		= ( globalColor != 0 ) ? globalColor :
//															( i & 1 ? 0 : 0x000000ff ) |
//															( i & 2 ? 0 : 0x0000ff00 ) |
//															( i & 4 ? 0 : 0x00ff0000 ) |
//																		  0xff000000;
//		float		radius		= 1.0f;
//		float		orientation	= 0.0f;
//		D3DXVECTOR3	position	= pEmitter->GetEmitterVolume()->Next();
//		D3DXVECTOR3	velocity;
//
//		{
//			List3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//			velocity.x	= v.m_Y;
//			velocity.y	= v.m_X;
//			velocity.z	= v.m_Z;
//		}
//
//		// If we are processing a list of particle elements, then load non-default values from the next element
//
//		if ( pNodeList )
//		{
//			CComPtr< IXMLDOMNode >		pNode;
//			HRESULT						hr;
//
//			hr = pNodeList->nextNode( &pNode );
//			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//		}
//
//		// Initialize the particle
//
//		D3DXCOLOR	rgba	= D3DXCOLOR( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//										 float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//		paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//	}
// }
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// *																													*/
/// ********************************************************************************************************************/
//
// void XmlConfiguration::ProcessStreakParticles( StreakEmitter * pEmitter,
//											   IXMLDOMNodeList * pNodeList,
//											   int numParticles,
//											   Configuration const * pConfiguration,
//											   float globalLifetime, uint32 globalColor,
//                                               float spread, float minSpeed, float maxSpeed )
// {
//	StreakParticle * const	paParticles				= pEmitter->GetParticles();
//	RandomFloat				rng( ( timeGetTime() ^ (uint32)pEmitter ) );
//	RandomDirection			rdir( ( timeGetTime() ^ (uint32)pEmitter ) + 1 );
//
//	for ( int i = 0; i < numParticles; i++ )
//	{
//		float		lifetime	= globalLifetime;
//		float		age			= rng.Get( -lifetime, 0.0f );
//		uint32		color		= ( globalColor != 0 ) ? globalColor :
//															( i & 1 ? 0 : 0x000000ff ) |
//															( i & 2 ? 0 : 0x0000ff00 ) |
//															( i & 4 ? 0 : 0x00ff0000 ) |
//																		  0xff000000;
//		float		radius		= 1.0f;
//		float		orientation	= 0.0f;
//		D3DXVECTOR3		position	= pEmitter->GetEmitterVolume()->Next();
//		D3DXVECTOR3		velocity;
//
//		{
//			List3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//			velocity.x	= v.m_Y;
//			velocity.y	= v.m_X;
//			velocity.z	= v.m_Z;
//		}
//
//		// If we are processing a list of particle elements, then load non-default values from the next element
//
//		if ( pNodeList )
//		{
//			CComPtr< IXMLDOMNode >		pNode;
//			HRESULT						hr;
//
//			hr = pNodeList->nextNode( &pNode );
//			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//		}
//
//		// Initialize the particle
//
//		D3DXCOLOR	rgba	= D3DXCOLOR( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//										 float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//		paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba );
//	}
// }
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// *																													*/
/// ********************************************************************************************************************/
//
// void XmlConfiguration::ProcessTexturedParticles( TexturedEmitter * pEmitter,
//												 IXMLDOMNodeList * pNodeList,
//												 int numParticles,
//												 Configuration const * pConfiguration,
//												 float globalLifetime, uint32 globalColor,
//												 float spread, float minSpeed, float maxSpeed )
// {
//	TexturedParticle * const	paParticles				= pEmitter->GetParticles();
//	RandomFloat					rng( ( timeGetTime() ^ (uint32)pEmitter ) );
//	RandomDirection				rdir( ( timeGetTime() ^ (uint32)pEmitter ) + 1 );
//
//	for ( int i = 0; i < numParticles; i++ )
//	{
//		float		lifetime	= globalLifetime;
//		float		age			= rng.Get( -lifetime, 0.0f );
//		uint32		color		= ( globalColor != 0 ) ? globalColor :
//															( i & 1 ? 0 : 0x000000ff ) |
//															( i & 2 ? 0 : 0x0000ff00 ) |
//															( i & 4 ? 0 : 0x00ff0000 ) |
//																		  0xff000000;
//		float		radius		= 1.0f;
//		float		orientation	= 0.0f;
//		D3DXVECTOR3	position	= pEmitter->GetEmitterVolume()->Next();
//		std::swap( position.y, position.z );
//
//		D3DXVECTOR3	velocity;
//
//		{
//			List3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//			velocity.x	= v.m_Y;
//			velocity.y	= v.m_X;
//			velocity.z	= v.m_Z;
//		}
//
//		// If we are processing a list of particle elements, then load non-default values from the next element
//
//		if ( pNodeList )
//		{
//			CComPtr< IXMLDOMNode >		pNode;
//			HRESULT						hr;
//
//			hr = pNodeList->nextNode( &pNode );
//			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//		}
//
//		// Initialize the particle
//
//		D3DXCOLOR	rgba	= D3DXCOLOR( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//										 float( color & 0xff000000U ) / float( 0xff000000U ) );
//
//		paParticles[ i ].Initialize( lifetime, age, position, velocity, rgba, radius, orientation );
//	}
// }
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// *																													*/
/// ********************************************************************************************************************/
//
// void XmlConfiguration::ProcessSphereParticles( SphereEmitter * pEmitter,
//											   IXMLDOMNodeList * pNodeList,
//											   int numParticles,
//											   Configuration const * pConfiguration,
//											   float globalLifetime, uint32 globalColor,
//											   float spread, float minSpeed, float maxSpeed )
// {
//	SphereParticle * const	paParticles				= pEmitter->GetParticles();
//	RandomFloat				rng( ( timeGetTime() ^ (uint32)pEmitter ) );
//	RandomDirection			rdir( ( timeGetTime() ^ (uint32)pEmitter ) + 1 );
//
//	for ( int i = 0; i < numParticles; i++ )
//	{
//		float		lifetime	= globalLifetime;
//		float		age			= rng.Get( -lifetime, 0.0f );
//		uint32		color		= ( globalColor != 0 ) ? globalColor :
//															( i & 1 ? 0 : 0x000000ff ) |
//															( i & 2 ? 0 : 0x0000ff00 ) |
//															( i & 4 ? 0 : 0x00ff0000 ) |
//																		  0xff000000;
//		float		radius		= 1.0f;
//		float		orientation	= 0.0f;
//		D3DXVECTOR3		position	= pEmitter->GetEmitterVolume()->Next();
//		D3DXVECTOR3		velocity;
//
//		{
//			List3 const	v	= rdir.Get( spread ) * rng.Get( minSpeed, maxSpeed );
//
//			velocity.x	= v.m_Y;
//			velocity.y	= v.m_X;
//			velocity.z	= v.m_Z;
//		}
//
//		// If we are processing a list of particle elements, then load non-default values from the next element
//
//		if ( pNodeList )
//		{
//			CComPtr< IXMLDOMNode >		pNode;
//			HRESULT						hr;
//
//			hr = pNodeList->nextNode( &pNode );
//			ProcessParticle( pNode, &lifetime, &age, &color, &radius, &orientation, &position, &velocity );
//		}
//
//		// Initialize the particle
//
//		D3DXCOLOR	rgba	= D3DXCOLOR( float( color & 0x000000ffU ) / float( 0x000000ffU ),
//										 float( color & 0x0000ff00U ) / float( 0x0000ff00U ),
//										 float( color & 0x00ff0000U ) / float( 0x00ff0000U ),
//										 float( color & 0xff000000U ) / float( 0xff000000U ) );
//
////		paParticles[ i ].Initialize( lifetime, age, rgba, position, velocity );
//	}
// }
//
//
//
//
//
/// ********************************************************************************************************************/
/// *																													*/
/// *																													*/
/// ********************************************************************************************************************/
//
// void XmlConfiguration::ProcessParticle( IXMLDOMNode *		pNode,
//										  float *			pLifetime,
//										  float *			pAge,
//										  D3DXVECTOR3 *		pVelocity,
//										  D3DXVECTOR3 *		pPosition,
//										  uint32 *			pColor,
//										  float *			pRadius,
//										  float *			pRotation,
//										  D3DXQUATERINION *	pOrientation )
// {
//	HRESULT		hr;
//
//	if ( Msxmlx::IsElementNode( pNode ) )
//	{
//		CComQIPtr< IXMLDOMElement >	pElement( pNode );
//		CComBSTR					tag;
//
//		hr = pElement->get_tagName( &tag );
//
//		if ( tag == CComBSTR( "Particle" ) )
//		{
//			//	<xsd:complexType name="particle">
//			//		<xsd:all>
//			//			<xsd:element name="Lifetime" type="xsd:float" minOccurs="0" />
//			//			<xsd:element name="Age" type="xsd:float" minOccurs="0" />
//			//			<xsd:element name="Position" type="vector3" />
//			//			<xsd:element name="Velocity" type="vector3" />
//			//			<xsd:element name="Color" type="rgba" minOccurs="0" />
//			//			<xsd:element name="Radius" type="xsd:float" minOccurs="0" />
//			//			<xsd:element name="Rotation" type="xsd:float" minOccurs="0" />
//			//			<xsd:element name="Orientation" type="quaternion" minOccurs="0" />
//			//		</xsd:all>
//			//	</xsd:complexType>
//
//			*pPosition		= GetD3DXVECTOR3SubElement(    pElement, "Position",    *pPosition );
//			*pVelocity		= GetD3DXVECTOR3SubElement(    pElement, "Velocity",    *pVelocity );
//			*pLifetime		= Msxmlx::GetFloatSubElement(  pElement, "Lifetime",    *pLifetime );
//			*pAge			= Msxmlx::GetFloatSubElement(  pElement, "Age",	        *pAge );
//			*pColor			= Msxmlx::GetHexSubElement(    pElement, "Color",	    *pColor );
//			*pRadius		= Msxmlx::GetFloatSubElement(  pElement, "Radius",	    *pRadius );
//			*pRotation		= Msxmlx::GetFloatSubElement(  pElement, "Rotation",	*pRotation );
//			*pOrientation	= GetD3DXQUATERNIONSubElement( pElement, "Orientation",	*pOrientation );
//
// #if defined( _DEBUG )
//			{
//				std::ostringstream	msg;
//				msg << "    Particle: "
//					<< *pLifetime << ", "
//					<< *pAge << ", "
//					<< "[" << *pVelocity << "],"
//					<< "[" << *pPosition << "] )"
//					<< *pColor << ", "
//					<< *pRadius << ", "
//					<< *pRotation << ", "
//					<< *pOrientation << ", "
//					<< std::endl;
//				OutputDebugString( msg.str().c_str() );
//			}
// #endif // defined( _DEBUG )
//		}
//	}
// }
} // namespace Confetti
