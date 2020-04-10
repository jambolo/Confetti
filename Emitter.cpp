#include "Emitter.h"

#include "Appearance.h"
#include "Particle.h"
#include "resource.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"

#include <Vkx/Camera.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <algorithm>

// This class sorts particles back-to-front
class ParticleSorter
{
public:
    ParticleSorter(glm::vec3 const & cameraPosition)
        : cameraPosition_(cameraPosition)
    {
    }

    bool operator ()(Confetti::Particle const & a, Confetti::Particle const & b)
    {
        glm::vec3 da = a.position() - cameraPosition_;
        glm::vec3 db = b.position() - cameraPosition_;
        return glm::length2(da) > glm::length2(db);
    }

    glm::vec3 cameraPosition_;
};

namespace Confetti
{
//! @param  volume          Emitter volume.
//! @param  environment     Environment applied to all particles.
//! @param  appearance      Appearance shared by all particles.
//! @param  sorted          True, if the particles should be sorted back-to-front when updated

BasicEmitter::BasicEmitter(std::shared_ptr<EmitterVolume> volume,
                           std::shared_ptr<Environment>   environment,
                           std::shared_ptr<Appearance>    appearance,
                           bool                           sorted)
    : volume_(volume)
    , appearance_(appearance)
    , environment_(environment)
    , sorted_(sorted)
    , enabled_(true)
    , position_({ 0.0f, 0.0f, 0.0f })
    , velocity_({ 0.0f, 0.0f, 0.0f })
{
//     // Check caps bits for certain features
//     {
// //         D3DCAPS9 caps;
// //
// //         pD3dDevice_->GetDeviceCaps(&caps);
// //
// //         alphaTestAvailable_ = ((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0);
// //         maxPrimitives_      = caps.MaxPrimitiveCount;
// //         maxVertexIndex_     = caps.MaxVertexIndex;
//     }
//
//     D3D11_BUFFER_DESC desc =
//     {
//         (UINT)(n * size),           // UINT        ByteWidth;
//         D3D11_USAGE_DYNAMIC,        // D3D11_USAGE Usage;
//         D3D11_BIND_VERTEX_BUFFER,   // UINT        BindFlags;
//         D3D11_CPU_ACCESS_WRITE,     // UINT        CPUAccessFlags;
//         0,                          // UINT        MiscFlags;
//         0                           // UINT        StructureByteStride;
//     };
//
//     HRESULT hr = device->CreateBuffer(&desc, nullptr, &pVB_);
//     assert_succeeded(hr);
}

//! @param  dt          Amount of time elapsed since the last update
//! @param  position    The new position of the emitter.
//! @param  velocity    The new velocity of the emitter.

void BasicEmitter::update(float dt, glm::vec3 const & position, glm::vec3 const & velocity)
{
    // Update the emitter's position and velocity

    update(position, velocity);

    // Update the emitter's particles

    update(dt);
}

//! @param enabled  If true, the emitter is enabled, otherwise the emitter is disabled.

bool BasicEmitter::enable(bool enable /*= true*/)
{
    bool oldState = enabled_;
    enabled_ = enable;
    return oldState;
}

//! @param  position    The new position of the emitter.
//! @param  velocity    The new velocity of the emitter.

void BasicEmitter::update(glm::vec3 const & position, glm::vec3 const & velocity)
{
    position_ = position;
    velocity_ = velocity;
}

/********************************************************************************************************************/
/*                                   P O I N T   P A R T I C L E   E M I T T E R                                    */
/********************************************************************************************************************/
//! @param  volume          Emitter volume.
//! @param  environment     Environment.
//! @param  appearance      Appearance.
//! @param  n               Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

PointEmitter::PointEmitter(int                            n,
                           std::shared_ptr<EmitterVolume> volume,
                           std::shared_ptr<Environment>   environment,
                           std::shared_ptr<Appearance>    appearance,
                           bool                           sorted)
    : BasicEmitter(volume, environment, appearance, sorted)
    , particles_(n)
{
    initialize();
}

//! @param  particles       Particle array (the emitter assumes ownership).
//! @param  volume          Emitter volume.
//! @param  environment     Environment applied to all particles.
//! @param  appearance      Appearance shared by all particles.
//! @param  n               Number of particles to create.
//!
//! @warning particles must have been allocated with new[].

PointEmitter::PointEmitter(std::vector<PointParticle>     particles,
                           std::shared_ptr<EmitterVolume> volume,
                           std::shared_ptr<Environment>   environment,
                           std::shared_ptr<Appearance>    appearance,
                           bool                           sorted)
    : BasicEmitter(volume, environment, appearance, sorted)
    , particles_(particles)
{
    initialize();
}

PointEmitter::~PointEmitter()
{
    uninitialize();
}

void PointEmitter::initialize()
{
    // Point the particles' emitter value here

    for (auto & p : particles_)
    {
        p.bind(this);
    }
#if 0
    // Load the shader
    {
        wchar_t shaderSource[] =
            L"texture g_texture;                                                    "
            L"float4x4 g_viewProjection : ViewProjection;                           "
            L"float4x4 g_cameraOrientation : InverseView;                           "
            L"                                                                      "
            L"sampler s0 = sampler_state                                            "
            L"{                                                                     "
            L"    Texture = (g_texture);                                            "
            L"    MinFilter = LINEAR;                                               "
            L"    MagFilter = LINEAR;                                               "
            L"    MipFilter = LINEAR;                                               "
            L"    AddressU = CLAMP;                                                 "
            L"    AddressV = CLAMP;                                                 "
            L"};                                                                    "
            L"                                                                      "
            L"float4 RotateZ(float4 v, float angle)                                 "
            L"{                                                                     "
            L"    float s, c;                                                       "
            L"    sincos(angle, s, c);                                              "
            L"                                                                      "
            L"    float4 result;                                                    "
            L"                                                                      "
            L"    result.x = c * v.x - s * v.y;                                     "
            L"    result.y = s * v.x + c * v.y;                                     "
            L"    result.z = v.z;                                                   "
            L"    result.w = v.w;                                                   "
            L"                                                                      "
            L"    return result;                                                    "
            L"}                                                                     "
            L"                                                                      "
            L"struct VSOutput                                                       "
            L"{                                                                     "
            L"    float4 position : POSITION;                                       "
            L"    float4 color : COLOR;                                             "
            L"    float2 texturePosition : TEXCOORD0;                               "
            L"};                                                                    "
            L"                                                                      "
            L"VSOutput VS(float4 position : POSITION0,                              "
            L"    float4 color : COLOR0,                                            "
            L"    float2 texturePosition : TEXCOORD0,                               "
            L"    float radius : TEXCOORD1,                                         "
            L"    float rotation : TEXCOORD2)                                       "
            L"{                                                                     "
            L"    VSOutput result;                                                  "
            L"                                                                      "
            L"    // Convert the texture position into the offset to the vertex     "
            L"    float4 offset = { (2.0 * texturePosition - 1.0) * radius, 0, 0 }; "
            L"                                                                      "
            L"    // Rotate                                                         "
            L"    offset = RotateZ(offset, rotation);                               "
            L"                                                                      "
            L"    // Align the offset with the camera's view plane                  "
            L"    float4 worldOffset = mul(offset, g_cameraOrientation);            "
            L"                                                                      "
            L"    // Set return values                                              "
            L"    result.position = mul(position + worldOffset, g_viewProjection);  "
            L"    result.color = color;                                             "
            L"    result.texturePosition = texturePosition;                         "
            L"                                                                      "
            L"    return result;                                                    "
            L"}                                                                     "
            L"                                                                      "
            L"struct PSOutput                                                       "
            L"{                                                                     "
            L"    float4 color : COLOR;                                             "
            L"};                                                                    "
            L"                                                                      "
            L"PSOutput PS(float4 color : COLOR,                                     "
            L"    float2 texturePosition : TEXCOORD)                                "
            L"{                                                                     "
            L"    PSOutput result;                                                  "
            L"                                                                      "
            L"    result.color = color * tex2D(s0, texturePosition);                "
            L"                                                                      "
            L"    return result;                                                    "
            L"}                                                                     "
            L"                                                                      "
            L"technique T0                                                          "
            L"{                                                                     "
            L"    pass P0                                                           "
            L"    {                                                                 "
            L"        Sampler[0] = (s0);                                            "
            L"        VertexShader = compile vs_2_0 VS();                           "
            L"        PixelShader = compile ps_2_0 PS();                            "
            L"    }                                                                 "
            L"}                                                                     "
        ;

#if defined(_DEBUG)
        static UINT constexpr D3DCOMPILE_FLAGS = D3DCOMPILE_DEBUG |
                                                 D3DCOMPILE_SKIP_OPTIMIZATION |
                                                 D3DCOMPILE_IEEE_STRICTNESS |
                                                 D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
        static UINT constexpr D3DCOMPILE_FLAGS = D3DCOMPILE_OPTIMIZATION_LEVEL3 |
                                                 D3DCOMPILE_IEEE_STRICTNESS |
                                                 D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

        ID3DBlob * pErrorMsgs = nullptr;
        hr = D3DCompile(shaderSource,
                        sizeof(shaderSource) - 1,
                        (LPCSTR)L"PointEmitter",
                        NULL,
                        NULL,
                        (LPCSTR)"T0",
                        (LPCSTR)"fx_2_0",
                        D3DCOMPILE_FLAGS,
                        0,
                        &pEffect_,
                        &pErrorMsgs);
        if (FAILED(hr) && pErrorMsgs && pErrorMsgs->GetBufferPointer())
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);
        Wx::SafeRelease(pErrorMsgs);

//         D3DXHANDLE hTechnique;
//         hr = pEffect_->FindNextValidTechnique(NULL, &hTechnique);
//         assert_succeeded(hr);
//
//         pEffect_->SetTechnique(hTechnique);
    }

//     hr = pD3dDevice_->CreateVertexDeclaration(PointParticle::aVSDataDeclarationInfo_, &pVertexDeclaration_);
//     assert_succeeded(hr);
#endif  // if 0
}

