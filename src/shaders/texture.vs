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
	float3 instancePosition: TEXCOORD1;
	float4 instanceColor: COLOR;
};

struct PixelInputType
{	
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float4 color: COLOR;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	input.position.x += input.instancePosition.x;
	input.position.y += input.instancePosition.y;
	input.position.z += input.instancePosition.z;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.color = input.instanceColor;

	return output;
}