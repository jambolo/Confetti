/** @file *//********************************************************************************************************

                                                     Emitter.cpp

                                            Copyright 2003, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Emitter.cpp#19 $

    $NoKeywords: $

********************************************************************************************************************/

#include "PrecompiledHeaders.h"

#include "Emitter.h"

#include "Appearance.h"
#include "Particle.h"
#include "StreakParticle.h"
#include "TexturedParticle.h"

#include "resource.h"

// Define the following symbol if the particles are sorted loosely instead of strictly (loosely is faster)
#define LOOSELY_SORTED

// Vertex buffer size
static int const VERTEX_BUFFER_SIZE = 1024 * 1024;

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

// This class sorts particles back-to-front.

class ParticleSorter
{
public:
    ParticleSorter(D3DXVECTOR3 const & cameraPosition)
        : m_CameraPosition(cameraPosition)
    {
    }

    bool operator ()(Confetti::Particle const & a, Confetti::Particle const & b)
    {
        D3DXVECTOR3 da = a.GetPosition() - m_CameraPosition;
        D3DXVECTOR3 db = b.GetPosition() - m_CameraPosition;

        float da2 = D3DXVec3LengthSq(&da);
        float db2 = D3DXVec3LengthSq(&db);

#if defined(LOOSELY_SORTED)

        int ea;
        int eb;
        frexp(da2, &ea);
        frexp(db2, &eb);

        return ea > eb;

#else   // defined( LOOSELY_SORTED )

        return da2 > db2;

#endif  // defined( LOOSELY_SORTED )
    }

    D3DXVECTOR3 const m_CameraPosition;
};