void PointEmitter::uninitialize()
{
}

//! @param  dt  Amount of time elapsed since the last update

void PointEmitter::update(float dt)
{
    for (auto & p : particles_)
    {
        p.update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(particles_.begin(),
                  particles_.end(),
                  ParticleSorter(appearance()->camera->position()));
    }
}

void PointEmitter::draw() const
{
#if 0
    std::shared_ptr<Appearance> appearance = appearance();
    int nParticles = particles_.size();                     // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable antialiasing so points less than a pixel in size are still drawn (but faded).

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

// // Set point size
//
// float size = GetAppearance()->particles_.size();
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE, *(DWORD*)&size );
// assert_succeeded( hr );
//
// float minSize = 0.5f;
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE_MIN, *(DWORD*)&minSize );
// assert_succeeded( hr );
//
// // Enable point scaling
//
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALEENABLE, TRUE );
// assert_succeeded( hr );
//
// // Set point scale factors
//
// float a = 0.0f;
// float b = 0.0f;
// float c = 1.0f; // FIXME: This should be tan( a/2 ) ** 2
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_A, *(DWORD*)&a );
// assert_succeeded( hr );
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_B, *(DWORD*)&b );
// assert_succeeded( hr );
// hr = Vkx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_C, *(DWORD*)&c );
// assert_succeeded( hr );

    // Turn on alphablending

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

