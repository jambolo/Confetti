#include "Emitter.h"

#include "Appearance.h"
#include "Particle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"
#include "resource.h"

#include "Wx/Wx.h"

#include <d3d11.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>

#include <algorithm>

using namespace DirectX;

// Vertex buffer size
static int const VERTEX_BUFFER_SIZE = 1024 * 1024;

// This class sorts particles back-to-front
class ParticleSorter
{
public:
    ParticleSorter(DirectX::XMFLOAT3 const & cameraPosition)
        : cameraPosition_(cameraPosition)
    {
    }

    bool operator ()(Confetti::Particle const & a, Confetti::Particle const & b)
    {
        DirectX::XMVECTOR positionA_simd(XMLoadFloat3(&a.GetPosition()));
        DirectX::XMVECTOR positionB_simd(XMLoadFloat3(&b.GetPosition()));
        DirectX::XMVECTOR cameraPosition_simd(XMLoadFloat3(&cameraPosition_));

        DirectX::XMVECTOR da = positionA_simd - cameraPosition_simd;
        DirectX::XMVECTOR db = positionB_simd - cameraPosition_simd;
        return XMVector3Greater(XMVector3LengthSq(da), XMVector3LengthSq(db));
    }

    DirectX::XMFLOAT3 cameraPosition_;
};

namespace Confetti
{
//! @param paParticles  Array of particles (the emitter assumes ownership).
//! @param pVol   Emitter volume.
//! @param pEnv   Environment applied to all particles.
//! @param pApp   Appearance shared by all particles.
//! @param n    Number of particles in the array.
//! @param spread   Emission angle (actual meaning is specific to the type of emitter).
//!
//! @warning paParticles must have been allocated with new[].

BasicEmitter::BasicEmitter(ID3D11Device * pD3dDevice,
                           Particle * paParticles,
                           int size,
                           EmitterVolume const * pVol,
                           Environment const * pEnv,
                           Appearance const * pApp,
                           int n,
                           bool sorted)
    : paParticles_(paParticles)
    , pEmitterVolume_(pVol)
    , pEnvironment_(pEnv)
    , pAppearance_(pApp)
    , numParticles_(n)
    , sorted_(sorted)
    , enabled_(true)
    , position_({ 0.0f, 0.0f, 0.0f })
    , velocity_({ 0.0f, 0.0f, 0.0f })
    , pVB_(nullptr)
{
    pD3dDevice->AddRef();

    // Check caps bits for certain features
    {
//         D3DCAPS9 caps;
// 
//         pD3dDevice_->GetDeviceCaps(&caps);
// 
//         alphaTestAvailable_ = ((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0);
//         maxPrimitives_      = caps.MaxPrimitiveCount;
//         maxVertexIndex_     = caps.MaxVertexIndex;
    }

    D3D11_BUFFER_DESC desc =
    {
        (UINT)(n * size),           // UINT        ByteWidth;
        D3D11_USAGE_DYNAMIC,        // D3D11_USAGE Usage;
        D3D11_BIND_VERTEX_BUFFER,   // UINT        BindFlags;
        D3D11_CPU_ACCESS_WRITE,     // UINT        CPUAccessFlags;
        0,                          // UINT        MiscFlags;
        0                           // UINT        StructureByteStride;
    };

    HRESULT hr = pD3dDevice->CreateBuffer(&desc, nullptr, &pVB_);
    assert_succeeded(hr);
}

BasicEmitter::~BasicEmitter()
{
    Wx::SafeRelease(pVB_);
    Wx::SafeRelease(pD3dDevice_);
}

//! @param dt Amount of time elapsed since the last update
//! @param position The new position of the emitter.
//! @param velocity The new velocity of the emitter.

void BasicEmitter::update(float dt, DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & velocity)
{
    // Update the emitter's position and velocity

    update(position, velocity);

    // Update the emitter's particles

    update(dt);
}

//!
//! @param enabled  If true, the emitter is enabled, otherwise the emitter is disabled.

bool BasicEmitter::enable(bool enable /*= true*/)
{
    bool oldState = enabled_;
    enabled_ = enable;
    return oldState;
}

//! @param position The new position of the emitter.
//! @param velocity The new velocity of the emitter.

void BasicEmitter::update(DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & velocity)
{
    position_ = position;
    velocity_ = velocity;
}

/********************************************************************************************************************/
/*                                   P O I N T   P A R T I C L E   E M I T T E R                                    */
/********************************************************************************************************************/
//! @param pVol Emitter volume.
//! @param pEnv Environment.
//! @param pApp Appearance.
//! @param n  Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

PointEmitter::PointEmitter(ID3D11Device *        pD3dDevice,
                           EmitterVolume const * pVol,
                           Environment const *   pEnv,
                           Appearance const *    pApp,
                           int                   n,
                           bool                  sorted)
    : BasicEmitter(pD3dDevice,
                   new PointParticle[n],
                   sizeof(PointParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    if (!particles())
        throw std::bad_alloc();

    initialize();
}

//! @param paParticles  Particle array (the emitter assumes ownership).
//! @param pVol   Emitter volume.
//! @param pEnv   Environment applied to all particles.
//! @param pApp   Appearance shared by all particles.
//! @param n    Number of particles to create.
//!
//! @warning paParticles must have been allocated with new[].

PointEmitter::PointEmitter(ID3D11Device *               pD3dDevice,
                           std::unique_ptr<PointParticle> qaParticles,
                           EmitterVolume const *        pVol,
                           Environment const *          pEnv,
                           Appearance const *           pApp,
                           int                          n,
                           bool                         sorted)
    : BasicEmitter(pD3dDevice,
                   qaParticles.release(),
                   sizeof(PointParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    initialize();
}

PointEmitter::~PointEmitter()
{
    uninitialize();
    delete[] particles();
}

void PointEmitter::initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    PointParticle * const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Bind(this);
    }

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
            sizeof(shaderSource)-1,
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
#endif // defined( _DEBUG )
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
}

void PointEmitter::uninitialize()
{
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pVertexDeclaration_);
}

