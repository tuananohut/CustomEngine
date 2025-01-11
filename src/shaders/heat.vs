cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer NoiseBuffer
{
	float frameTime; 
	float3 scrollSpeeds;
	float3 scales;
	float padding;
};

struct VertexInputType
{
	float4 position: POSITION;
	float2 tex: TEXCOORD0;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float2 texCoords1: TEXCOORD1;
	float2 texCoords2: TEXCOORD2;
	float2 texCoords3: TEXCOORD3;
};

PixelInputType HeatVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;
	
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.texCoords1 = (input.tex * scales.x);
	output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);

	output.texCoords2 = (input.tex * scales.y);
	output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);

	output.texCoords3 = (input.tex * scales.z);
	output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);

	return output;
}