// glm::mat4x4 view   = pAppearance->camera()->GetViewMatrix();
// glm::mat4x4 projection  = pAppearance->camera()->GetProjectionMatrix();
// glm::mat4x4 viewProjection = pAppearance->camera()->GetViewProjectionMatrix();
// pEffect_->SetMatrix( "ViewMatrix", &view );
// pEffect_->SetMatrix( "ProjectionMatrix", &projection );
// pEffect_->SetMatrix( "ViewProjectionMatrix", &view );
    pEffect_->SetFloat("Size", appearance()->particles_.size());

    // Set up vertex data

    hr = pD3dDevice_->SetVertexDeclaration(pVertexDeclaration_);
    assert_succeeded(hr);

    hr = pD3dDevice_->SetStreamSource(0, pVB_, 0, sizeof(PointParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    int particlesPerGroup = std::min(std::min(VERTEX_BUFFER_SIZE / (int)sizeof(PointParticle::VBEntry), maxPrimitives_),
                                     nParticles);

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;                      // Number of particles to be rendered this block

        {
            Vkx::VertexBufferLock lock(pVB_, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

            PointParticle::VBEntry * paVB      = (PointParticle::VBEntry *)lock.GetLockedBuffer();
            PointParticle const *    particles = particles();

            while (i < nParticles && count < particlesPerGroup)
            {
                PointParticle const * pParticle = &particles_[i];      // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    PointParticle::VBEntry * pVBEntry = &paVB[count];           // Convenience

                    pVBEntry->v[0].position = pParticle->GetPosition();
                    pVBEntry->v[0].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().x,
                                                                  pParticle->GetColor().y,
                                                                  pParticle->GetColor().z,
                                                                  pParticle->GetColor().w);

                    ++count;
                }

                ++i;
            }
        }

        // Render the group of particles (if there are any to render)

        if (count > 0)
        {
            UINT nPasses;

            pEffect_->Begin(&nPasses, 0);
            assert(nPasses == 1);

            pEffect_->BeginPass(0);
            pEffect_->CommitChanges();
            pD3dDevice_->DrawPrimitive(D3DPT_POINTLIST, 0, count);
            pEffect_->EndPass();

            pEffect_->End();
        }
    }
