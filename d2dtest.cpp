//==================================================
//	D2D Test
//==================================================
#include <windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <stdio.h>
#include <wincodec.h>
#include <d2d1helper.h>

#define WIN32_DEFAULT_LIBS 
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
//==================================================
//int render_type=0;
int render_type=1;
//==================================================
//
HINSTANCE g_hInst=NULL;
HWND m_hWnd=NULL;
//
IDXGISwapChain *m_DXGISwapChain=NULL;
IDXGISwapChain1 *m_DXGISwapChain1=NULL;
ID3D11Device *m_D3DDevice=NULL;
ID3D11Device1 *m_D3DDevice1=NULL;
ID3D11DeviceContext *m_D3DDeviceContext=NULL;
ID3D11DeviceContext1 *m_D3DDeviceContext1=NULL;
ID3D11RenderTargetView *m_D3DRenderTargetView=NULL;
IDXGISurface *m_DXGISurface=NULL;
IDXGIDevice *m_DXGIDevice=NULL;
//
//ID2D1Factory *m_D2DFactory=NULL;
ID2D1Factory4 *m_D2DFactory=NULL;
//ID2D1Factory1 *m_D2DFactory1=NULL;
//ID2D1Factory2 *m_D2DFactory2=NULL;
//ID2D1Factory3 *m_D2DFactory3=NULL;
//ID2D1Factory4 *m_D2DFactory4=NULL;


//ID2D1SolidColorBrush *m_pLightSlateGrayBrush=NULL;

ID2D1SpriteBatch *m_spriteBatch=NULL;
ID2D1Bitmap *m_bitmap=NULL;

//ID2D1Device  *m_D2DDevice=NULL;
ID2D1Device3 *m_D2DDevice=NULL;
//ID2D1DeviceContext *m_D2DDeviceContext=NULL;
ID2D1DeviceContext3 *m_D2DDeviceContext3=NULL;

ID2D1RenderTarget *m_D2DRenderTarget=NULL;
//ID2D1RenderTarget *m_pRenderTarget=NULL;
ID2D1HwndRenderTarget *m_pRenderTarget=NULL;
UINT g_width;
UINT g_height;

IWICImagingFactory *m_IWICImagingFactory=NULL;
//==================================================

HRESULT Init_WICImagingFactory();
HRESULT InitializeWindow(HINSTANCE hInstance);
HRESULT Init_DXGI(HWND hWnd);
HRESULT CreateD3D11Device(HWND hWnd);
HRESULT InitializeD3D11(HWND hWnd);
HRESULT InitializeD2D_UnUseDX11(HWND hWnd);
HRESULT InitializeD2D_UseDX11(HWND hWnd);
HRESULT Initialize_Type1(HWND hWnd);
HRESULT Initialize_Type2(HWND hWnd);
ID2D1Bitmap *LoadBitmapFromFile(ID2D1RenderTarget *pRenderTarget,PCWSTR uri);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RunMessageLoop();
void OnRenderer();
void OnRenderer1(ID2D1RenderTarget *RenderTarget);
void OnRenderer2(ID2D1RenderTarget *RenderTarget);
void InitSpriteBatch(ID2D1SpriteBatch *SpriteBatch);

//==================================================
const char *log_filename="log.txt";
FILE *log_hd=NULL;

static void OpenLog(){
	if(log_hd)return;
	log_hd = fopen(log_filename, "w");
}
static void CloseLog(){
	if(log_hd){
		fclose(log_hd );
		log_hd=NULL;
	}
}
static void log(const char *m,...){
	va_list list;
	va_start(list,m);
	if(!log_hd){
		OpenLog();
	}
	vfprintf(log_hd ,m,list);
}
//==================================================
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow){
//    return MessageBox(NULL, "hello, world", "caption", 0);
	log("---- log start ---\n");
	g_hInst=hInst;
	HRESULT hr=0;
	
	// COM初期化
	CoInitialize(NULL);
	
	hr=Init_WICImagingFactory();
	if(FAILED(hr)){
		log("Init_WICImagingFactory Error !!\n");
		return 0;
	}
	
	hr=InitializeWindow(hInst);
	if(SUCCEEDED(hr)){
		if(render_type==0){
			hr=Initialize_Type1(m_hWnd);
		}else{
			hr=Initialize_Type2(m_hWnd);
		}
		if(SUCCEEDED(hr)){
			RunMessageLoop();
		}
	}
	return 0;
}

