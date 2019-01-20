/** @file *//********************************************************************************************************

                                                     Emitter.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Emitter.cpp#19 $

    $NoKeywords: $

********************************************************************************************************************/

#include "Emitter.h"

#include "Appearance.h"
#include "Particle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"

#include "resource.h"

#include <algorithm>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// Vertex buffer size
static int const VERTEX_BUFFER_SIZE = 1024 * 1024;

// This class sorts particles back-to-front.

class ParticleSorter
{
public:
    ParticleSorter(DirectX::XMFLOAT4 const & cameraPosition)
        : cameraPosition_(cameraPosition)
    {
    }

    bool operator ()(Confetti::Particle const & a, Confetti::Particle const & b)
    {
        DirectX::XMFLOAT4 da = a.GetPosition() - cameraPosition_;
        DirectX::XMFLOAT4 db = b.GetPosition() - cameraPosition_;

        return XMVector3Less(XMVector3LengthSq(da), XMVector3LengthSq(db));
    }

    DirectX::XMFLOAT4 const cameraPosition_;
};

namespace Confetti
{
//! @param	paParticles		Array of particles (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles in the array.
//! @param	spread			Emission angle (actual meaning is specific to the type of emitter).
//!
//! @warning	paParticles must have been allocated with new[].

BasicEmitter::BasicEmitter(IDirect3DDevice11 * pD3dDevice,
                           Particle * paParticles,
                           int size, uint32_t usage, D3DPOOL pool,
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
    , position_(DirectX::XMVectorZero())
    , velocity_(DirectX::XMVectorZero())
    , pD3dDevice_(pD3dDevice)
    , pVB_(0)
{
    HRESULT hr;

    pD3dDevice->AddRef();

    // Check caps bits for certain features

    {
        D3DCAPS9 caps;

        pD3dDevice_->GetDeviceCaps(&caps);

        alphaTestAvailable_ = ((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0);
        maxPrimitives_      = caps.MaxPrimitiveCount;
        maxVertexIndex_     = caps.MaxVertexIndex;
    }

    // Create the vertex buffer

    hr = pD3dDevice->CreateVertexBuffer(std::min(VERTEX_BUFFER_SIZE, n * size), usage, 0, pool, &pVB_, NULL);
    assert_succeeded(hr);
}

BasicEmitter::~BasicEmitter()
{
    Wx::SafeRelease(pVB_);
    Wx::SafeRelease(pD3dDevice_);
}

//! @param	dt	Amount of time elapsed since the last update
//! @param	position	The new position of the emitter.
//! @param	velocity	The new velocity of the emitter.

void BasicEmitter::Update(float dt, DirectX::XMFLOAT4 const & position, DirectX::XMFLOAT4 const & velocity)
{
    // Update the emitter's position and velocity

    Update(position, velocity);

    // Update the emitter's particles

    Update(dt);
}


//!
//! @param	enabled		If true, the emitter is enabled, otherwise the emitter is disabled.

bool BasicEmitter::Enable(bool enabled)
{
    bool oldState = enabled_;
    enabled_ = enabled;
    return oldState;
}

//! @param	position	The new position of the emitter.
//! @param	velocity	The new velocity of the emitter.

void BasicEmitter::Update(DirectX::XMFLOAT4 const & position, DirectX::XMFLOAT4 const & velocity)
{
    position_ = position;
    velocity_ = velocity;
}


/********************************************************************************************************************/
/*										P O I N T   P A R T I C L E   E M I T T E R                                 */
/********************************************************************************************************************/
//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

PointEmitter::PointEmitter(IDirect3DDevice11 *   pD3dDevice_,
                           EmitterVolume const * pVol,
                           Environment const *   pEnv,
                           Appearance const *    pApp,
                           int                   n,
                           bool                  sorted)
    : BasicEmitter(pD3dDevice_,
                   new PointParticle[n],
                   sizeof(PointParticle::VBEntry), PointParticle::USAGE, (D3DPOOL)PointParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

PointEmitter::PointEmitter(IDirect3DDevice11 *          pD3dDevice_,
                           std::auto_ptr<PointParticle> qaParticles,
                           EmitterVolume const *        pVol,
                           Environment const *          pEnv,
                           Appearance const *           pApp,
                           int                          n,
                           bool                         sorted)
    : BasicEmitter(pD3dDevice_,
                   qaParticles.release(),
                   sizeof(PointParticle::VBEntry), PointParticle::USAGE, (D3DPOOL)PointParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

PointEmitter::~PointEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

void PointEmitter::Initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    PointParticle * const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Bind(this);
    }

    // Load the effects file

    {
//         WCHAR str[MAX_PATH];
//         DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"BasicHLSL10.fx");
//
//         hr = D3DX11CompileFromFile(str, NULL, NULL, pFunctionName, pProfile, D3D10_SHADER_ENABLE_STRICTNESS, NULL, NULL, &pBlob,
// &pErrorBlob, NULL);
        ID3DXBuffer * pErrorMsgs;
        hr = D3DXCreateEffectFromFile(pD3dDevice_, _T("../res/PointParticle.fxo"), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( pD3dDevice_, _T( "res/PointParticle.fxo" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs );
//		hr = D3DXCreateEffectFromResource( pD3dDevice_, NULL, _T( "IDR_POINTEFFECT" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs
// );
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

    hr = pD3dDevice_->CreateVertexDeclaration(PointParticle::aVSDataDeclarationInfo_, &pVertexDeclaration_);
    assert_succeeded(hr);
}

void PointEmitter::Uninitialize()
{
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pVertexDeclaration_);
}

//!
//! @param	dt	Amount of time elapsed since the last update

void PointEmitter::Update(float dt)
{
    PointParticle * const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (IsSorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[GetNumParticles()],
                  ParticleSorter(GetAppearance()->GetCamera()->GetPosition()));
    }
}

void PointEmitter::Draw() const
{
    HRESULT hr;

    Appearance const * pAppearance = GetAppearance();
    int nParticles = GetNumParticles();                     // Number of particles contained in this emitter

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
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable antialiasing so points less than a pixel in size are still drawn (but faded).

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

//	// Set point size
//
//	float	size	= GetAppearance()->GetSize();
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE, *(DWORD*)&size );
//	assert_succeeded( hr );
//
//	float	minSize	= 0.5f;
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSIZE_MIN, *(DWORD*)&minSize );
//	assert_succeeded( hr );
//
//	// Enable point scaling
//
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALEENABLE, TRUE );
//	assert_succeeded( hr );
//
//	// Set point scale factors
//
//	float	a	= 0.0f;
//	float	b	= 0.0f;
//	float	c	= 1.0f;	// FIXME: This should be tan( a/2 ) ** 2
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_A, *(DWORD*)&a );
//	assert_succeeded( hr );
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_B, *(DWORD*)&b );
//	assert_succeeded( hr );
//	hr = Dxx::SetRenderState( pD3dDevice_, D3DRS_POINTSCALE_C, *(DWORD*)&c );
//	assert_succeeded( hr );

    // Turn on alphablending

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

