#include <windows.h>
#include <xnamath.h>
#include"D3DClass.h"
#include "BlockClass.h"
#include "CameraClass.h"
#include "InputClass.h"
#include "resource.h"
#include "FileTracker.h"
#include "WindowClass.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include"RectangleClass.h"
#include"FrameBuffer.h"
#include"MazeGenerator.h"
#include"VerticalBlur.hpp"
#include"HorizontalBlur.hpp"
#include"PrimitivePipeline.h"
#include"VHBlurClass.hpp"

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

RectangleClass rec;
CameraClass camera;
BlockClass bk;
D3DClass d3d;
HorizontalBlur blur;

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	AllocConsole();
	FILE * m_new_stdout_file;
	freopen_s(&m_new_stdout_file, "CONOUT$", "w+t", stdout);
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"aMazingWndClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"aMazingWndClass", L"aMazing by AntiMoron anti2moron@gmail.com", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}

HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

	WindowClass::getInstance().setWidth(width);
	WindowClass::getInstance().setHeight(height);
	//Initialize all the thing we need to prepare for rendering work.
	d3d.Initialize(g_hWnd);
	
	// Load the Texture
	hr = TEXTURE.addTexture(d3d.getDevice(), d3d.getContext(), L"seafloor.dds");
	if (FAILED(hr))
		return E_FAIL;

	hr = TEXTURE.addTexture(d3d.getDevice(), d3d.getContext(), L"glowstone.png");
	if (FAILED(hr))
		return E_FAIL;

	hr = TEXTURE.addTexture(d3d.getDevice(), d3d.getContext(),
		L"leaves.png");
	if (FAILED(hr))
		return E_FAIL;

	//aditional operations.
	bk.Initialize(d3d.getDevice(), d3d.getContext());
	camera.Initialize(d3d.getDevice(), d3d.getContext());
	rec.Initialize(d3d.getDevice(), d3d.getContext());
	blur.Initialize(d3d.getDevice(), d3d.getContext());
	GRAPHICS.Initialize(&d3d);
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	SHADERS.addPair(d3d.getDevice(), d3d.getContext(),
		"Basic3D.fx", "Basic3D.fx",
		layout, numElements, "Basic3D");

	SHADERS.addPair(d3d.getDevice(), d3d.getContext(),
		"Basic2D.fx", "Basic2D.fx",
		layout, numElements, "Basic2D");

	SHADERS.addPair(d3d.getDevice(), d3d.getContext(),
		"Shader/hblur/hblur.fx", "Shader/hblur/hblur.fx",
		layout, numElements, "Hblur");

	SHADERS.addPair(d3d.getDevice(), d3d.getContext(),
		"Shader/vblur/vblur.fx", "Shader/vblur/vblur.fx",
		layout, numElements, "Vblur");
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	blur.Shutdown();
	d3d.Shutdown();
	bk.Shutdown();
	camera.Shutdown();
	rec.Shutdown();
	GRAPHICS.Shutdown();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	UINT width; 
	UINT height;
    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

		case WM_KEYDOWN:
			INPUT.keys[wParam] = true;
			break;

		case WM_KEYUP:
			INPUT.keys[wParam] = false;
			break;

		case WM_LBUTTONDOWN:
			INPUT.keys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			INPUT.keys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			INPUT.keys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			INPUT.keys[VK_RBUTTON] = false;
			break;
		case WM_MOUSEMOVE:
			INPUT.mouse_x = LOWORD(lParam);
			INPUT.mouse_y = HIWORD(lParam);
			break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_SIZE:
			//Update window size
			GetClientRect(g_hWnd, &rc);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;

			WindowClass::getInstance().setWidth(width);
			WindowClass::getInstance().setHeight(height);
			break;
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

Maze* mz = MAZEFACTORY.genMaze(10);


void Render()
{
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
    d3d.getContext()->ClearRenderTargetView( d3d.getRenderTargetView(), ClearColor );
	d3d.getContext()->ClearDepthStencilView(d3d.getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (INPUT.keys['W'])
	{
		camera.moveForward(0.1f);
	}
	if (INPUT.keys['S'])
	{
		camera.moveBackward(0.1f);
	}
	if (INPUT.keys['A'])
	{
		camera.moveLeft(0.1f);
	}
	if (INPUT.keys['D'])
	{
		camera.moveRight(0.1f);
	}
	if (INPUT.keys[VK_LEFT])
	{
		camera.turnLeft(1);
	}
	if (INPUT.keys[VK_RIGHT])
	{
		camera.turnRight(1);
	}
	if (INPUT.keys[VK_UP])
	{
		camera.lookUp(1);
	}
	if (INPUT.keys[VK_DOWN])
	{
		camera.lookDown(1);
	}
	camera.Render(d3d.getDevice(), d3d.getContext());
	
	auto renderFunction = [&](ID3D11Device* device,
		ID3D11DeviceContext* context)->void{
		SHADERS.getPair("Basic3D").bindShader(device, context);
		TEXTURE.getTexture(1)->bindPS(device, context, 0);
		mz->Render(device, context);
	};
	
	blur.Render(d3d.getDevice(),
		d3d.getContext(),
		d3d.getDepthStencilView(),
		renderFunction);

	d3d.setRenderTarget();
	blur.bindPS(d3d.getDevice(), d3d.getContext(),0);
	SHADERS.getPair("Basic2D").bindShader(d3d.getDevice(), d3d.getContext());
	GRAPHICS.RenderRectangle(0, 0, WINWIDTH, WINHEIGHT);
	d3d.Present(true);//V-Sync
}