HRESULT Initialize_Type1(HWND hWnd){
	log("Initialize_Type1\n");
	HRESULT hr=0;
//	hr=CreateD3D11Device(hWnd);
//	if(SUCCEEDED(hr)){
		hr=InitializeD2D_UnUseDX11(hWnd);
//	}
	/*
	hr=Init_DXGI(m_hWnd);
	if(SUCCEEDED(hr)){
		hr=InitializeD2D_UnUseDX11(m_hWnd);
		if(SUCCEEDED(hr)){
			return hr;
		}
	}
	*/
	return hr;
}
HRESULT Initialize_Type2(HWND hWnd){
	log("Initialize_Type2\n");
	HRESULT hr=0;
	hr=InitializeD3D11(hWnd);
	if(SUCCEEDED(hr)){
//		hr=InitializeD2D_UseDX11(m_hWnd);
//		if(SUCCEEDED(hr)){
//			PCWSTR filename=L"check.bmp";
//			m_bitmap=LoadBitmapFromFile(m_D2DRenderTarget,filename);
//		}
	}
	return hr;
}

//==================================================
void RunMessageLoop(){
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//==================================================
//Windowの作成
HRESULT InitializeWindow(HINSTANCE hInst){
	HRESULT hr=0;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
//	hr = CreateDeviceIndependentResources();

//	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style		 = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = WndProc;
		wcex.cbClsExtra	= 0;
		wcex.cbWndExtra	= sizeof(LONG_PTR);
		wcex.hInstance	 = hInst;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName  = NULL;
		wcex.hCursor	   = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = TEXT("D2DDemoApp");

		RegisterClassEx(&wcex);

		// In terms of using the correct DPI, to create a window at a specific size
		// like this, the procedure is to first create the window hidden. Then we get
		// the actual DPI from the HWND (which will be assigned by whichever monitor
		// the window is created on). Then we use SetWindowPos to resize it to the
		// correct DPI-scaled size, then we use ShowWindow to show it.

		m_hWnd = CreateWindow(
			TEXT("D2DDemoApp"),
			TEXT("Direct2D demo application"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			640,
			480,
			NULL,
			NULL,
			hInst,
			NULL);

		if (m_hWnd ){
			// Because the SetWindowPos function takes its size in pixels, we
			// obtain the window's DPI, and use it to scale the window size.
			float dpi = GetDpiForWindow(m_hWnd);
			/*
			SetWindowPos(
				m_hWnd ,
				NULL,
				NULL,
				NULL,
				static_cast<int>(ceil(640.f * dpi / 96.f)),
				static_cast<int>(ceil(480.f * dpi / 96.f)),
				SWP_NOMOVE);
				*/
			ShowWindow(m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_hWnd);
		}
	}
	log("hWnd(%p)\n",m_hWnd);
	return hr;
}

//==================================================
//DX11のDeivceとDeviceContextの作成
HRESULT CreateD3D11Device(HWND hWnd){
	log("CreateD3D11Device\n");
	HRESULT hr=0;
	RECT rc;
	GetClientRect(hWnd, &rc);
	g_width  = rc.right - rc.left;
	g_height = rc.bottom - rc.top;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;//★---変更--- DirectX11上でDirect2Dを使用するために必要
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] ={
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] ={
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	log("InitializeD3D11 _01\n");

	D3D_DRIVER_TYPE g_driverType     = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++){
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &m_D3DDevice, &g_featureLevel, &m_D3DDeviceContext);
		if (hr == E_INVALIDARG){
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &m_D3DDevice, &g_featureLevel, &m_D3DDeviceContext);
		}
		if(SUCCEEDED(hr)){
			break;
		}
	}
	if(FAILED(hr)){
		return hr;
	}
	log("D3D11 Device:(%p)\n",m_D3DDevice);
	log("D3D11 DeviceContext:(%p)\n",m_D3DDeviceContext);
	return hr;
}
//==================================================
//DX11の初期化
HRESULT InitializeD3D11(HWND hWnd){
	HRESULT hr=0;
	
	log("InitializeD3D11\n");
	
	hr=CreateD3D11Device(hWnd);
	if(FAILED(hr)){
		return hr;
	}
	//======================
	log("InitializeD3D11 _02\n");
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = m_D3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr)){
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if(FAILED(hr)){
		return hr;
	}
	log("DXGI Factory :(%p)\n",dxgiFactory );
	
	//======================
	log("InitializeD3D11 _03\n");
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2){
		log("InitializeD3D11 _04\n");
		hr = m_D3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_D3DDevice1));
		if(SUCCEEDED(hr)){
			m_D3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_D3DDeviceContext1));
		}
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width  = g_width;
		sd.Height = g_height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		hr = dxgiFactory2->CreateSwapChainForHwnd(m_D3DDevice, hWnd, &sd, nullptr, nullptr, &m_DXGISwapChain1);
		if (SUCCEEDED(hr)){
			hr = m_DXGISwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_DXGISwapChain));
		}
		dxgiFactory2->Release();
	}else{
		log("InitializeD3D11 _05\n");
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width  = g_width;
		sd.BufferDesc.Height = g_height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		hr = dxgiFactory->CreateSwapChain(m_D3DDevice, &sd, &m_DXGISwapChain);
	}
	log("InitializeD3D11 _06\n");
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiFactory->Release();
	if(FAILED(hr)){
		return hr;
	}
	log("SwapChain:(%p)\n",m_DXGISwapChain);
	log("SwapChain1:(%p)\n",m_DXGISwapChain1);
	

	log("InitializeD3D11 _07\n");
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if(FAILED(hr)){
		return hr;
	}
	log("pBackBuffer:(%p)\n",pBackBuffer );

	log("InitializeD3D11 _08\n");
	hr = m_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_D3DRenderTargetView);
	pBackBuffer->Release();
	if(FAILED(hr)){
		return hr;
	}
	log("m_D3DRenderTargetView:(%p)\n",m_D3DRenderTargetView);
	

	log("InitializeD3D11 _09\n");
	m_D3DDeviceContext->OMSetRenderTargets(1, &m_D3DRenderTargetView, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)g_width;
	vp.Height = (FLOAT)g_height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_D3DDeviceContext->RSSetViewports(1, &vp);
	log("InitializeD3D11 _10\n");

   //--------------------------★追加↓--------------------------
	// Direct2D,DirectWriteの初期化
	log("InitializeD3D11 _10\n");
