cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
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
	float4 reflectionPosition: TEXCOORD1;
};


PixelInputType ReflectionVertexShader(VertexInputType input)
{
	PixelInputType output;
	matrix reflectProjectWorld; 

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	return output;
}