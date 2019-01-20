/*********************************************************************************************************************

                                                       main.cpp

                                            Copyright 2002, John J. Bolton
    --------------------------------------------------------------------------------------------------------------

    $Header: //depot/Libraries/Confetti/Test/main.cpp#16 $

    $NoKeywords: $

*********************************************************************************************************************/

#include <tchar.h>
#include <windows.h>

#include <d3d9.h>
// #include "DxUtility/dxutil.h"
#include <stdlib.h>

#include "Confetti/Confetti.h"

#include "Dxx/Dxx.h"
#include "MyMath/Constants.h"
#include "MyMath/FastMath.h"
#include "TgaFile/TgaFile.h"
#include "Time/Clock.h"
#include "Time/FrameRateCalculator.h"
#include "Time/Timer.h"
#include "Wx/Wx.h"

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static void InitializeRendering(HWND hWnd);
static void InitializeGrid();
static void CleanUpGrid();
static void CleanUpRendering();
static void Display();
static void Reshape(HWND hWnd, int w, int h);

static bool Update(HWND hWnd);

static void InitializeParticleSystem();
static void CleanUpParticleSystem();
static void UpdateParticleSystem(float dt);
static void DisplayParticles();

static char s_AppName[]  = "Confetti";
static char s_TitleBar[] = "Confetti";

static Dxx::Camera * s_pCamera;
static float         s_CameraSpeed = 2.f;

static IDirect3D9 *        s_pD3d;
static IDirect3DDevice11 * s_pD3dDevice;
static D3DCAPS9 s_Caps;

static FrameRateCalculator * s_pFrameRateCalculator;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
    if (Wx::RegisterWindowClass(CS_OWNDC,
                                (WNDPROC)WindowProc,
                                hInstance,
                                s_AppName,
                                LoadCursor(NULL, IDC_ARROW)
        ) == NULL)
    {
        MessageBox(NULL, "Wx::RegisterWindowClass() failed.", "Error", MB_OK);
        exit(1);
    }

    OutputDebugString("***CreateWindowEx\n");
    HWND hWnd = CreateWindowEx(0,
                               s_AppName,
                               s_TitleBar,
                               WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                               0, 0, 500, 500,
                               NULL,
                               NULL,
                               hInstance,
                               NULL);

    if (hWnd)
    {
        MessageBox(NULL, "CreateWindowEx() failed.", "Error", MB_OK);
        exit(1);
    }

    OutputDebugString("***ShowWindow\n");
    ShowWindow(hWnd, nCmdShow);

    // Initialize the clock

    Clock::Instantiate();

    s_pFrameRateCalculator = new FrameRateCalculator;

    OutputDebugString("***Wx::MessageLoop\n");
    return Wx::MessageLoop(hWnd, Update);

    delete s_pFrameRateCalculator;
    Clock::Destroy();
}

