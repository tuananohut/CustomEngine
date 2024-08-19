#include "headers/specmapshader.h"

SpecMapShader::SpecMapShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_lightBuffer = nullptr;
	m_cameraBuffer = nullptr;
}

SpecMapShader::SpecMapShader(const SpecMapShader& other) {}

SpecMapShader::~SpecMapShader() {}

bool SpecMapShader::Initialize(ID3D11Device* device, HWND hwnd)
{

}