#endif  // if 0
}

/********************************************************************************************************************/
/*                                  S T R E A K   P A R T I C L E   E M I T T E R                                   */
/********************************************************************************************************************/

//! @param  n               Number of particles to create.
//! @param  volume          Emitter volume.
//! @param  environment     Environment.
//! @param  appearance      Appearance.
//! @param  sorted          If true, then the particles will be sorted back to front during the update
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

StreakEmitter::StreakEmitter(int                            n,
                             std::shared_ptr<EmitterVolume> volume,
                             std::shared_ptr<Environment>   environment,
                             std::shared_ptr<Appearance>    appearance,
                             bool                           sorted)
    : BasicEmitter(volume, environment, appearance, sorted)
    , particles_(n)

{
    initialize();
}

//! @param  particles       Particle array (the emitter assumes ownership).
//! @param  volume          Emitter volume.
//! @param  environment     Environment applied to all particles.
//! @param  appearance      Appearance shared by all particles.
//! @param  sorted          If true, then the particles will be sorted back to front during the update

StreakEmitter::StreakEmitter(std::vector<StreakParticle>    particles,
                             std::shared_ptr<EmitterVolume> volume,
                             std::shared_ptr<Environment>   environment,
                             std::shared_ptr<Appearance>    appearance,
                             bool                           sorted)
    : BasicEmitter(volume, environment, appearance, sorted)
    , particles_(particles)
{
    initialize();
}

StreakEmitter::~StreakEmitter()
{
    uninitialize();
}

//!
//! @param dt Amount of time elapsed since the last update

void StreakEmitter::update(float dt)
{
    for (auto & p : particles_)
    {
        p.update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(particles_.begin(),
                  particles_.end(),
                  ParticleSorter(appearance()->camera->position()));
    }
}

