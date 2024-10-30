cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewMatrix2;
	matrix projectionMatrix2;
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
	float4 viewPosition: TEXCOORD1;
};

PixelInputType ProjectionVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.viewPosition, viewMatrix2);
	output.viewPosition = mul(output.viewPosition, projectionMatrix2);

	output.tex = input.tex; 

	return output;
}