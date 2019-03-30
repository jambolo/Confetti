#if !defined(CONFETTI_XMLCONFIGURATION_H)
#define CONFETTI_XMLCONFIGURATION_H

#pragma once

#include <Confetti/Configuration.h>

struct IXMLDOMDocument2;
struct IXMLDOMElement;
struct IXMLDOMNodeVector;
struct IXMLDOMNode;

namespace Confetti
{
//! A Configuration loaded from XML.
class XmlConfiguration : public Configuration
{
public:
    //! Constructor.
    explicit XmlConfiguration(char const * path);

    //! Constructor.
    explicit XmlConfiguration(IXMLDOMDocument2 * doc);

    //! Destructor.
    virtual ~XmlConfiguration() override = default;

    //! Saves the configuration to an XML DOM.
    bool toXml(IXMLDOMDocument2 * document);

private:
    bool load(char const * path);
    bool load(IXMLDOMDocument2 * document);

    bool processSurface(IXMLDOMElement * element, SurfaceList & list);
    bool processSurfaceList(IXMLDOMElement * element, SurfaceListMap & lists);
    bool processClipper(IXMLDOMElement * element, ClipperList & planes);
    bool processClipperList(IXMLDOMElement * element, ClipperListMap & lists);
    bool processEnvironment(IXMLDOMElement * element, EnvironmentMap & environments);
    bool processAppearance(IXMLDOMElement * element, AppearanceMap & appearances);
    bool processVolume(IXMLDOMElement * element, EmitterVolumeMap & volumes);
    bool processEmitter(IXMLDOMElement * element, EmitterMap & emitters);
    //         static void processPointParticles(PointEmitter *        pEmitter,
    //                                           IXMLDOMNodeVector *   pNodeVector,
    //                                           int                   numParticles,
    //                                           Configuration const * pConfiguration,
    //                                           float                 globalLifetime,
    //                                           uint32_t              globalColor,
    //                                           float                 spread,
    //                                           float                 minSpeed,
    //                                           float                 maxSpeed);
    //         static void processStreakParticles(StreakEmitter *       pEmitter,
    //                                            IXMLDOMNodeVector *   pNodeVector,
    //                                            int                   numParticles,
    //                                            Configuration const * pConfiguration,
    //                                            float                 globalLifetime,
    //                                            uint32_t              globalColor,
    //                                            float                 spread,
    //                                            float                 minSpeed,
    //                                            float                 maxSpeed);
    //         static void processTexturedParticles(TexturedEmitter *     pEmitter,
    //                                              IXMLDOMNodeVector *   pNodeVector,
    //                                              int                   numParticles,
    //                                              Configuration const * pConfiguration,
    //                                              float                 globalLifetime,
    //                                              uint32_t              globalColor,
    //                                              float                 spread,
    //                                              float                 minSpeed,
    //                                              float                 maxSpeed);
    //         static void processSphereParticles(SphereEmitter *       pEmitter,
    //                                            IXMLDOMNodeVector *   pNodeVector,
    //                                            int                   numParticles,
    //                                            Configuration const * pConfiguration,
    //                                            float                 globalLifetime,
    //                                            uint32_t              globalColor,
    //                                            float                 spread,
    //                                            float                 minSpeed,
    //                                            float                 maxSpeed);
    //         static void processParticle(IXMLDOMNode * pNode,
    //                                     float *       pLifetime,
    //                                     float *       pAge,
    //                                     uint32_t *    pColor,
    //                                     float *       pRadius,
    //                                     float *       pOrientation,
    //                                     glm::vec4 * pVelocity,
    //                                     glm::vec4 * pPosition);
};
} // namespace Confetti

#endif // !defined(CONFETTI_XMLCONFIGURATION_H)
