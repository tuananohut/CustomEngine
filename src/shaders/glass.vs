cbuffer MatrixBufferType
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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
	float4 refractionPosition: TEXCOORD1;
};

PixelInputType GlassVertexShader(VertexInputType input)
{
	PixelInputType output;
	matrix viewProjectWorld;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	viewProjectWorld = mul(viewMatrix, projectionMatrix);
	viewProjectWorld = mul(worldMatrix, viewProjectWorld);

	output.refractionPosition = mul(input.position, viewProjectWorld);

	return output;
}