void StreakEmitter::initialize()
{
    // Point the particles' emitter value here

    for (auto & p : particles_)
    {
        p.bind(this);
    }
#if 0
    // Load the effects file

    {
        std::shared_ptr<Vkx::LocalBuffer> pErrorMsgs;
        hr = D3DXCreateEffectFromFile(pD3dDevice_, L"../res/StreakParticle.fxo", NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
//  hr = D3DXCreateEffectFromFile( pD3dDevice_, _T( "res/StreakParticle.fxo" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs );
//  hr = D3DXCreateEffectFromResource( pD3dDevice_, NULL, _T( "IDR_STREAKEFFECT" ), NULL, NULL, 0, NULL, &pEffect_,
// &pErrorMsgs );
        if (FAILED(hr) && pErrorMsgs != 0 && pErrorMsgs->GetBufferPointer() != 0)
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);

        D3DXHANDLE hTechnique;
        hr = pEffect_->FindNextValidTechnique(NULL, &hTechnique);
        assert_succeeded(hr);

        pEffect_->SetTechnique(hTechnique);

        Wx::SafeRelease(pErrorMsgs);
    }

    hr = pD3dDevice_->CreateVertexDeclaration(StreakParticle::aVSDataDeclarationInfo_, &pVertexDeclaration_);
    assert_succeeded(hr);
#endif  // if 0
}

void StreakEmitter::uninitialize()
{
}

void StreakEmitter::draw() const
{
#if 0
    std::shared_ptr<Appearance> appearance = appearance();
    int nParticles = particles_.size();                     // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable line antialiasing.

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

    glm::mat4x4 const & viewProjection = pAppearance->camera()->viewProjectionMatrix();
    pEffect_->SetMatrix("ViewProjectionMatrix", &viewProjection);

    // Set up vertex data

    hr = pD3dDevice_->SetVertexDeclaration(pVertexDeclaration_);
    assert_succeeded(hr);

    hr = pD3dDevice_->SetStreamSource(0, pVB_, 0, sizeof(StreakParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    // Render

    int particlesPerGroup = std::min(std::min(VERTEX_BUFFER_SIZE / (int)sizeof(StreakParticle::VBEntry),
                                              maxPrimitives_),
                                     nParticles);

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;

        {
            Vkx::VertexBufferLock lock(pVB_, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

            StreakParticle::VBEntry * paVB      = (StreakParticle::VBEntry *)lock.GetLockedBuffer();
            StreakParticle const *    particles = particles();

            while (i < nParticles && count < particlesPerGroup)
            {
                StreakParticle const * const pParticle = &particles_[i];       // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    StreakParticle::VBEntry * pVBEntry = &paVB[count];          // Convenience

                    pVBEntry->v[0].position = pParticle->GetPosition();
                    pVBEntry->v[0].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().x,
                                                                  pParticle->GetColor().y,
                                                                  pParticle->GetColor().z,
                                                                  pParticle->GetColor().w);

                    pVBEntry->v[1].position = pParticle->GetTailPosition();
                    pVBEntry->v[1].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().x,
                                                                  pParticle->GetColor().y,
                                                                  pParticle->GetColor().z,
                                                                  0.0f);

                    ++count;
                }

                ++i;
            }
        }

        // If there are no particles, there is nothing to draw

        if (count > 0)
        {
            UINT nPasses;

            pEffect_->Begin(&nPasses, 0);
            assert(nPasses == 1);

            pEffect_->BeginPass(0);
            pEffect_->CommitChanges();
            pD3dDevice_->DrawPrimitive(D3DPT_LINELIST, 0, count);
            pEffect_->EndPass();

            pEffect_->End();
        }
    }
#endif  // if 0
}

/********************************************************************************************************************/
/*                                  T E X T U R E D   P A R T I C L E   E M I T T E R                               */
/********************************************************************************************************************/

//! @param volume Emitter volume.
//! @param environment Environment.
//! @param appearance Appearance.
//! @param n  Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