static bool Update(HWND hWnd)
{
    static Timer   timer;
    static __int64 oldTime = Clock::Instance()->GetTime();

    __int64 const newTime = timer.Start();
    float const   dt      = (float)Clock::ToSeconds(newTime - oldTime);

    s_pFrameRateCalculator->Update(newTime);

    UpdateParticleSystem(dt);
//	InvalidateRect( hWnd, NULL, FALSE );
    Display();

    oldTime = newTime;

    return true;    // Call this function as often as possible
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
            OutputDebugString("***WM_CREATE\n");
            InitializeRendering(hWnd);
            InitializeParticleSystem();
            return 0;

        case WM_SIZE:
            OutputDebugString("***WM_SIZE\n");
            Reshape(hWnd, LOWORD(lParam), HIWORD(lParam));
            return 0;

//	case WM_PAINT:
//		Display();
//		ValidateRect( hWnd, NULL );
//		return 0;

        case WM_CHAR:
        {
            switch (wParam)
            {
                case VK_ESCAPE: // Quit
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                case ' ': // Forward
                    s_pCamera->Move(Dxx::Vector3ZAxis() * s_CameraSpeed);
                    break;

                case 's': // Backwards
                    s_pCamera->Move(-Dxx::Vector3ZAxis() * s_CameraSpeed);
                    break;

                case 'd': // Strafe right
                    s_pCamera->Move(Dxx::Vector3XAxis() * s_CameraSpeed);
                    break;

                case 'a': // Strafe left
                    s_pCamera->Move(-Dxx::Vector3XAxis() * s_CameraSpeed);
                    break;

                case 'w': // Strafe up
                    s_pCamera->Move(Dxx::Vector3YAxis() * s_CameraSpeed);
                    break;

                case 'x': // Strafe down
                    s_pCamera->Move(-Dxx::Vector3YAxis() * s_CameraSpeed);
                    break;
            }

            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_UP:
                    s_pCamera->Turn(-.1f, Dxx::Vector3XAxis());
                    break;

                case VK_DOWN:
                    s_pCamera->Turn(.1f, Dxx::Vector3XAxis());
                    break;

                case VK_LEFT:
                    s_pCamera->Turn(-.1f, Dxx::Vector3YAxis());
                    break;

                case VK_RIGHT:
                    s_pCamera->Turn(.1f, Dxx::Vector3YAxis());
                    break;

                case VK_PRIOR:
                    s_pCamera->Turn(-.1f, Dxx::Vector3ZAxis());
                    break;

                case VK_INSERT:
                    s_pCamera->Turn(.1f, Dxx::Vector3ZAxis());
                    break;
            }
            return 0;

        case WM_CLOSE:
            OutputDebugString("***WM_CLOSE\n");
            CleanUpParticleSystem();
            CleanUpRendering();
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            OutputDebugString("***WM_DESTROY\n");
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

struct GridVertex
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
};

