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
	float4 color: COLOR;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float4 color: COLOR;
};

PixelInputType ParticleVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.color = input.color;

	return output;
}