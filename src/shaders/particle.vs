cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position: POSITION;
	float2 tex: TEXCOORD0;
	float4 data1: TEXCOORD1;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float4 data1: TEXCOORD1;
	float2 texCoords1: TEXCOORD2;
};

PixelInputType ParticleVertexShader(VertexInputType input)
{
	PixelInputType output;
	float scroll1X, scroll1Y; 

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.data1 = input.data1;

	scroll1X = input.data1.y; 
	scroll1Y = input.data1.z; 

	output.texCoords1.x = input.tex.x - scroll1X; 
	output.texCoords1.y = input.tex.y - scroll1Y; 

	return output;
}