static GridVertex const s_aGridVertexData[] =
{
    {{ -100.0f, -100.0f, 0.0f }, 0xff00ff00 }, {{ -100.0f,  100.0f, 0.0f }, 0xffffffff },
    {{  -90.0f, -100.0f, 0.0f }, 0xff0df200 }, {{  -90.0f,  100.0f, 0.0f }, 0xfff2f2ff },
    {{  -80.0f, -100.0f, 0.0f }, 0xff1ae600 }, {{  -80.0f,  100.0f, 0.0f }, 0xffe6e6ff },
    {{  -70.0f, -100.0f, 0.0f }, 0xff26d900 }, {{  -70.0f,  100.0f, 0.0f }, 0xffd9d9ff },
    {{  -60.0f, -100.0f, 0.0f }, 0xff33cc00 }, {{  -60.0f,  100.0f, 0.0f }, 0xffccccff },
    {{  -50.0f, -100.0f, 0.0f }, 0xff40bf00 }, {{  -50.0f,  100.0f, 0.0f }, 0xffbfbfff },
    {{  -40.0f, -100.0f, 0.0f }, 0xff4db300 }, {{  -40.0f,  100.0f, 0.0f }, 0xffb3b3ff },
    {{  -30.0f, -100.0f, 0.0f }, 0xff59a600 }, {{  -30.0f,  100.0f, 0.0f }, 0xffa6a6ff },
    {{  -20.0f, -100.0f, 0.0f }, 0xff669900 }, {{  -20.0f,  100.0f, 0.0f }, 0xff9999ff },
    {{  -10.0f, -100.0f, 0.0f }, 0xff738c00 }, {{  -10.0f,  100.0f, 0.0f }, 0xff8c8cff },
    {{    0.0f, -100.0f, 0.0f }, 0xff808000 }, {{    0.0f,  100.0f, 0.0f }, 0xff8080ff },
    {{   10.0f, -100.0f, 0.0f }, 0xff8c7300 }, {{   10.0f,  100.0f, 0.0f }, 0xff7373ff },
    {{   20.0f, -100.0f, 0.0f }, 0xff996600 }, {{   20.0f,  100.0f, 0.0f }, 0xff6666ff },
    {{   30.0f, -100.0f, 0.0f }, 0xffa65900 }, {{   30.0f,  100.0f, 0.0f }, 0xff5959ff },
    {{   40.0f, -100.0f, 0.0f }, 0xffb34d00 }, {{   40.0f,  100.0f, 0.0f }, 0xff4d4dff },
    {{   50.0f, -100.0f, 0.0f }, 0xffbf4000 }, {{   50.0f,  100.0f, 0.0f }, 0xff4040ff },
    {{   60.0f, -100.0f, 0.0f }, 0xffcc3300 }, {{   60.0f,  100.0f, 0.0f }, 0xff3333ff },
    {{   70.0f, -100.0f, 0.0f }, 0xffd92600 }, {{   70.0f,  100.0f, 0.0f }, 0xff2626ff },
    {{   80.0f, -100.0f, 0.0f }, 0xffe61a00 }, {{   80.0f,  100.0f, 0.0f }, 0xff1a1aff },
    {{   90.0f, -100.0f, 0.0f }, 0xfff20d00 }, {{   90.0f,  100.0f, 0.0f }, 0xff0d0dff },
    {{  100.0f, -100.0f, 0.0f }, 0xffff0000 }, {{  100.0f,  100.0f, 0.0f }, 0xff0000ff },

    {{ -100.0f, -100.0f, 0.0f }, 0xff00ff00 }, {{  100.0f, -100.0f, 0.0f }, 0xffff0000 },
    {{ -100.0f,  -90.0f, 0.0f }, 0xff0dff0d }, {{  100.0f,  -90.0f, 0.0f }, 0xfff2000d },
    {{ -100.0f,  -80.0f, 0.0f }, 0xff1aff1a }, {{  100.0f,  -80.0f, 0.0f }, 0xffe6001a },
    {{ -100.0f,  -70.0f, 0.0f }, 0xff26ff26 }, {{  100.0f,  -70.0f, 0.0f }, 0xffd90026 },
    {{ -100.0f,  -60.0f, 0.0f }, 0xff33ff33 }, {{  100.0f,  -60.0f, 0.0f }, 0xffcc0033 },
    {{ -100.0f,  -50.0f, 0.0f }, 0xff40ff40 }, {{  100.0f,  -50.0f, 0.0f }, 0xffbf0040 },
    {{ -100.0f,  -40.0f, 0.0f }, 0xff4dff4d }, {{  100.0f,  -40.0f, 0.0f }, 0xffb3004d },
    {{ -100.0f,  -30.0f, 0.0f }, 0xff59ff59 }, {{  100.0f,  -30.0f, 0.0f }, 0xffa60059 },
    {{ -100.0f,  -20.0f, 0.0f }, 0xff66ff66 }, {{  100.0f,  -20.0f, 0.0f }, 0xff990066 },
    {{ -100.0f,  -10.0f, 0.0f }, 0xff73ff73 }, {{  100.0f,  -10.0f, 0.0f }, 0xff8c0073 },
    {{ -100.0f,    0.0f, 0.0f }, 0xff80ff80 }, {{  100.0f,    0.0f, 0.0f }, 0xff800080 },
    {{ -100.0f,   10.0f, 0.0f }, 0xff8cff8c }, {{  100.0f,   10.0f, 0.0f }, 0xff73008c },
    {{ -100.0f,   20.0f, 0.0f }, 0xff99ff99 }, {{  100.0f,   20.0f, 0.0f }, 0xff660099 },
    {{ -100.0f,   30.0f, 0.0f }, 0xffa6ffa6 }, {{  100.0f,   30.0f, 0.0f }, 0xff5900a6 },
    {{ -100.0f,   40.0f, 0.0f }, 0xffb3ffb3 }, {{  100.0f,   40.0f, 0.0f }, 0xff4d00b3 },
    {{ -100.0f,   50.0f, 0.0f }, 0xffbfffbf }, {{  100.0f,   50.0f, 0.0f }, 0xff4000bf },
    {{ -100.0f,   60.0f, 0.0f }, 0xffccffcc }, {{  100.0f,   60.0f, 0.0f }, 0xff3300cc },
    {{ -100.0f,   70.0f, 0.0f }, 0xffd9ffd9 }, {{  100.0f,   70.0f, 0.0f }, 0xff2600d9 },
    {{ -100.0f,   80.0f, 0.0f }, 0xffe6ffe6 }, {{  100.0f,   80.0f, 0.0f }, 0xff1a00e6 },
    {{ -100.0f,   90.0f, 0.0f }, 0xfff2fff2 }, {{  100.0f,   90.0f, 0.0f }, 0xff0d00f2 },
    {{ -100.0f,  100.0f, 0.0f }, 0xffffffff }, {{  100.0f,  100.0f, 0.0f }, 0xff0000ff }
};