TexturedEmitter::TexturedEmitter(std::shared_ptr<Vkx::Device>   device,
                                 int                            n,
                                 std::shared_ptr<EmitterVolume> volume,
                                 std::shared_ptr<Environment>   environment,
                                 std::shared_ptr<Appearance>    appearance,
                                 bool                           sorted)
    : BasicEmitter(device, sizeof(TexturedParticle::VBEntry), volume, environment, appearance, sorted)
    , particles_(n)
{
    initialize();
}

//! @param particles  Particle array (the emitter assumes ownership).
//! @param volume   Emitter volume.
//! @param environment   Environment applied to all particles.
//! @param appearance   Appearance shared by all particles.
//! @param n    Number of particles to create.
//!
//! @warning particles must have been allocated with new[].

TexturedEmitter::TexturedEmitter(std::shared_ptr<Vkx::Device>   device,
                                 std::vector<TexturedParticle>  particles,
                                 std::shared_ptr<EmitterVolume> volume,
                                 std::shared_ptr<Environment>   environment,
                                 std::shared_ptr<Appearance>    appearance,
                                 bool                           sorted)
    : BasicEmitter(device, sizeof(TexturedParticle::VBEntry), volume, environment, appearance, sorted)
    , particles_(particles)
{
    initialize();
}

TexturedEmitter::~TexturedEmitter()
{
    uninitialize();
}

void TexturedEmitter::initialize()
{
    // Point the particles' emitter value here
    for (auto & p : particles_)
    {
        p.bind(this);
    }
#if 0
    // Figure out the maximum necessary size of the index buffer. It is the minimum of the following:
    //
    // 1. The number of particles * 6 ( 2 triangles per particle, 3 verts per triangle
    // 2. Max number of particles that the vertex buffer holds times 6. (6 indexes per particle)
    // 3. The limit specified by the hardware caps.

    int nIndexes = particles_.size() * 6;
    if (nIndexes > VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6)
        nIndexes = VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6;
    if (nIndexes > maxPrimitives_ * 3)
        nIndexes = maxPrimitives_ * 3;

    // Create the index buffer
    D3D11_BUFFER_DESC      desc;
    D3D11_SUBRESOURCE_DATA data;

    hr = pD3dDevice_->CreateBuffer(&desc, &data, &pIB_);
//     hr = pD3dDevice_->CreateIndexBuffer(nIndexes * sizeof(__int16),
//                                         D3DFMT_INDEX16,
//                                         &pIB_,
//                                         NULL);
    assert_succeeded(hr);

    // Load the effects file

    {
        std::shared_ptr<Vkx::LocalBuffer> pErrorMsgs;
        hr =
            D3DXCreateEffectFromFile(pD3dDevice_, _T("../res/TexturedParticle.fxo"), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
//  hr = D3DXCreateEffectFromFile( pD3dDevice_, _T( "res/TexturedParticle.fxo" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs
// );
//  hr = D3DXCreateEffectFromResource( pD3dDevice_, NULL, _T( "IDR_TEXTUREDEFFECT" ), NULL, NULL, 0, NULL, &pEffect_,
// &pErrorMsgs );
        if (FAILED(hr) && pErrorMsgs != 0 && pErrorMsgs->GetBufferPointer() != 0)
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);

        D3DXHANDLE hTechnique;
        hr = pEffect_->FindNextValidTechnique(NULL, &hTechnique);
        assert_succeeded(hr);

        pEffect_->SetTechnique(hTechnique);

        Wx::SafeRelease(pErrorMsgs);
    }

    hr = pD3dDevice_->CreateVertexDeclaration(TexturedParticle::aVSDataDeclarationInfo_, &pVertexDeclaration_);
    assert_succeeded(hr);
#endif  // if 0
}

void TexturedEmitter::uninitialize()
{
}

//!
//! @param dt Amount of time elapsed since the last update

void TexturedEmitter::update(float dt)
{
    for (auto & p : particles_)
    {
        p.update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(particles_.begin(),
                  particles_.end(),
                  ParticleSorter(appearance()->camera->position()));
    }
}

