#include "headers/blur.h"

Blur::Blur()
{
	m_DownSampleTexture1 = nullptr;
	m_DownSampleTexture2 = nullptr;
	m_DownSampleWindow = nullptr;
	m_UpSampleWindow = nullptr;
}

Blur::Blur(const Blur& other) {}

Blur::~Blur() {}


bool Blur::Initialize(D3D* Direct3D, int downSampleWidth, int downSampleHeight, float screenNear, float screenDepth, int renderWidth, int renderHeight)
{
	bool result;

	m_downSampleWidth = downSampleWidth;
	m_downSampleHeight = downSampleHeight;

	m_DownSampleTexture1 = new RenderTexture;
	result = m_DownSampleTexture1->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight, screenDepth, screenNear, 1);
	if (!result)
	{
		return false;
	}

	m_DownSampleTexture2 = new RenderTexture;
	result = m_DownSampleTexture2->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight, screenDepth, screenNear, 1);
	if (!result)
	{
		return false;
	}

	m_DownSampleWindow = new OrthoWindow;
	result = m_DownSampleWindow->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight);
	if (!result)
	{
		return false;
	}

	m_UpSampleWindow = new OrthoWindow;
	result = m_UpSampleWindow->Initialize(Direct3D->GetDevice(), renderWidth, renderHeight);
	if (!result)
	{
		return false;
	}

	return true;
}


void Blur::Shutdown()
{
	if (m_UpSampleWindow)
	{
		m_UpSampleWindow->Shutdown();
		delete m_UpSampleWindow;
		m_UpSampleWindow = nullptr;
	}

	if (m_DownSampleWindow)
	{
		m_DownSampleWindow->Shutdown();
		delete m_DownSampleWindow;
		m_DownSampleWindow = nullptr;
	}

	if (m_DownSampleTexture2)
	{
		m_DownSampleTexture2->Shutdown();
		delete m_DownSampleTexture2;
		m_DownSampleTexture2 = nullptr;
	}

	if (m_DownSampleTexture1)
	{
		m_DownSampleTexture1->Shutdown();
		delete m_DownSampleTexture1;
		m_DownSampleTexture1 = nullptr;
	}
}


bool Blur::BlurTexture(D3D* Direct3D, Camera* Camera, RenderTexture* RenderTexture, TextureShader* TextureShader, BlurShader* BlurShader)
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float blurType;
	bool result;

	Direct3D->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);

	Direct3D->TurnZBufferOff();

	m_DownSampleTexture1->SetRenderTarget(Direct3D->GetDeviceContext());
	m_DownSampleTexture1->ClearRenderTarget(Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	m_DownSampleTexture1->GetOrthoMatrix(orthoMatrix);

	m_DownSampleWindow->Render(Direct3D->GetDeviceContext());
	result = TextureShader->Render(Direct3D->GetDeviceContext(), m_DownSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, RenderTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	blurType = 0.0f;

	m_DownSampleTexture2->SetRenderTarget(Direct3D->GetDeviceContext());
	m_DownSampleTexture2->ClearRenderTarget(Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	m_DownSampleTexture2->GetOrthoMatrix(orthoMatrix);

	m_DownSampleWindow->Render(Direct3D->GetDeviceContext());
	result = BlurShader->Render(Direct3D->GetDeviceContext(), m_DownSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_DownSampleTexture1->GetShaderResourceView(), m_downSampleWidth, m_downSampleHeight, blurType);
	if (!result)
	{
		return false;
	}

	blurType = 1.0f;

	m_DownSampleTexture1->SetRenderTarget(Direct3D->GetDeviceContext());
	m_DownSampleTexture1->ClearRenderTarget(Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	m_DownSampleTexture1->GetOrthoMatrix(orthoMatrix);

	m_DownSampleWindow->Render(Direct3D->GetDeviceContext());

	result = BlurShader->Render(Direct3D->GetDeviceContext(), m_DownSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_DownSampleTexture2->GetShaderResourceView(), m_downSampleWidth, m_downSampleHeight, blurType);
	if (!result)
	{
		return false;
	}

	RenderTexture->SetRenderTarget(Direct3D->GetDeviceContext());
	RenderTexture->ClearRenderTarget(Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	RenderTexture->GetOrthoMatrix(orthoMatrix);

	m_UpSampleWindow->Render(Direct3D->GetDeviceContext());
	result = TextureShader->Render(Direct3D->GetDeviceContext(), m_UpSampleWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_DownSampleTexture1->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	Direct3D->TurnZBufferOn();

	Direct3D->SetBackBufferRenderTarget();
	Direct3D->ResetViewport();

	return true;
}