#define GRIDVERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

static IDirect3DVertexBuffer11 * s_pGridVB;

// static ID3DXMesh *	s_pTeapot;

static void InitializeGrid()
{
    HRESULT hr;

    // Create the vertex buffer for the grid.

    hr = Dxx::CreateStaticVertexBuffer(s_pD3dDevice, (void *)s_aGridVertexData, sizeof(s_aGridVertexData),
                                       GRIDVERTEX_FVF, &s_pGridVB);
    assert_succeeded(hr);

//	hr = D3DXCreateTeapot( s_pD3dDevice, &s_pTeapot, NULL );
//	assert_succeeded( hr );
}

static void CleanUpGrid()
{
//	Wx::SafeRelease( s_pTeapot );

    Wx::SafeRelease(s_pGridVB);
}

static ID3DXFont * s_pFont;

static void InitializeFonts(HWND hWnd)
{
    HRESULT hr;

    static D3DXFONT_DESC fontDescription =
    {
        16, 0,
        FW_BOLD,
        1,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE,
        _T("Courier New")
    };

    hr = D3DXCreateFontIndirect(s_pD3dDevice, &fontDescription, &s_pFont);
}

static void CleanupFonts()
{
    Wx::SafeRelease(s_pFont);
}

static void InitializeRendering(HWND hWnd)
{
    OutputDebugString("***InitializeRendering\n");
    HRESULT hr;

    // Create the D3D and D3DDevice objects

    s_pD3d = Direct3DCreate9(D3D_SDK_VERSION);
    hr     = Dxx::CreateD3dWindowedDevice(s_pD3d, hWnd, &s_pD3dDevice, D3DFMT_D24S8, D3DADAPTER_DEFAULT,
                                          D3DPRESENT_INTERVAL_IMMEDIATE);
//	hr = Dxx::CreateD3dWindowedDevice( s_pD3d, hWnd, &s_pD3dDevice, D3DFMT_D24S8, D3DADAPTER_DEFAULT, D3DPRESENT_INTERVAL_IMMEDIATE,
// D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING );
//	hr = Dxx::CreateD3dWindowedDevice( s_pD3d, hWnd, &s_pD3dDevice, D3DFMT_D24S8, D3DADAPTER_DEFAULT, D3DPRESENT_INTERVAL_IMMEDIATE,
// D3DDEVTYPE_REF );
    assert_succeeded(hr);

    // Query the device's caps

    s_pD3dDevice->GetDeviceCaps(&s_Caps);

    // Make sure the necssary caps are supported

    assert(s_Caps.VertexShaderVersion >= D3DVS_VERSION(2, 0));      // vs 2.0
    assert(s_Caps.PixelShaderVersion >= D3DVS_VERSION(2, 0));       // ps 2.0

    // Create fonts

    InitializeFonts(hWnd);

    // Create the camera

    s_pCamera = new Dxx::Camera(s_pD3dDevice,
                                90.0f,
                                1.0f, 1000.0f,
                                1.0f,
                                DirectX::XMFLOAT4(0.0f, 200.0f, 180.0f),
                                DirectX::XMFLOAT4((float)sin(MyMath::PI_OVER_4), 0.0f, 0.0f, (float)cos(MyMath::PI_OVER_4)));

    // Create the grid

    InitializeGrid();
}

static void CleanUpRendering()
{
    CleanUpGrid();

    delete s_pCamera;
    s_pCamera = 0;

    CleanupFonts();

    Wx::SafeRelease(s_pD3dDevice);
    Wx::SafeRelease(s_pD3d);
}

static Confetti::ParticleSystem *   s_pParticleSystem;
static Confetti::XmlConfiguration * s_pConfiguration;
static Confetti::Builder *          s_pBuilder;

