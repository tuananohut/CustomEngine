#ifndef _HEATSHADER_H_
#define _HEATSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std; 

class HeatShader
{
private: 
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct GlowBufferType
	{
		float emissiveMultiplier;
		XMFLOAT3 padding;
	};

	struct NoiseBufferType
	{
		float frameTime;
		XMFLOAT3 scrollSpeeds;
		XMFLOAT3 scales; 
		float padding; 
	};

	struct DistortionBufferType
	{
		XMFLOAT2 distortion1;
		XMFLOAT2 distortion2;
		XMFLOAT2 distortion3; 
		float distortionScale;
		float distortionBias; 
	};

public:
	HeatShader();
	HeatShader(const HeatShader&);
	~HeatShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, XMFLOAT3, XMFLOAT3, XMFLOAT2, XMFLOAT2, XMFLOAT2, float, float, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, XMFLOAT3, XMFLOAT3, XMFLOAT2, XMFLOAT2, XMFLOAT2, float, float, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private: 
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_glowBuffer; 
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11Buffer* m_noiseBuffer; 
	ID3D11Buffer* m_distortionBuffer;
};

#endif