void TexturedEmitter::draw() const
{
#if 0
    std::shared_ptr<Appearance>   appearance = appearance();
    std::shared_ptr<Vkx::Texture> pTexture   = appearance->texture();
    int nParticles = particles_.size();                         // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No back-face culling

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_CULLMODE, D3DCULL_NONE);
    assert_succeeded(hr);

    // No lighting

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Enable alpha testing (skips pixels with less than a certain alpha). This helps with unsorted particles since
    // the Z-buffer is not set for nearly-transparent pixels and particles drawn behind them will not be rejected.
    // Also, since the particles are triangles, there are large areas of transparent pixels, this will speed up
    // rendering by rejecting those pixels.

    if (alphaTestAvailable_)
    {
        pD3dDevice_->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        pD3dDevice_->SetRenderState(D3DRS_ALPHAREF, 8);
        pD3dDevice_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    }

    // ???
    hr = Vkx::SetRenderState(pD3dDevice_, D3DRS_DITHERENABLE, TRUE);
    assert_succeeded(hr);

    // Set effect variables

    glm::mat4x4 viewProjection = pAppearance->camera()->viewProjectionMatrix();

    pEffect_->SetMatrix("g_viewProjection", &viewProjection);
    pEffect_->SetTexture("g_texture", pTexture);

    // Get the camera's (and thus particles') X and Y axes

    glm::mat4x4 cameraOrientation = pAppearance->camera()->frame().GetOrientationMatrix();
    pEffect_->SetMatrix("g_cameraOrientation", &cameraOrientation);

    // Set up vertex data

    hr = pD3dDevice_->SetVertexDeclaration(pVertexDeclaration_);
    assert_succeeded(hr);

    hr = pD3dDevice_->SetStreamSource(0, pVB_, 0, sizeof(TexturedParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    hr = pD3dDevice_->SetIndices(pIB_);
    assert_succeeded(hr);

    // Render

    int particlesPerGroup = nParticles;
    if (particlesPerGroup > VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry))
        particlesPerGroup = VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry);
    if (particlesPerGroup * TexturedParticle::VBEntry::NUM_VERTICES - 1 > maxVertexIndex_)
        particlesPerGroup = (maxVertexIndex_ + 1) / TexturedParticle::VBEntry::NUM_VERTICES;
    if (particlesPerGroup > maxPrimitives_)
        particlesPerGroup = maxPrimitives_;

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;                      // Number of particles to be rendered this block
        unsigned __int16 baseIndex = 0;         //

        {
            Vkx::VertexBufferLock vbLock(pVB_, 0, particlesPerGroup * sizeof(TexturedParticle::VBEntry), D3DLOCK_DISCARD);
            Vkx::IndexBufferLock  ibLock(pIB_, 0, particlesPerGroup * 6 * sizeof(__int16), D3DLOCK_DISCARD);

            TexturedParticle::VBEntry * paVB      = (TexturedParticle::VBEntry *)vbLock.GetLockedBuffer();
            unsigned __int16 *          pIB       = (unsigned __int16 *)ibLock.GetLockedBuffer();
            TexturedParticle const *    particles = particles();

            // Draw each particle

            while (i < nParticles && count < particlesPerGroup)
            {
                TexturedParticle const * const pParticle = &particles_[i];     // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    TexturedParticle::VBEntry * pVBEntry = &paVB[count];        // Convenience

                    float     radius   = pParticle->GetRadius();
                    glm::vec4 position = pParticle->GetPosition();
                    float     rotation = pParticle->GetRotation();
                    glm::vec4 color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
                                                             pParticle->GetColor().g,
                                                             pParticle->GetColor().b,
                                                             pParticle->GetColor().a);

                    /****************************************************************************************************/

                    pVBEntry->v[0].u        = 0.0f;
                    pVBEntry->v[0].v        = 0.0f;
                    pVBEntry->v[0].position = position;
                    pVBEntry->v[0].color    = color;
                    pVBEntry->v[0].radius   = radius;
                    pVBEntry->v[0].rotation = rotation;

                    /****************************************************************************************************/

                    pVBEntry->v[1].u        = 0.0f;
                    pVBEntry->v[1].v        = 1.0f;
                    pVBEntry->v[1].position = position;
                    pVBEntry->v[1].color    = color;
                    pVBEntry->v[1].radius   = radius;
                    pVBEntry->v[1].rotation = rotation;

                    /****************************************************************************************************/

                    pVBEntry->v[2].u        = 1.0f;
                    pVBEntry->v[2].v        = 1.0f;
                    pVBEntry->v[2].position = position;
                    pVBEntry->v[2].color    = color;
                    pVBEntry->v[2].radius   = radius;
                    pVBEntry->v[2].rotation = rotation;

                    /****************************************************************************************************/

                    pVBEntry->v[3].u        = 1.0f;
                    pVBEntry->v[3].v        = 0.0f;
                    pVBEntry->v[3].position = position;
                    pVBEntry->v[3].color    = color;
                    pVBEntry->v[3].radius   = radius;
                    pVBEntry->v[3].rotation = rotation;

                    /****************************************************************************************************/

                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 0;
                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 1;
                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 3;

                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 3;
                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 1;
                    *pIB++ = count * TexturedParticle::VBEntry::NUM_VERTICES + 2;

                    /****************************************************************************************************/

                    ++count;
                }

                ++i;
            }
        }

        // If there are no particles, there is nothing to draw

        if (count > 0)
        {
            UINT nPasses;

            pEffect_->Begin(&nPasses, 0);
            assert(nPasses == 1);

            pEffect_->BeginPass(0);
            pEffect_->CommitChanges();
            pD3dDevice_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                              0,
                                              0,
                                              count * TexturedParticle::VBEntry::NUM_VERTICES,
                                              0,
                                              count * 2);
            pEffect_->EndPass();
            pEffect_->End();
        }
    }