static void InitializeParticleSystem()
{
    OutputDebugString("***InitializeParticleSystem\n");
    s_pConfiguration = new Confetti::XmlConfiguration("Configuration.xml");
    if (!s_pConfiguration)
        throw std::bad_alloc();

    s_pBuilder = new Confetti::Builder((uint32_t)(Clock::Instance()->GetTime() & (uint32_t)-1));
    if (!s_pBuilder)
        throw std::bad_alloc();

    s_pParticleSystem = s_pBuilder->BuildParticleSystem(*s_pConfiguration,
                                                        s_pD3dDevice,
                                                        s_pCamera).release();
    if (!s_pParticleSystem)
        throw std::bad_alloc();
}

static void CleanUpParticleSystem()
{
    delete s_pParticleSystem;   s_pParticleSystem = 0;
    delete s_pBuilder;          s_pBuilder        = 0;
    delete s_pConfiguration;    s_pConfiguration  = 0;
}

static void UpdateParticleSystem(float dt)
{
    s_pParticleSystem->Update(dt);
}

static void DisplayParticles()
{
    s_pParticleSystem->Draw();
}

static void DrawHud()
{
    char buffer [256];

    sprintf(buffer, "Frame rate: %6.2f fps (%6.2f fps average)",
            s_pFrameRateCalculator->GetAverageFrameRate(),
            s_pFrameRateCalculator->GetAverageFrameRate());

    RECT rect = { 0, 0, 320, 40 };
    s_pFont->DrawText(NULL, buffer, -1, &rect, 0, D3DCOLOR_XRGB(255, 255, 0));
}

static void DrawGrid()
{
    HRESULT hr;

    hr = Dxx::SetRenderState(s_pD3dDevice, D3DRS_LIGHTING, FALSE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(s_pD3dDevice, D3DRS_ZENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(s_pD3dDevice, D3DRS_ZWRITEENABLE, TRUE);
    assert_succeeded(hr);

    hr = Dxx::SetRenderState(s_pD3dDevice, D3DRS_ANTIALIASEDLINEENABLE, FALSE);
    assert_succeeded(hr);

//	s_pTeapot->DrawSubset( 0 );

    // D3DMATERIAL9	material	=
    // {
    //	{ .5f, .5f, .5f, 1.f },
    //	{ .5f, .5f, .5f, 1.f },
    //	{ .5f, .5f, .5f, 1.f },
    //	{ .5f, .5f, .5f, 1.f },
    //	0.f
    // };

    // hr = s_pD3dDevice->SetMaterial( &material );
    // assert_succeeded( hr );

    hr = s_pD3dDevice->SetStreamSource(0, s_pGridVB, 0, sizeof(s_aGridVertexData[0]));
    assert_succeeded(hr);

    hr = s_pD3dDevice->SetFVF(GRIDVERTEX_FVF);
    assert_succeeded(hr);

    hr = s_pD3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, elementsof(s_aGridVertexData) / 2);
    assert_succeeded(hr);
}

static void Display()
{
    HRESULT hr;

    // Clear the viewport

    hr = s_pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(128, 192, 255), 1.0f, 0);
    assert_succeeded(hr);

    // Begin the scene

    hr = s_pD3dDevice->BeginScene();
    assert_succeeded(hr);

    // Place the camera

    s_pCamera->Look();

    // Lights

//	s_pDirectionalLight->Apply();

    // Draw grid

    DrawGrid();

    // Draw the particles

    DisplayParticles();

    // Draw the hud

    DrawHud();

    hr = s_pD3dDevice->EndScene();
    assert_succeeded(hr);

    hr = s_pD3dDevice->Present(NULL, NULL, NULL, NULL);
    assert_succeeded(hr);
}

static void Reshape(HWND hWnd, int w, int h)
{
//	HRESULT		hr;
//
//	OutputDebugString( "***ResetD3dWindowedDevice\n" );
//	hr = Dxx::ResetD3dWindowedDevice( s_pD3dDevice, D3DFMT_D24S8 );
//	assert_succeeded( hr );

    OutputDebugString("***s_pCamera->Reshape\n");
    s_pCamera->SetAspectRatio(float(w), float(h));
    s_pCamera->Reshape();
}
