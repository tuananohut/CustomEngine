cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position: POSITION;
	float4 color: COLOR;
};

struct HullInputType
{
	float3 position: POSITION;
	float4 color: COLOR;
};

HullInputType ColorVertexShader(VertexInputType input)
{
	HullInputType output;

	output.position = input.position;

	output.color = input.color;

	return output;
}