//	DirectX::XMFLOAT4X4	view			= pAppearance->GetCamera()->GetViewMatrix();
//	DirectX::XMFLOAT4X4	projection		= pAppearance->GetCamera()->GetProjectionMatrix();
//	DirectX::XMFLOAT4X4	viewProjection	= pAppearance->GetCamera()->GetViewProjectionMatrix();
//	pEffect_->SetMatrix( "ViewMatrix", &view );
//	pEffect_->SetMatrix( "ProjectionMatrix", &projection );
//	pEffect_->SetMatrix( "ViewProjectionMatrix", &view );
    pEffect_->SetFloat("Size", GetAppearance()->GetSize());

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
            PointParticle const *    paParticles = GetParticles();

            while (i < nParticles && count < particlesPerGroup)
            {
                PointParticle const * pParticle = &paParticles[i];      // Convenience

                // Draw only particles that have been born

                if (pParticle->GetAge() >= 0.0f)
                {
                    PointParticle::VBEntry * pVBEntry = &paVB[count];           // Convenience

                    pVBEntry->v[0].position = pParticle->GetPosition();
                    pVBEntry->v[0].color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
                                                                  pParticle->GetColor().g,
                                                                  pParticle->GetColor().b,
                                                                  pParticle->GetColor().a);

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
/*										S T R E A K   P A R T I C L E   E M I T T E R                               */
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//! @param	sorted	If @c true, then the particles will be sorted back to front
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

StreakEmitter::StreakEmitter(IDirect3DDevice11 *   pD3dDevice_,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(pD3dDevice_,
                   new StreakParticle[n],
                   sizeof(StreakParticle::VBEntry), StreakParticle::USAGE, (D3DPOOL)StreakParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//! @param	sorted	If @c true, then the particles will be sorted back to front
//!
//! @warning	paParticles must have been allocated with new[].

StreakEmitter::StreakEmitter(IDirect3DDevice11 *           pD3dDevice_,
                             std::auto_ptr<StreakParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(pD3dDevice_,
                   qaParticles.release(),
                   sizeof(StreakParticle::VBEntry), StreakParticle::USAGE, (D3DPOOL)StreakParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

StreakEmitter::~StreakEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

//!
//! @param	dt	Amount of time elapsed since the last update

void StreakEmitter::Update(float dt)
{
    StreakParticle *    const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (IsSorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[GetNumParticles()],
                  ParticleSorter(GetAppearance()->GetCamera()->GetPosition()));
    }
}

void StreakEmitter::Initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    StreakParticle *    const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Bind(this);
    }

    // Load the effects file

    {
        ID3DXBuffer * pErrorMsgs;
        hr = D3DXCreateEffectFromFile(pD3dDevice_, _T("../res/StreakParticle.fxo"), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( pD3dDevice_, _T( "res/StreakParticle.fxo" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs );
//		hr = D3DXCreateEffectFromResource( pD3dDevice_, NULL, _T( "IDR_STREAKEFFECT" ), NULL, NULL, 0, NULL, &pEffect_,
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

void StreakEmitter::Uninitialize()
{
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pVertexDeclaration_);
}

void StreakEmitter::Draw() const
{
    HRESULT hr;

    Appearance const * pAppearance = GetAppearance();
    int nParticles = GetNumParticles();                     // Number of particles contained in this emitter

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
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
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

    DirectX::XMFLOAT4X4 const & viewProjection = pAppearance->GetCamera()->GetViewProjectionMatrix();
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
            StreakParticle const *    paParticles = GetParticles();

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
/*									T E X T U R E D   P A R T I C L E   E M I T T E R                               */
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

TexturedEmitter::TexturedEmitter(IDirect3DDevice11 *   pD3dDevice_,
                                 EmitterVolume const * pVol,
                                 Environment const *   pEnv,
                                 Appearance const *    pApp,
                                 int                   n,
                                 bool                  sorted)
    : BasicEmitter(pD3dDevice_,
                   new TexturedParticle[n],
                   sizeof(TexturedParticle::VBEntry), TexturedParticle::USAGE, (D3DPOOL)TexturedParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

TexturedEmitter::TexturedEmitter(IDirect3DDevice11 *             pD3dDevice_,
                                 std::auto_ptr<TexturedParticle> qaParticles,
                                 EmitterVolume const *           pVol,
                                 Environment const *             pEnv,
                                 Appearance const *              pApp,
                                 int                             n,
                                 bool                            sorted)
    : BasicEmitter(pD3dDevice_,
                   qaParticles.release(),
                   sizeof(TexturedParticle::VBEntry), TexturedParticle::USAGE, (D3DPOOL)TexturedParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

TexturedEmitter::~TexturedEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

void TexturedEmitter::Initialize()
{
    HRESULT hr;

    // Point the particles' emitter value here

    TexturedParticle *  const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Bind(this);
    }

    // Figure out the maximum necessary size of the index buffer. It is the minimum of the following:
    //
    //	1. The number of particles * 6 ( 2 triangles per particle, 3 verts per triangle
    //	2. Max number of particles that the vertex buffer holds times 6. (6 indexes per particle)
    //	3. The limit specified by the hardware caps.

    int nIndexes = GetNumParticles() * 6;
    if (nIndexes > VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6)
        nIndexes = VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry) * 6;
    if (nIndexes > maxPrimitives_ * 3)
        nIndexes = maxPrimitives_ * 3;

    // Create the index buffer

    hr = pD3dDevice_->CreateIndexBuffer(nIndexes * sizeof(__int16),
                                        TexturedParticle::USAGE,
                                        D3DFMT_INDEX16,
                                        (D3DPOOL)TexturedParticle::POOL,
                                        &pIB_,
                                        NULL);
    assert_succeeded(hr);

    // Load the effects file

    {
        ID3DXBuffer * pErrorMsgs;
        hr =
            D3DXCreateEffectFromFile(pD3dDevice_, _T("../res/TexturedParticle.fxo"), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( pD3dDevice_, _T( "res/TexturedParticle.fxo" ), NULL, NULL, 0, NULL, &pEffect_, &pErrorMsgs
// );
//		hr = D3DXCreateEffectFromResource( pD3dDevice_, NULL, _T( "IDR_TEXTUREDEFFECT" ), NULL, NULL, 0, NULL, &pEffect_,
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

void TexturedEmitter::Uninitialize()
{
    Wx::SafeRelease(pVertexDeclaration_);
    Wx::SafeRelease(pEffect_);
    Wx::SafeRelease(pIB_);
}

//!
//! @param	dt	Amount of time elapsed since the last update

void TexturedEmitter::Update(float dt)
{
    TexturedParticle *  const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (IsSorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[GetNumParticles()],
                  ParticleSorter(GetAppearance()->GetCamera()->GetPosition()));
    }
}

void TexturedEmitter::Draw() const
{
    HRESULT hr;

    Appearance const *   pAppearance = GetAppearance();
    IDirect3DTexture11 * pTexture    = pAppearance->GetTexture();
    int nParticles = GetNumParticles();                         // Number of particles contained in this emitter

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
    hr = Dxx::SetRenderState(pD3dDevice_, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
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

    DirectX::XMFLOAT4X4 viewProjection = pAppearance->GetCamera()->GetViewProjectionMatrix();

    pEffect_->SetMatrix("g_viewProjection", &viewProjection);
    pEffect_->SetTexture("g_texture", pTexture);

    // Get the camera's (and thus particles') X and Y axes

    DirectX::XMFLOAT4X4 cameraOrientation = pAppearance->GetCamera()->GetFrame().GetOrientationMatrix();
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
            TexturedParticle const *    paParticles = GetParticles();

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

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

SphereEmitter::SphereEmitter(IDirect3DDevice11 *   pD3dDevice_,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(pD3dDevice_,
                   new SphereParticle[n],
                   sizeof(SphereParticle::VBEntry), SphereParticle::USAGE, (D3DPOOL)SphereParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

SphereEmitter::SphereEmitter(IDirect3DDevice11 *           pD3dDevice_,
                             std::auto_ptr<SphereParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(pD3dDevice_,
                   qaParticles.release(),
                   sizeof(SphereParticle::VBEntry), SphereParticle::USAGE, (D3DPOOL)SphereParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

SphereEmitter::~SphereEmitter()
{
    Uninitialize();
}

//!
//! @param	dt	Amount of time elapsed since the last update

void SphereEmitter::Update(float dt)
{
    SphereParticle *    const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Update(dt);
    }

    // Sort the particles by distance from the camera if desired

    if (IsSorted())
    {
        std::sort(&paParticles[0],
                  &paParticles[GetNumParticles()],
                  ParticleSorter(GetAppearance()->GetCamera()->GetPosition()));
    }
}

void SphereEmitter::Initialize()
{
    // Point the particles' emitter value here

    SphereParticle *    const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Bind(this);
    }
}

void SphereEmitter::Uninitialize()
{
    delete[] GetParticles();
}

void SphereEmitter::Draw() const
{
}
} // namespace Confetti