//	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
//	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,__uuidof(ID2D1Factory1),reinterpret_cast<void**>(&m_D2DFactory));
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,__uuidof(ID2D1Factory4),reinterpret_cast<void**>(&m_D2DFactory));
	if (FAILED(hr)){
		return hr;
	}
	log("m_D2DFactory(%p)\n",m_D2DFactory);
	
	log("InitializeD3D11 _11\n");
	hr = m_DXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&m_DXGISurface));
	if (FAILED(hr)){
		return hr;
	}
	log("m_DXGISurface(%p)\n",m_DXGISurface);

	FLOAT dpiX;
	FLOAT dpiY;
	m_D2DFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

	log("InitializeD3D11 _12\n");
	hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(m_DXGISurface, &props, &m_D2DRenderTarget);
	if (FAILED(hr)){
		return hr;
	}
	log("m_D2DRenderTarget(%p)\n",m_D2DRenderTarget);
	
	
	hr=m_D2DRenderTarget->QueryInterface(__uuidof(ID2D1DeviceContext3), reinterpret_cast<void**>(&m_D2DDeviceContext3));
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !!\n");
		return hr;
	}
	log("m_D2DDeviceContext3(%p)\n",m_D2DDeviceContext3);
	
	hr=m_D2DDeviceContext3->CreateSpriteBatch(&m_spriteBatch);
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !!\n");
		return hr;
	}
	log("m_spriteBatch(%p)\n",m_spriteBatch);
	
	InitSpriteBatch(m_spriteBatch);



	log("InitializeD3D11 _99\n");
	return hr;
}
//==================================================
HRESULT InitializeD2D_UseDX11(HWND hWnd){
	log("InitializeD2D_UseDX11\n");
	HRESULT hr=0;
//	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,__uuidof(ID2D1Factory1),reinterpret_cast<void**>(&m_D2DFactory));
	if(FAILED(hr)){
		log("D2D1CreateFactory Error !!\n");
		return hr;
	}
	log("m_D2DFactory(%p)\n",m_D2DFactory);
	
	/*
	hr=m_D2DFactory->QueryInterface(__uuidof(ID2D1Factory2), reinterpret_cast<void**>(&m_D2DFactory2));
	log("m_D2DFactory2(%p)\n",m_D2DFactory2);
	if(FAILED(hr)){
		log("m_D2DFactory2 Error !!\n");
		return hr;
	}
	hr=m_D2DFactory->QueryInterface(__uuidof(ID2D1Factory3), reinterpret_cast<void**>(&m_D2DFactory3));
	log("m_D2DFactory3(%p)\n",m_D2DFactory3);
	if(FAILED(hr)){
		log("m_D2DFactory3 Error !!\n");
		return hr;
	}
	hr=m_D2DFactory->QueryInterface(__uuidof(ID2D1Factory4), reinterpret_cast<void**>(&m_D2DFactory4));
	log("m_D2DFactory4(%p)\n",m_D2DFactory3);
	if(FAILED(hr)){
		log("m_D2DFactory4 Error !!\n");
		return hr;
	}
	*/
	
	
	//hr = m_D3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_DXGIDevice);
	hr = m_D3DDevice->QueryInterface(IID_PPV_ARGS(&m_DXGIDevice));
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 01", "caption", 0);
		log("m_DXGIDevice Error !!\n");
		return hr;
	}
	log("m_DXGIDevice(%p)\n",m_DXGIDevice);
	