#endif  // if 0
}

//! @param volume Emitter volume.
//! @param environment Environment.
//! @param appearance Appearance.
//! @param n  Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

SphereEmitter::SphereEmitter(std::shared_ptr<Vkx::Device>   device,
                             int                            n,
                             std::shared_ptr<EmitterVolume> volume,
                             std::shared_ptr<Environment>   environment,
                             std::shared_ptr<Appearance>    appearance,
                             bool                           sorted)
    : BasicEmitter(device, sizeof(SphereParticle::VBEntry), volume, environment, appearance, sorted)
    , particles_(n)
{
    initialize();
}

//! @param particles  Particle array (the emitter assumes ownership).
//! @param volume   Emitter volume.
//! @param environment   Environment applied to all particles.
//! @param appearance   Appearance shared by all particles.
//! @param n    Number of particles to create.
//!
//! @warning particles must have been allocated with new[].

SphereEmitter::SphereEmitter(std::shared_ptr<Vkx::Device>   device,
                             std::vector<SphereParticle>    particles,
                             std::shared_ptr<EmitterVolume> volume,
                             std::shared_ptr<Environment>   environment,
                             std::shared_ptr<Appearance>    appearance,
                             bool                           sorted)
    : BasicEmitter(device, sizeof(SphereParticle::VBEntry), volume, environment, appearance, sorted)
    , particles_(particles)
{
    initialize();
}

SphereEmitter::~SphereEmitter()
{
    uninitialize();
}

//!
//! @param dt Amount of time elapsed since the last update

void SphereEmitter::update(float dt)
{
    for (auto & p : particles_)
    {
        p.update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(particles_.begin(),
                  particles_.end(),
                  ParticleSorter(appearance()->camera->position()));
    }
}

void SphereEmitter::initialize()
{
    // Point the particles' emitter value here

    for (auto & p : particles_)
    {
        p.bind(this);
    }
}

void SphereEmitter::uninitialize()
{
}

void SphereEmitter::draw() const
{
}
} // namespace Confetti
