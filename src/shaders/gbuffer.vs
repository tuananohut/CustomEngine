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
	float3 normal: NORMAL;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float4 viewPosition: POSITION;
	float3 normal: NORMAL;
};

PixelInputType GBufferVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f; 

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex; 

	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.viewPosition, viewMatrix);

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = mul(output.normal, (float3x3)viewMatrix);

	return output;
}