//	hr=D2D1CreateDevice(m_DXGIDevice,NULL,&m_D2DDevice);
	hr = m_D2DFactory->CreateDevice(m_DXGIDevice,&m_D2DDevice);
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 02", "caption", 0);
		log("D2D1CreateDevice Error !!:hr(%08x)\n",hr);
		return hr;
	}
	log("m_D2DDevice3(%p)\n",m_D2DDevice);
	
	/*
	//D2D1_CREATION_PROPERTIES prop;
	//prop.threadingMode=D2D1_THREADING_MODE_SINGLE_THREADED;
	//prop.debugLevel   =D2D1_DEBUG_LEVEL_NONE;
	//prop.options      =D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	
	//hr=D2D1CreateDeviceContext(m_DXGISurface,&prop,&m_D2DDeviceContext);
	hr=D2D1CreateDeviceContext(m_DXGISurface,NULL,&m_D2DDeviceContext);
//	hr=D2D1CreateDeviceContext3(m_DXGISurface,NULL,&m_D2DDeviceContext3);
	//hr=D2D1CreateDeviceContext(m_DXGISurface,NULL,&m_D2DDeviceContext3);
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 03", "caption", 0);
		log("D2D1CreateDeviceContext Error !!\n");
		return hr;
	}
	log("m_D2DDeviceContext(%p)\n",m_D2DDeviceContext);
	
	
	hr=m_D2DDeviceContext->QueryInterface(__uuidof(ID2D1DeviceContext3), reinterpret_cast<void**>(&m_D2DDeviceContext3));
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !!\n");
		return hr;
	}
	log("m_D2DDeviceContext3(%p)\n",m_D2DDeviceContext3);
	*/
	/*
	hr = m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DDeviceContext3);
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !!\n");
		return hr;
	}
	log("m_D2DDeviceContext3(%p)\n",m_D2DDeviceContext3);
	*/
	
	/*
	hr=m_D2DDeviceContext3->CreateSpriteBatch(&m_spriteBatch);
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !!\n");
		return hr;
	}
	log("m_spriteBatch(%p)\n",m_spriteBatch);
	
	InitSpriteBatch(m_spriteBatch);
	*/
	
	log("InitializeD2D_UseDX11 ok  !!\n");
	return hr;
}
//==================================================
//D2Dの初期化
HRESULT InitializeD2D_UnUseDX11(HWND hWnd){
	log("InitializeD2D_UnUseDX11\n");
	HRESULT hr=0;
//	REFIID m_D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	if(FAILED(hr))return hr;
	
	/*
	hr = m_D3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_DXGIDevice);
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 01", "caption", 0);
		log("m_DXGIDevice Error !!\n");
		return hr;
	}
	
	D2D1_CREATION_PROPERTIES prop;
	prop.threadingMode=D2D1_THREADING_MODE_SINGLE_THREADED;
	prop.debugLevel   =D2D1_DEBUG_LEVEL_NONE;
	prop.options      =D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	
	//hr=D2D1CreateDevice(m_DXGIDevice,&prop,&m_D2DDevice);
	hr=D2D1CreateDevice(m_DXGIDevice,NULL,&m_D2DDevice);
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 02", "caption", 0);
		log("D2D1CreateDevice Error !!:hr(%08x)\n",hr);
		return hr;
	}
	hr=D2D1CreateDeviceContext(m_DXGISurface,&prop,&m_D2DDeviceContext);
	if(FAILED(hr)){
		//MessageBox(NULL, "InitializeD2D err 03", "caption", 0);
		log("D2D1CreateDeviceContext Error !!\n");
		return hr;
	}
	*/
	
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	
	D2D1_SIZE_U size = D2D1::SizeU(
	    rc.right - rc.left,
	    rc.bottom - rc.top);

	// Create a Direct2D render target.
	hr = m_D2DFactory->CreateHwndRenderTarget(
	    D2D1::RenderTargetProperties(),
	    D2D1::HwndRenderTargetProperties(m_hWnd, size),
	    &m_pRenderTarget);
	if(FAILED(hr))return hr;
	log("m_pRenderTarget(%p)\n",m_pRenderTarget);
	
	// Create a gray brush.
//	hr = m_pRenderTarget->CreateSolidColorBrush(
//		D2D1::ColorF(D2D1::ColorF::LightSlateGray),
//		&m_pLightSlateGrayBrush);
//	if(FAILED(hr))return hr;
//	log("m_pLightSlateGrayBrush(%p)\n",m_pLightSlateGrayBrush);
	
	
	hr=m_pRenderTarget->QueryInterface(__uuidof(ID2D1DeviceContext3), reinterpret_cast<void**>(&m_D2DDeviceContext3));
	if(FAILED(hr)){
		log("m_D2DDeviceContext3 Error !\n");
	}else{
		log("m_D2DDeviceContext3(%p)\n",m_D2DDeviceContext3);
	}
	
	if(m_D2DDeviceContext3){
		hr=m_D2DDeviceContext3->CreateSpriteBatch(&m_spriteBatch);
		if(FAILED(hr)){
			log("m_D2DDeviceContext3 Error !!\n");
			return hr;
		}
		log("m_spriteBatch(%p)\n",m_spriteBatch);
		
		InitSpriteBatch(m_spriteBatch);
		
	}
	
	
	/*
	IDXGISurface *m_DXGISurface=NULL;
	ID2D1DeviceContext *m_D2DDeviceContext=NULL;
	hr=D2D1CreateDeviceContext(m_DXGISurface,NULL,&m_D2DDeviceContext);
	
	
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
				 D3D11_CREATE_DEVICE_BGRA_SUPPORT
	
	D3D_DRIVER_TYPE driverType=D3D_DRIVER_TYPE_HARDWARE;
	const D3D_FEATURE_LEVEL featureLevels[] ={
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1,
    };
	ID3D11Device *D3D11_device;
 	hr = D3D11CreateDevice(nullptr,
      driverType,
      nullptr,
      flags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      &D3D11_device,
      nullptr,
      nullptr);
 
    if(FAILED(hr))return hr;
    IDXGIDevice2 *dxgiDevice;
//    hr = D3D11_device->As(&dxgiDevice);
	hr = D3D11_device->QueryInterface(__uuidof(IDXGIDevice2), (void **)&dxgiDevice);
    if (FAILED(hr)) return hr;
 
//    ID2D1Device *d2d1Device;
//    hr = m_D2DFactory->CreateDevice(dxgiDevice.Get(), &d2d1Device);
//    if (FAILED(hr)) return hr;
	hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget();
	if (FAILED(hr)) return hr;
	*/
	//printf("InitializeD2D ok !!\n");
	//MessageBox(NULL, "InitializeD2D ok !!", "caption", 0);
	return hr;
}
//==================================================
void InitSpriteBatch(ID2D1SpriteBatch *SpriteBatch){
	if(!SpriteBatch)return;
	HRESULT hr=0;
	//SpriteBatch->Clear();
	int spriteCount=1;
	//D2D1_RECT_F sp_dst[]={D2D1::RectF(0,0,g_width,g_height)};
	const static D2D1_RECT_F sp_dst[]={D2D1::RectF(0,0,256,256)};
	const static D2D_RECT_U sp_src[]={{0,0,256,256}};
	const static D2D1_COLOR_F sp_col[]={D2D1::ColorF(1,1,1,1)};
	const static D2D1_MATRIX_3X2_F sp_mt[]={D2D1::IdentityMatrix()};
	//SpriteBatch->SetSprites(0,spriteCount,sp_dst,sp_src,sp_col,sp_mt,0,0,0,0);
	hr=SpriteBatch->AddSprites(spriteCount,sp_dst,sp_src,sp_col,sp_mt,0,0,0,0);
	if(FAILED(hr)){
		log("AddSprites Error !!\n");
		return;
	}
	log("InitSpriteBatch ok !!\n");
}
//==================================================
HRESULT Init_DXGI(HWND hWnd){
	HRESULT hr=0;
	
	RECT rect;
	GetClientRect(hWnd,&rect);

	
	//	DirectX11版PresentationParameter
//			バッファの数やサイズ、カラーフォーマット等を設定する
//UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;//★---変更--- DirectX11上でDirect2Dを使用するために必要
	
	DXGI_SWAP_CHAIN_DESC dxgi;
	ZeroMemory(&dxgi, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgi.BufferCount = 1;					// バッファの数
	dxgi.BufferDesc.Width = (rect.right - rect.left);	// バッファの横幅
	dxgi.BufferDesc.Height = (rect.bottom - rect.top);	// バッファの縦幅
//	dxgi.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// カラーフォーマット
	dxgi.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;	// カラーフォーマット
	dxgi.BufferDesc.RefreshRate.Numerator = 60;		// リフレッシュレートの分母
	dxgi.BufferDesc.RefreshRate.Denominator = 1;		// リフレッシュレートの分子
	dxgi.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使い方 Usage => 使用方法
	dxgi.OutputWindow = hWnd;			// 出力対象のウィンドウハンドル
	dxgi.SampleDesc.Count = 1;				// マルチサンプリングのサンプル数(未使用は1)
	dxgi.SampleDesc.Quality = 0;				// マルチサンプリングの品質(未使用は0)
	dxgi.Windowed = true;					// ウィンドウモード指定

	D3D_FEATURE_LEVEL level;

	// デバイス生成とスワップチェーン作成を行う
	hr=D3D11CreateDeviceAndSwapChain(
		nullptr,			// ビデオアダプタ指定(nullptrは既定)
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプ
		nullptr,			// D3D_DRIVER_TYPE_SOFTWARE指定時に使用
		0,				// フラグ指定
		nullptr,			// D3D_FEATURE_LEVEL指定で自分で定義した配列を指定可能
		0,				// 上のD3D_FEATURE_LEVEL配列の要素数
		D3D11_SDK_VERSION,		// SDKバージョン
		&dxgi,				// DXGI_SWAP_CHAIN_DESC
		&m_DXGISwapChain,			// 関数成功時のSwapChainの出力先 
		&m_D3DDevice,			// 関数成功時のDeviceの出力先
		&level,				// 成功したD3D_FEATURE_LEVELの出力先
		&m_D3DDeviceContext);			// 関数成功時のContextの出力先
	
	if(FAILED(hr)){
		return hr;
	}
	
	hr = m_DXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&m_DXGISurface));
	if(FAILED(hr)){
		return hr;
	}
	
	//MessageBox(NULL, "Init_DXGI ok !!", "caption", 0);
	return hr;
}