//!
//! @param dt Amount of time elapsed since the last update

void PointEmitter::update(float dt)
{
    PointParticle * const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[size()],
                  ParticleSorter(appearance()->camera()->position()));
    }
}

void PointEmitter::draw() const
{
    HRESULT hr;

    Appearance const * pAppearance = appearance();
    int nParticles = size();                     // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable antialiasing so points less than a pixel in size are still drawn (but faded).

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

// // Set point size
//
// float size = GetAppearance()->size();
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE, *(DWORD*)&size );
// assert_succeeded( hr );
//
// float minSize = 0.5f;
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE_MIN, *(DWORD*)&minSize );
// assert_succeeded( hr );
//
// // Enable point scaling
//
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALEENABLE, TRUE );
// assert_succeeded( hr );
//
// // Set point scale factors
//
// float a = 0.0f;
// float b = 0.0f;
// float c = 1.0f; // FIXME: This should be tan( a/2 ) ** 2
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_A, *(DWORD*)&a );
// assert_succeeded( hr );
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_B, *(DWORD*)&b );
// assert_succeeded( hr );
// hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_C, *(DWORD*)&c );
// assert_succeeded( hr );

    // Turn on alphablending

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

// DirectX::XMFLOAT4X4 view   = pAppearance->camera()->GetViewMatrix();
// DirectX::XMFLOAT4X4 projection  = pAppearance->camera()->GetProjectionMatrix();
// DirectX::XMFLOAT4X4 viewProjection = pAppearance->camera()->GetViewProjectionMatrix();
// pEffect_->SetMatrix( "ViewMatrix", &view );
// pEffect_->SetMatrix( "ProjectionMatrix", &projection );
// pEffect_->SetMatrix( "ViewProjectionMatrix", &view );
    pEffect_->SetFloat("Size", appearance()->size());

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
            Dxx::VertexBufferLock lock(pVB_, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

            PointParticle::VBEntry * paVB        = (PointParticle::VBEntry *)lock.GetLockedBuffer();
            PointParticle const *    paParticles = particles();

            while (i < nParticles && count < particlesPerGroup)
            {
                PointParticle const * pParticle = &paParticles[i];      // Convenience

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
}

/********************************************************************************************************************/
/*                                  S T R E A K   P A R T I C L E   E M I T T E R                                   */
/********************************************************************************************************************/

//! @param pVol Emitter volume.
//! @param pEnv Environment.
//! @param pApp Appearance.
//! @param n  Number of particles to create.
//! @param sorted If true, then the particles will be sorted back to front
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

StreakEmitter::StreakEmitter(ID3D11Device *        pD3dDevice,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(pD3dDevice,
                   new StreakParticle[n],
                   sizeof(StreakParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    if (!particles())
        throw std::bad_alloc();

    initialize();
}

//! @param paParticles  Particle array (the emitter assumes ownership).
//! @param pVol   Emitter volume.
//! @param pEnv   Environment applied to all particles.
//! @param pApp   Appearance shared by all particles.
//! @param n    Number of particles to create.
//! @param sorted If true, then the particles will be sorted back to front
//!
//! @warning paParticles must have been allocated with new[].

StreakEmitter::StreakEmitter(ID3D11Device *                pD3dDevice,
                             std::unique_ptr<StreakParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(pD3dDevice,
                   qaParticles.release(),
                   sizeof(StreakParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    initialize();
}

StreakEmitter::~StreakEmitter()
{
    uninitialize();
    delete[] particles();
}

//!
//! @param dt Amount of time elapsed since the last update

void StreakEmitter::update(float dt)
{
    StreakParticle *    const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[size()],
                  ParticleSorter(appearance()->camera()->position()));
    }
}

void StreakEmitter::initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    StreakParticle *    const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Bind(this);
    }

    // Load the effects file

    {
        ID3D11Buffer * pErrorMsgs;
        hr = D3DXCreateEffectFromFile(pD3dDevice_, _T("../res/StreakParticle.fxo"), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
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
}

void StreakEmitter::uninitialize()
{
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pVertexDeclaration_);
}

void StreakEmitter::draw() const
{
    HRESULT hr;

    Appearance const * pAppearance = appearance();
    int nParticles = size();                     // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable line antialiasing.

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

    DirectX::XMFLOAT4X4 const & viewProjection = pAppearance->camera()->viewProjectionMatrix();
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
            Dxx::VertexBufferLock lock(pVB_, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

            StreakParticle::VBEntry * paVB        = (StreakParticle::VBEntry *)lock.GetLockedBuffer();
            StreakParticle const *    paParticles = particles();

            while (i < nParticles && count < particlesPerGroup)
            {
                StreakParticle const * const pParticle = &paParticles[i];       // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    StreakParticle::VBEntry * pVBEntry = &paVB[count];          // Convenience

                    pVBEntry->v[0].position = pParticle->GetPosition();
                    pVBEntry->v[0].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
                                                                  pParticle->GetColor().g,
                                                                  pParticle->GetColor().b,
                                                                  pParticle->GetColor().a);

                    pVBEntry->v[1].position = pParticle->GetTailPosition();
                    pVBEntry->v[1].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
                                                                  pParticle->GetColor().g,
                                                                  pParticle->GetColor().b,
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
}

/********************************************************************************************************************/
/*         T E X T U R E D   P A R T I C L E   E M I T T E R                               */
/********************************************************************************************************************/

//! @param pVol Emitter volume.
//! @param pEnv Environment.
//! @param pApp Appearance.
//! @param n  Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

TexturedEmitter::TexturedEmitter(ID3D11Device *        pD3dDevice,
                                 EmitterVolume const * pVol,
                                 Environment const *   pEnv,
                                 Appearance const *    pApp,
                                 int                   n,
                                 bool                  sorted)
    : BasicEmitter(pD3dDevice,
                   new TexturedParticle[n],
                   sizeof(TexturedParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    if (!particles())
        throw std::bad_alloc();

    initialize();
}

//! @param paParticles  Particle array (the emitter assumes ownership).
//! @param pVol   Emitter volume.
//! @param pEnv   Environment applied to all particles.
//! @param pApp   Appearance shared by all particles.
//! @param n    Number of particles to create.
//!
//! @warning paParticles must have been allocated with new[].

TexturedEmitter::TexturedEmitter(ID3D11Device *                  pD3dDevice,
                                 std::unique_ptr<TexturedParticle> qaParticles,
                                 EmitterVolume const *           pVol,
                                 Environment const *             pEnv,
                                 Appearance const *              pApp,
                                 int                             n,
                                 bool                            sorted)
    : BasicEmitter(pD3dDevice,
                   qaParticles.release(),
                   sizeof(TexturedParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    initialize();
}

TexturedEmitter::~TexturedEmitter()
{
    uninitialize();
    delete[] particles();
}

void TexturedEmitter::initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    TexturedParticle *  const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Bind(this);
    }

    // Figure out the maximum necessary size of the index buffer. It is the minimum of the following:
    //
    // 1. The number of particles * 6 ( 2 triangles per particle, 3 verts per triangle
    // 2. Max number of particles that the vertex buffer holds times 6. (6 indexes per particle)
    // 3. The limit specified by the hardware caps.

    int nIndexes = size() * 6;
    if (nIndexes > VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6)
        nIndexes = VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6;
    if (nIndexes > maxPrimitives_ * 3)
        nIndexes = maxPrimitives_ * 3;

    // Create the index buffer
    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA data;

    hr = pD3dDevice_->CreateBuffer(&desc, &data, &pIB_);
//     hr = pD3dDevice_->CreateIndexBuffer(nIndexes * sizeof(__int16),
//                                         D3DFMT_INDEX16,
//                                         &pIB_,
//                                         NULL);
    assert_succeeded(hr);

    // Load the effects file

    {
        ID3D11Buffer * pErrorMsgs;
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
}

void TexturedEmitter::uninitialize()
{
    Wx::SafeRelease(pVertexDeclaration_);
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pIB_);
}

//!
//! @param dt Amount of time elapsed since the last update

void TexturedEmitter::update(float dt)
{
    TexturedParticle *  const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[size()],
                  ParticleSorter(appearance()->camera()->position()));
    }
}

void TexturedEmitter::draw() const
{
    HRESULT hr;

    Appearance const *   pAppearance = appearance();
    ID3D11Texture2D * pTexture    = pAppearance->texture();
    int nParticles = size();                         // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, sorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No back-face culling

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_CULLMODE, D3DCULL_NONE);
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
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
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_DITHERENABLE, TRUE);
    assert_succeeded(hr);

    // Set effect variables

    DirectX::XMFLOAT4X4 viewProjection = pAppearance->camera()->viewProjectionMatrix();

    pEffect_->SetMatrix("g_viewProjection", &viewProjection);
    pEffect_->SetTexture("g_texture", pTexture);

    // Get the camera's (and thus particles') X and Y axes

    DirectX::XMFLOAT4X4 cameraOrientation = pAppearance->camera()->frame().GetOrientationMatrix();
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
            Dxx::VertexBufferLock vbLock(pVB_, 0, particlesPerGroup * sizeof(TexturedParticle::VBEntry), D3DLOCK_DISCARD);
            Dxx::IndexBufferLock  ibLock(pIB_, 0, particlesPerGroup * 6 * sizeof(__int16), D3DLOCK_DISCARD);

            TexturedParticle::VBEntry * paVB        = (TexturedParticle::VBEntry *)vbLock.GetLockedBuffer();
            unsigned __int16 *          pIB         = (unsigned __int16 *)ibLock.GetLockedBuffer();
            TexturedParticle const *    paParticles = particles();

            // Draw each particle

            while (i < nParticles && count < particlesPerGroup)
            {
                TexturedParticle const * const pParticle = &paParticles[i];     // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    TexturedParticle::VBEntry * pVBEntry = &paVB[count];        // Convenience

                    float radius = pParticle->GetRadius();
                    DirectX::XMFLOAT4 position = pParticle->GetPosition();
                    float rotation          = pParticle->GetRotation();
                    DirectX::XMFLOAT4 color = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
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
}

