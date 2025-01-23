#include "headers/d3d.h"

D3D::D3D()
{
  m_swapChain = nullptr;
  m_device = nullptr;
  m_deviceContext = nullptr;
  m_renderTargetView = nullptr;
  m_depthStencilBuffer = nullptr;
  m_depthStencilState = nullptr;
  m_depthDisabledStencilState = nullptr;
  m_depthStencilView = nullptr;
  m_rasterState = 0;
  m_alphaEnableBlendingState = nullptr;
  m_alphaDisableBlendingState = nullptr;
}

D3D::D3D(const D3D& other) {}

D3D::~D3D() {}

bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
  HRESULT result;
  IDXGIFactory* factory;
  IDXGIAdapter* adapter;
  IDXGIOutput* adapterOutput;
  unsigned int numModes, i, numerator = 0, denominator = 0;
  unsigned long long stringLength;
  DXGI_MODE_DESC* displayModeList;
  DXGI_ADAPTER_DESC adapterDesc;
  int error;
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  D3D_FEATURE_LEVEL featureLevel;
  ID3D11Texture2D* backBufferPtr;
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  D3D11_RASTERIZER_DESC rasterDesc;
  float fieldOfView, screenAspect;
  D3D11_BLEND_DESC blendStateDescription;
  D3D11_BLEND_DESC smokeBlendStateDescription;

  m_vsync_enabled = vsync;

  result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
  if(FAILED(result))
    {
      return false;
    }

  result = factory->EnumAdapters(0, &adapter);
  if(FAILED(result))
    {
      return false;
    }

  result = adapter->EnumOutputs(0, &adapterOutput);
  if(FAILED(result))
    {
      return false;
    }

  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
  if(FAILED(result))
    {
      return false;
    }

  displayModeList = new DXGI_MODE_DESC[numModes];
  if(!displayModeList)
    {
      return false;
    }

  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
  if(FAILED(result))
    {
      return false;
    }

  for(i = 0; i < numModes; i++)
    {
      if(displayModeList[i].Width == (unsigned int) screenWidth)
	{
	  if(displayModeList[i].Height == (unsigned int) screenHeight)
	    {
	      numerator = displayModeList[i].RefreshRate.Numerator;
	      denominator = displayModeList[i].RefreshRate.Denominator;
	    }
	}
    }

  result = adapter->GetDesc(&adapterDesc);
  if(FAILED(result))
    {
      return false;
    }

  m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

  error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
  if(error != 0)
    {
      return false;
    }

  delete[] displayModeList;
  displayModeList = NULL;

  adapterOutput->Release();
  adapterOutput = NULL;

  adapter->Release();
  adapter = NULL;

  factory->Release();
  factory = NULL;

  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

  swapChainDesc.BufferCount = 1;

  swapChainDesc.BufferDesc.Width = screenWidth;
  swapChainDesc.BufferDesc.Height = screenHeight;

  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  if(m_vsync_enabled)
    {
      swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
      swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
  else
    {
      swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
      swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  swapChainDesc.OutputWindow = hwnd;

  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;

  if(fullscreen)
    {
      swapChainDesc.Windowed = false;
    }
  else
    {
      swapChainDesc.Windowed = true;
    }

  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  swapChainDesc.Flags = 0;

  featureLevel = D3D_FEATURE_LEVEL_11_0;

  
  result = D3D11CreateDeviceAndSwapChain(NULL,
					 D3D_DRIVER_TYPE_HARDWARE,
					 NULL, 0, &featureLevel, 1,
					 D3D11_SDK_VERSION,
					 &swapChainDesc,
					 &m_swapChain,
					 &m_device, NULL,
					 &m_deviceContext);
  if(FAILED(result))
    {
      return false;
    }

  result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
  if(FAILED(result))
    {
      return false;
    }

  result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
  if(FAILED(result))
    {
      return false;
    }

  backBufferPtr->Release();
  backBufferPtr = NULL;

  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

  depthBufferDesc.Width = screenWidth;
  depthBufferDesc.Height = screenHeight;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
  if(FAILED(result))
    {
      return false;
    }

  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
  if(FAILED(result))
    {
      return false;
    }

  m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
  if(FAILED(result))
    {
      return false;
    }

  m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

  rasterDesc.AntialiasedLineEnable = false;
  rasterDesc.CullMode = D3D11_CULL_BACK;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = true;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = false;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;

  result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
  if(FAILED(result))
    {
      return false;
    }

  m_deviceContext->RSSetState(m_rasterState);

  m_viewport.Width = (float)screenWidth;
  m_viewport.Height = (float)screenHeight;
  m_viewport.MinDepth = 0.0f;
  m_viewport.MaxDepth = 1.0f;
  m_viewport.TopLeftX = 0.0f;
  m_viewport.TopLeftY = 0.0f;

  m_deviceContext->RSSetViewports(1, &m_viewport);

  fieldOfView = 3.141592654f / 4.0f;
  screenAspect = (float)screenWidth / (float)screenHeight;

  m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

  m_worldMatrix = XMMatrixIdentity();

  m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

  ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

  depthDisabledStencilDesc.DepthEnable = false;
  depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthDisabledStencilDesc.StencilEnable = true;
  depthDisabledStencilDesc.StencilReadMask = 0xFF;
  depthDisabledStencilDesc.StencilWriteMask = 0xFF;
  depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
  if(FAILED(result))
    {
      return false;
    }

  ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

  blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
  blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
  blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

  result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
  if (FAILED(result))
  {
      return false;
  }

  blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

  result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
  if (FAILED(result))
  {
      return false;
  }  

  ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

  // Premultiplied alpha blend state: result = src.RGB + (dest.RGB * (1 - src.A))
  blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
  blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
  blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
  blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
  blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

  result = m_device->CreateBlendState(&blendStateDescription, &m_alphaParticleEnableBlendingState);
  if (FAILED(result))
  {
      return false; 
  }

  return true;
}

void D3D::Shutdown()
{
  if(m_swapChain)
    {
      m_swapChain->SetFullscreenState(false, NULL);
    }

  if (m_alphaEnableBlendingState)
  {
      m_alphaEnableBlendingState->Release();
      m_alphaEnableBlendingState = nullptr;
  }

  if (m_alphaDisableBlendingState)
  {
      m_alphaDisableBlendingState->Release();
      m_alphaDisableBlendingState = nullptr;
  }

  if(m_depthDisabledStencilState)
    {
      m_depthDisabledStencilState->Release();
      m_depthDisabledStencilState = nullptr;
    }

  if(m_rasterState)
    {
      m_rasterState->Release();
      m_rasterState = nullptr;
    }
	
  if(m_depthStencilView)
    {
      m_depthStencilView->Release();
      m_depthStencilView = nullptr;
    }

  if(m_depthStencilState)
    {
      m_depthStencilState->Release();
      m_depthStencilState = nullptr;
    }

  if(m_depthStencilBuffer)
    {
      m_depthStencilBuffer->Release();
      m_depthStencilBuffer = nullptr;
    }

  if(m_renderTargetView)
    {
      m_renderTargetView->Release();
      m_renderTargetView = nullptr;
    }

  if(m_deviceContext)
    {
      m_deviceContext->Release();
      m_deviceContext = nullptr;
    }

  if(m_device)
    {
      m_device->Release();
      m_device = nullptr;
    }

  if(m_swapChain)
    {
      m_swapChain->Release();
      m_swapChain = nullptr;
    }
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
  float color[4];

  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = alpha;

  m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

  m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
  if(m_vsync_enabled)
    {
      m_swapChain->Present(1, 0);
    }
  else
    {
      m_swapChain->Present(0, 0);
    }
}

ID3D11Device* D3D::GetDevice()
{
  return m_device;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
  return m_deviceContext;
}

void D3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
  projectionMatrix = m_projectionMatrix;
}

void D3D::GetWorldMatrix(XMMATRIX& worldMatrix)
{
  worldMatrix = m_worldMatrix;
}

void D3D::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
  orthoMatrix = m_orthoMatrix;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory)
{
  strcpy_s(cardName, 128, m_videoCardDescription);
  memory = m_videoCardMemory;
}

void D3D::SetBackBufferRenderTarget()
{
  m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3D::ResetViewport()
{
  m_deviceContext->RSSetViewports(1, &m_viewport);
}

void D3D::TurnZBufferOn()
{
  m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3D::TurnZBufferOff()
{
  m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void D3D::EnableAlphaBlending()
{
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3D::DisableAlphaBlending()
{
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void D3D::EnableParticleAlphaBlending()
{
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaParticleEnableBlendingState, blendFactor, 0xffffffff);
}