HRESULT Init_WICImagingFactory(){
	HRESULT hr=0;
	//hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>( &m_IWICImagingFactory) );
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&m_IWICImagingFactory));
	if(FAILED(hr)){
		log("m_IWICImagingFactory error !!\n");
		return hr;
	}
	log("m_IWICImagingFactory(%p)\n",m_IWICImagingFactory);
	return hr;
}
//==================================================
ID2D1Bitmap *LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
//	IWICImagingFactory *pIWICFactory,
	PCWSTR uri
//	UINT destinationWidth,
//	UINT destinationHeight,
//	ID2D1Bitmap **ppBitmap
){
	HRESULT hr=0;
	
	ID2D1Bitmap *ppBitmap=NULL;
	if(!m_IWICImagingFactory){
		log("m_IWICImagingFactory==NULL\n");
		return NULL;
	}
	if(!pRenderTarget){
		log("pRenderTarget==NULL\n");
		return NULL;
	}
	
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	hr = m_IWICImagingFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder);
	if(FAILED(hr)){
		log("LoadBitmapFromFile Error 01\n");
		return NULL;
	}
	
	// Create the initial frame.
	hr = pDecoder->GetFrame(0, &pSource);
	if(FAILED(hr)){
		log("LoadBitmapFromFile Error 02\n");
		return NULL;
	}

	// Convert the image format to 32bppPBGRA
	// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
	hr = m_IWICImagingFactory->CreateFormatConverter(&pConverter);
	if(FAILED(hr)){
		log("LoadBitmapFromFile Error 03\n");
		return NULL;
	}
	
	hr = pConverter->Initialize(
		pSource,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut);
	if(FAILED(hr)){
		log("LoadBitmapFromFile Error 04\n");
		return NULL;
	}
	// Create a Direct2D bitmap from the WIC bitmap.
	hr = pRenderTarget->CreateBitmapFromWicBitmap(
		pConverter,
		NULL,
		&ppBitmap);
	if(FAILED(hr)){
		log("LoadBitmapFromFile Error 05\n");
		return NULL;
	}
	
	if(pDecoder)pDecoder->Release();
	if(pSource)pSource->Release();
	if(pStream)pStream->Release();
	if(pConverter)pConverter->Release();
	if(pScaler)pScaler->Release();
	log("ppBitmap(%p)\n",ppBitmap);
	return ppBitmap;
}
//==================================================
void OnRenderer(){
	if(render_type==0){
		//OnRenderer1(m_D2DRenderTarget);
		if(m_D2DDeviceContext3){
			OnRenderer1(m_D2DDeviceContext3);
		}else{
			OnRenderer1(m_pRenderTarget);
		}
	}else{
		//OnRenderer2(m_D2DRenderTarget);
		OnRenderer2(m_D2DDeviceContext3);
	}
}
void OnRenderer1(ID2D1RenderTarget *RenderTarget){
//	log("OnRenderer1:RenderTarget(%p)\n",RenderTarget);
	if(!RenderTarget)return;
	
	
	if(!m_bitmap){
		PCWSTR filename=L"check.bmp";
		m_bitmap=LoadBitmapFromFile(RenderTarget,filename);
	}
	//InitSpriteBatch(m_spriteBatch);
	
	RenderTarget->BeginDraw();
	RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Red));
	
	
	{
	D2D1::Matrix3x2F mt=D2D1::Matrix3x2F::Translation(223,134);
	RenderTarget->SetTransform(mt);
	//D2D1_RECT_F dst_rect=D2D1::RectF(0,0,g_width,g_height);
	D2D1_RECT_F src_rect=D2D1::RectF(0,0,256,256);
	D2D1_RECT_F dst_rect=src_rect;
	//D2D1_RECT_F src_rect=D2D1::RectF(128,0,256,128);
	//D2D1_RECT_F dst_rect=D2D1::RectF(0,0,128,128);
	RenderTarget->DrawBitmap(m_bitmap,dst_rect,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,src_rect);
	}
	
	
	if(RenderTarget && m_bitmap &&m_spriteBatch){
		/*
		((ID2D1DeviceContext3 *)RenderTarget)->DrawSpriteBatch(
			m_spriteBatch,
			0,
			1,
			m_bitmap ,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1_SPRITE_OPTIONS_CLAMP_TO_SOURCE_RECTANGLE);
		*/
		((ID2D1DeviceContext3 *)RenderTarget)->DrawSpriteBatch(
			m_spriteBatch,
			m_bitmap,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	}
	
	
	HRESULT hr=RenderTarget->EndDraw();
	if(hr)log("hr(%p)\n",hr);
	
	//MessageBox(NULL, "OnRenderer ok !!", "caption", 0);
}
void OnRenderer2(ID2D1RenderTarget *RenderTarget){
	if(!RenderTarget)return;
//	log("OnRenderer2:m_D2DRenderTarget(%p),m_D2DDeviceContext3(%p),m_D3DDeviceContex(%p)\n",m_D2DRenderTarget,m_D2DDeviceContext3,m_D3DDeviceContext);
	
	RenderTarget->BeginDraw();
	
	if(!m_bitmap){
		PCWSTR filename=L"check.bmp";
		m_bitmap=LoadBitmapFromFile(RenderTarget,filename);
	}
	//InitSpriteBatch(m_spriteBatch);
	
//	float col[]={1,0,1,1};
//	m_D3DDeviceContext->ClearRenderTargetView(m_D3DRenderTargetView, col);
	
	RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Green));
	
	
	{
	D2D1::Matrix3x2F mt=D2D1::Matrix3x2F::Translation(223,134);
	RenderTarget->SetTransform(mt);
	//D2D1_RECT_F dst_rect=D2D1::RectF(0,0,g_width,g_height);
	D2D1_RECT_F src_rect=D2D1::RectF(0,0,256,256);
	D2D1_RECT_F dst_rect=src_rect;
	//D2D1_RECT_F src_rect=D2D1::RectF(128,0,256,128);
	//D2D1_RECT_F dst_rect=D2D1::RectF(0,0,128,128);
	RenderTarget->DrawBitmap(m_bitmap,dst_rect,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,src_rect);
	}
	
	
	if(m_D2DDeviceContext3 && m_bitmap &&m_spriteBatch){
		log("OnRenderer2:m_D2DRenderTarget::DrawSpriteBatch\n");
		
		m_D2DDeviceContext3->DrawSpriteBatch(
			m_spriteBatch,
			m_bitmap,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			//D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			//D2D1_SPRITE_OPTIONS_NONE);
	}
	
	
	
	
	HRESULT hr=RenderTarget->EndDraw();
	if(hr)log("hr(%p)\n",hr);
	
	//MessageBox(NULL, "OnRenderer ok !!", "caption", 0);
	
	m_DXGISwapChain->Present(0,0 );
}

//==================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HRESULT hr;
	
	switch(msg){
		case WM_DESTROY:
			{
			CloseLog();
			PostQuitMessage(0);
			}
			break;
		case WM_CREATE:
			SetTimer(hWnd , 1 , 1000/60 , NULL);
			break;
		case WM_PAINT:
			{
			BeginPaint(hWnd, nullptr);
			OnRenderer();
			EndPaint(hWnd, nullptr);
			ValidateRect(hWnd, NULL);
			}
			break;
		case WM_TIMER:
			InvalidateRect(hWnd, 0, TRUE);
			break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}