namespace Confetti
{
/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	paParticles		Array of particles (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles in the array.
//! @param	spread			Emission angle (actual meaning is specific to the type of emitter).
//!
//! @warning	paParticles must have been allocated with new[].

BasicEmitter::BasicEmitter(IDirect3DDevice9 * pD3dDevice,
                           Particle * paParticles,
                           int size, uint32 usage, D3DPOOL pool,
                           EmitterVolume const * pVol,
                           Environment const * pEnv,
                           Appearance const * pApp,
                           int n,
                           bool sorted)
    : m_paParticles(paParticles),
    m_pEmitterVolume(pVol),
    m_pEnvironment(pEnv),
    m_pAppearance(pApp),
    m_NumParticles(n),
    m_Sorted(sorted),
    m_Enabled(true),
    m_Position(Dxx::Vector3Origin()),
    m_Velocity(Dxx::Vector3Origin()),
    m_pD3dDevice(pD3dDevice),
    m_pVB(0)
{
    HRESULT hr;

    pD3dDevice->AddRef();

    // Check caps bits for certain features

    {
        D3DCAPS9 caps;

        m_pD3dDevice->GetDeviceCaps(&caps);

        m_AlphaTestAvailable = ((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0);
        m_MaxPrimitives      = caps.MaxPrimitiveCount;
        m_MaxVertexIndex     = caps.MaxVertexIndex;
    }

    // Create the vertex buffer

    hr = pD3dDevice->CreateVertexBuffer(std::min(VERTEX_BUFFER_SIZE, n * size), usage, 0, pool, &m_pVB, NULL);
    assert_succeeded(hr);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

BasicEmitter::~BasicEmitter()
{
    Wx::SafeRelease(m_pVB);
    Wx::SafeRelease(m_pD3dDevice);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	dt	Amount of time elapsed since the last update
//! @param	position	The new position of the emitter.
//! @param	velocity	The new velocity of the emitter.

void BasicEmitter::Update(float dt, D3DXVECTOR3 const & position, D3DXVECTOR3 const & velocity)
{
    // Update the emitter's position and velocity

    Update(position, velocity);

    // Update the emitter's particles

    Update(dt);
}

/********************************************************************************************************************/
/********************************************************************************************************************/
/*																													*/
/*										P O I N T   P A R T I C L E   E M I T T E R                                 */
/*																													*/
/********************************************************************************************************************/
/********************************************************************************************************************/

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

PointEmitter::PointEmitter(IDirect3DDevice9 *    m_pD3dDevice,
                           EmitterVolume const * pVol,
                           Environment const *   pEnv,
                           Appearance const *    pApp,
                           int                   n,
                           bool                  sorted)
    : BasicEmitter(m_pD3dDevice,
                   new PointParticle[n],
                   sizeof(PointParticle::VBEntry), PointParticle::USAGE, (D3DPOOL)PointParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

PointEmitter::PointEmitter(IDirect3DDevice9 *           m_pD3dDevice,
                           std::auto_ptr<PointParticle> qaParticles,
                           EmitterVolume const *        pVol,
                           Environment const *          pEnv,
                           Appearance const *           pApp,
                           int                          n,
                           bool                         sorted)
    : BasicEmitter(m_pD3dDevice,
                   qaParticles.release(),
                   sizeof(PointParticle::VBEntry), PointParticle::USAGE, (D3DPOOL)PointParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

PointEmitter::~PointEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
        ID3DXBuffer * pErrorMsgs;
        hr = D3DXCreateEffectFromFile(m_pD3dDevice, _T("../res/PointParticle.fxo"), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( m_pD3dDevice, _T( "res/PointParticle.fxo" ), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs );
//		hr = D3DXCreateEffectFromResource( m_pD3dDevice, NULL, _T( "IDR_POINTEFFECT" ), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs
// );
        if (FAILED(hr) && pErrorMsgs != 0 && pErrorMsgs->GetBufferPointer() != 0)
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);

        D3DXHANDLE hTechnique;
        hr = m_pEffect->FindNextValidTechnique(NULL, &hTechnique);
        assert_succeeded(hr);

        m_pEffect->SetTechnique(hTechnique);

        Wx::SafeRelease(pErrorMsgs);
    }

    hr = m_pD3dDevice->CreateVertexDeclaration(PointParticle::m_aVSDataDeclarationInfo, &m_pVertexDeclaration);
    assert_succeeded(hr);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void PointEmitter::Uninitialize()
{
    Wx::SafeRelease(m_pEffect);
    Wx::SafeRelease(m_pVertexDeclaration);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable antialiasing so points less than a pixel in size are still drawn (but faded).

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

//	// Set point size
//
//	float	size	= GetAppearance()->GetSize();
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSIZE, *(DWORD*)&size );
//	assert_succeeded( hr );
//
//	float	minSize	= 0.5f;
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSIZE_MIN, *(DWORD*)&minSize );
//	assert_succeeded( hr );
//
//	// Enable point scaling
//
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSCALEENABLE, TRUE );
//	assert_succeeded( hr );
//
//	// Set point scale factors
//
//	float	a	= 0.0f;
//	float	b	= 0.0f;
//	float	c	= 1.0f;	// FIXME: This should be tan( a/2 ) ** 2
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSCALE_A, *(DWORD*)&a );
//	assert_succeeded( hr );
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSCALE_B, *(DWORD*)&b );
//	assert_succeeded( hr );
//	hr = Dxx::SetRenderState( m_pD3dDevice, D3DRS_POINTSCALE_C, *(DWORD*)&c );
//	assert_succeeded( hr );

    // Turn on alphablending

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

//	D3DXMATRIX	view			= pAppearance->GetCamera()->GetViewMatrix();
//	D3DXMATRIX	projection		= pAppearance->GetCamera()->GetProjectionMatrix();
//	D3DXMATRIX	viewProjection	= pAppearance->GetCamera()->GetViewProjectionMatrix();
//	m_pEffect->SetMatrix( "ViewMatrix", &view );
//	m_pEffect->SetMatrix( "ProjectionMatrix", &projection );
//	m_pEffect->SetMatrix( "ViewProjectionMatrix", &view );
    m_pEffect->SetFloat("Size", GetAppearance()->GetSize());

    // Set up vertex data

    hr = m_pD3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
    assert_succeeded(hr);

    hr = m_pD3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(PointParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    int particlesPerGroup = std::min(std::min(VERTEX_BUFFER_SIZE / (int)sizeof(PointParticle::VBEntry), m_MaxPrimitives),
                                     nParticles);

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;                      // Number of particles to be rendered this block

        {
            Dxx::VertexBufferLock lock(m_pVB, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

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

            m_pEffect->Begin(&nPasses, 0);
            assert(nPasses == 1);

            m_pEffect->BeginPass(0);
            m_pEffect->CommitChanges();
            m_pD3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, count);
            m_pEffect->EndPass();

            m_pEffect->End();
        }
    }
}

/********************************************************************************************************************/
/********************************************************************************************************************/
/*																													*/
/*										S T R E A K   P A R T I C L E   E M I T T E R                               */
/*																													*/
/********************************************************************************************************************/
/********************************************************************************************************************/

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//! @param	sorted	If @c true, then the particles will be sorted back to front
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

StreakEmitter::StreakEmitter(IDirect3DDevice9 *    m_pD3dDevice,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(m_pD3dDevice,
                   new StreakParticle[n],
                   sizeof(StreakParticle::VBEntry), StreakParticle::USAGE, (D3DPOOL)StreakParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//! @param	sorted	If @c true, then the particles will be sorted back to front
//!
//! @warning	paParticles must have been allocated with new[].

StreakEmitter::StreakEmitter(IDirect3DDevice9 *            m_pD3dDevice,
                             std::auto_ptr<StreakParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(m_pD3dDevice,
                   qaParticles.release(),
                   sizeof(StreakParticle::VBEntry), StreakParticle::USAGE, (D3DPOOL)StreakParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

StreakEmitter::~StreakEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
        hr = D3DXCreateEffectFromFile(m_pD3dDevice, _T("../res/StreakParticle.fxo"), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( m_pD3dDevice, _T( "res/StreakParticle.fxo" ), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs );
//		hr = D3DXCreateEffectFromResource( m_pD3dDevice, NULL, _T( "IDR_STREAKEFFECT" ), NULL, NULL, 0, NULL, &m_pEffect,
// &pErrorMsgs );
        if (FAILED(hr) && pErrorMsgs != 0 && pErrorMsgs->GetBufferPointer() != 0)
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);

        D3DXHANDLE hTechnique;
        hr = m_pEffect->FindNextValidTechnique(NULL, &hTechnique);
        assert_succeeded(hr);

        m_pEffect->SetTechnique(hTechnique);

        Wx::SafeRelease(pErrorMsgs);
    }

    hr = m_pD3dDevice->CreateVertexDeclaration(StreakParticle::m_aVSDataDeclarationInfo, &m_pVertexDeclaration);
    assert_succeeded(hr);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void StreakEmitter::Uninitialize()
{
    Wx::SafeRelease(m_pEffect);
    Wx::SafeRelease(m_pVertexDeclaration);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Enable line antialiasing.

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Set effect variables

    D3DXMATRIX const & viewProjection = pAppearance->GetCamera()->GetViewProjectionMatrix();
    m_pEffect->SetMatrix("ViewProjectionMatrix", &viewProjection);

    // Set up vertex data

    hr = m_pD3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
    assert_succeeded(hr);

    hr = m_pD3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(StreakParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    // Render

    int particlesPerGroup = std::min(std::min(VERTEX_BUFFER_SIZE / (int)sizeof(StreakParticle::VBEntry),
                                              m_MaxPrimitives),
                                     nParticles);

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;

        {
            Dxx::VertexBufferLock lock(m_pVB, 0, particlesPerGroup * sizeof(StreakParticle::VBEntry), D3DLOCK_DISCARD);

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

            m_pEffect->Begin(&nPasses, 0);
            assert(nPasses == 1);

            m_pEffect->BeginPass(0);
            m_pEffect->CommitChanges();
            m_pD3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, count);
            m_pEffect->EndPass();

            m_pEffect->End();
        }
    }
}

/********************************************************************************************************************/
/********************************************************************************************************************/
/*																													*/
/*									T E X T U R E D   P A R T I C L E   E M I T T E R                               */
/*																													*/
/********************************************************************************************************************/
/********************************************************************************************************************/

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

TexturedEmitter::TexturedEmitter(IDirect3DDevice9 *    m_pD3dDevice,
                                 EmitterVolume const * pVol,
                                 Environment const *   pEnv,
                                 Appearance const *    pApp,
                                 int                   n,
                                 bool                  sorted)
    : BasicEmitter(m_pD3dDevice,
                   new TexturedParticle[n],
                   sizeof(TexturedParticle::VBEntry), TexturedParticle::USAGE, (D3DPOOL)TexturedParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

TexturedEmitter::TexturedEmitter(IDirect3DDevice9 *              m_pD3dDevice,
                                 std::auto_ptr<TexturedParticle> qaParticles,
                                 EmitterVolume const *           pVol,
                                 Environment const *             pEnv,
                                 Appearance const *              pApp,
                                 int                             n,
                                 bool                            sorted)
    : BasicEmitter(m_pD3dDevice,
                   qaParticles.release(),
                   sizeof(TexturedParticle::VBEntry), TexturedParticle::USAGE, (D3DPOOL)TexturedParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

TexturedEmitter::~TexturedEmitter()
{
    Uninitialize();
    delete[] GetParticles();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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
    if (nIndexes > m_MaxPrimitives * 3)
        nIndexes = m_MaxPrimitives * 3;

    // Create the index buffer

    hr = m_pD3dDevice->CreateIndexBuffer(nIndexes * sizeof(__int16),
                                         TexturedParticle::USAGE,
                                         D3DFMT_INDEX16,
                                         (D3DPOOL)TexturedParticle::POOL,
                                         &m_pIB,
                                         NULL);
    assert_succeeded(hr);

    // Load the effects file

    {
        ID3DXBuffer * pErrorMsgs;
        hr =
            D3DXCreateEffectFromFile(m_pD3dDevice, _T("../res/TexturedParticle.fxo"), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs);
//		hr = D3DXCreateEffectFromFile( m_pD3dDevice, _T( "res/TexturedParticle.fxo" ), NULL, NULL, 0, NULL, &m_pEffect, &pErrorMsgs
// );
//		hr = D3DXCreateEffectFromResource( m_pD3dDevice, NULL, _T( "IDR_TEXTUREDEFFECT" ), NULL, NULL, 0, NULL, &m_pEffect,
// &pErrorMsgs );
        if (FAILED(hr) && pErrorMsgs != 0 && pErrorMsgs->GetBufferPointer() != 0)
        {
#if defined(_DEBUG)
            OutputDebugString((char *)pErrorMsgs->GetBufferPointer());
#endif          // defined( _DEBUG )
        }
        assert_succeeded(hr);

        D3DXHANDLE hTechnique;
        hr = m_pEffect->FindNextValidTechnique(NULL, &hTechnique);
        assert_succeeded(hr);

        m_pEffect->SetTechnique(hTechnique);

        Wx::SafeRelease(pErrorMsgs);
    }

    hr = m_pD3dDevice->CreateVertexDeclaration(TexturedParticle::m_aVSDataDeclarationInfo, &m_pVertexDeclaration);
    assert_succeeded(hr);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void TexturedEmitter::Uninitialize()
{
    Wx::SafeRelease(m_pVertexDeclaration);
    Wx::SafeRelease(m_pEffect);
    Wx::SafeRelease(m_pIB);
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void TexturedEmitter::Draw() const
{
    HRESULT hr;

    Appearance const *  pAppearance = GetAppearance();
    IDirect3DTexture9 * pTexture    = pAppearance->GetTexture();
    int nParticles = GetNumParticles();                         // Number of particles contained in this emitter

    // Test the Z-buffer, and write to it if the particles are sorted or don't write to it if they aren't. Particles
    // obscured by previously rendered objects will not be drawn because the Z-test is enabled. If Z-write is
    // disabled (because particles are not sorted), then everything drawn after a particle (including other
    // particles) will be drawn on top of it (regardless of Z). In this case, particles should be drawn last. The
    // purpose is to prevent artifacts caused by a nearer particle preventing a farther particle from being drawn
    // when the nearer particle is transparent. If the particles are sorted back-to-front, then this problem does
    // not occur and writing to the Z-buffer is enabled so that particles do not have to treated as a special case.

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

#if defined(LOOSELY_SORTED)
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, FALSE);
#else   // defined( LOOSELY_SORTED )
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ZWRITEENABLE, IsSorted() ? TRUE : FALSE);
#endif  // defined( LOOSELY_SORTED )
    assert_succeeded(hr);

    // No back-face culling

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_CULLMODE, D3DCULL_NONE);
    assert_succeeded(hr);

    // No lighting

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    // Turn on alphablending

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    assert_succeeded(hr);

    // Enable alpha testing (skips pixels with less than a certain alpha). This helps with unsorted particles since
    // the Z-buffer is not set for nearly-transparent pixels and particles drawn behind them will not be rejected.
    // Also, since the particles are triangles, there are large areas of transparent pixels, this will speed up
    // rendering by rejecting those pixels.

    if (m_AlphaTestAvailable)
    {
        m_pD3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        m_pD3dDevice->SetRenderState(D3DRS_ALPHAREF, 8);
        m_pD3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    }

    // ???
    hr = Dxx::SetRenderState(m_pD3dDevice, D3DRS_DITHERENABLE, TRUE);
    assert_succeeded(hr);

    // Set effect variables

    D3DXMATRIX viewProjection = pAppearance->GetCamera()->GetViewProjectionMatrix();

    m_pEffect->SetMatrix("g_viewProjection", &viewProjection);
    m_pEffect->SetTexture("g_texture", pTexture);

    // Get the camera's (and thus particles') X and Y axes

    D3DXMATRIX cameraOrientation = pAppearance->GetCamera()->GetFrame().GetOrientationMatrix();
    m_pEffect->SetMatrix("g_cameraOrientation", &cameraOrientation);

    // Set up vertex data

    hr = m_pD3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
    assert_succeeded(hr);

    hr = m_pD3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(TexturedParticle::VBEntry::Vertex));
    assert_succeeded(hr);

    hr = m_pD3dDevice->SetIndices(m_pIB);
    assert_succeeded(hr);

    // Render

    int particlesPerGroup = nParticles;
    if (particlesPerGroup > VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry))
        particlesPerGroup = VERTEX_BUFFER_SIZE / sizeof(TexturedParticle::VBEntry);
    if (particlesPerGroup * TexturedParticle::VBEntry::NUM_VERTICES - 1 > m_MaxVertexIndex)
        particlesPerGroup = (m_MaxVertexIndex + 1) / TexturedParticle::VBEntry::NUM_VERTICES;
    if (particlesPerGroup > m_MaxPrimitives)
        particlesPerGroup = m_MaxPrimitives;

    int i = 0;
    while (i < nParticles)
    {
        int count = 0;                      // Number of particles to be rendered this block
        unsigned __int16 baseIndex = 0;         //

        {
            Dxx::VertexBufferLock vbLock(m_pVB, 0, particlesPerGroup * sizeof(TexturedParticle::VBEntry), D3DLOCK_DISCARD);
            Dxx::IndexBufferLock  ibLock(m_pIB, 0, particlesPerGroup * 6 * sizeof(__int16), D3DLOCK_DISCARD);

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

                    float       radius   = pParticle->GetRadius();
                    D3DXVECTOR3 position = pParticle->GetPosition();
                    float       rotation = pParticle->GetRotation();
                    D3DCOLOR    color    = D3DCOLOR_COLORVALUE(pParticle->GetColor().r,
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

            m_pEffect->Begin(&nPasses, 0);
            assert(nPasses == 1);

            m_pEffect->BeginPass(0);
            m_pEffect->CommitChanges();
            m_pD3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                                               0,
                                               0,
                                               count * TexturedParticle::VBEntry::NUM_VERTICES,
                                               0,
                                               count * 2);
            m_pEffect->EndPass();
            m_pEffect->End();
        }
    }
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	pVol	Emitter volume.
//! @param	pEnv	Environment.
//! @param	pApp	Appearance.
//!	@param	n		Number of particles to create.
//!
//! @warning std::bad_alloc is thown if memory is unable to be allocated for the particles.

SphereEmitter::SphereEmitter(IDirect3DDevice9 *    m_pD3dDevice,
                             EmitterVolume const * pVol,
                             Environment const *   pEnv,
                             Appearance const *    pApp,
                             int                   n,
                             bool                  sorted)
    : BasicEmitter(m_pD3dDevice,
                   new SphereParticle[n],
                   sizeof(SphereParticle::VBEntry), SphereParticle::USAGE, (D3DPOOL)SphereParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    if (!GetParticles())
        throw std::bad_alloc();

    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

//! @param	paParticles		Particle array (the emitter assumes ownership).
//! @param	pVol			Emitter volume.
//! @param	pEnv			Environment applied to all particles.
//! @param	pApp			Appearance shared by all particles.
//!	@param	n				Number of particles to create.
//!
//! @warning	paParticles must have been allocated with new[].

SphereEmitter::SphereEmitter(IDirect3DDevice9 *            m_pD3dDevice,
                             std::auto_ptr<SphereParticle> qaParticles,
                             EmitterVolume const *         pVol,
                             Environment const *           pEnv,
                             Appearance const *            pApp,
                             int                           n,
                             bool                          sorted)
    : BasicEmitter(m_pD3dDevice,
                   qaParticles.release(),
                   sizeof(SphereParticle::VBEntry), SphereParticle::USAGE, (D3DPOOL)SphereParticle::POOL,
                   pVol, pEnv, pApp, n, sorted)
{
    Initialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

SphereEmitter::~SphereEmitter()
{
    Uninitialize();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

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

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void SphereEmitter::Initialize()
{
    // Point the particles' emitter value here

    SphereParticle *    const paParticles = GetParticles();

    for (int i = 0; i < GetNumParticles(); i++)
    {
        paParticles[i].Bind(this);
    }
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void SphereEmitter::Uninitialize()
{
    delete[] GetParticles();
}

/********************************************************************************************************************/
/*																													*/
/********************************************************************************************************************/

void SphereEmitter::Draw() const
{
}
} // namespace Confetti