//! @param pVol Emitter volume.
//! @param pEnv Environment.
//! @param pApp Appearance.
//! @param n  Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

SphereEmitter::SphereEmitter(ID3D11Device *        pD3dDevice,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(pD3dDevice,
                   new SphereParticle[n],
                   sizeof(SphereParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
{
    if (!particles())
        throw std::bad_alloc();

    initialize();
}

//! @param paParticles  Particle array (the emitter assumes ownership).
//! @param pVol   Emitter volume.
//! @param pEnv   Environment applied to all particles.
//! @param pApp   Appearance shared by all particles.
//! @param n    Number of particles to create.
//!
//! @warning paParticles must have been allocated with new[].

SphereEmitter::SphereEmitter(ID3D11Device *                pD3dDevice,
                             std::unique_ptr<SphereParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(pD3dDevice,
                   qaParticles.release(),
                   sizeof(SphereParticle::VBEntry),
                   pVol, pEnv, pApp, n, sorted)
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
    SphereParticle *    const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (sorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[size()],
                  ParticleSorter(appearance()->camera()->position()));
    }
}

void SphereEmitter::initialize()
{
    // Point the particles' emitter value here

    SphereParticle *    const paParticles = particles();

    for (int i = 0; i < size(); i++)
    {
        paParticles[i].Bind(this);
    }
}

void SphereEmitter::uninitialize()
{
    delete[] particles();
}

void SphereEmitter::draw() const
{
}
} // namespace Confetti
