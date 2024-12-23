/*
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
	float3 normal: NORMAL; 
};

PixelInputType DeferredVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex; 

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}
*/

/* Update normalmap */

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
	float3 tangent: TANGENT;
	float3 binormal: BINORMAL;
};

struct PixelInputType
{
	float4 position: SV_POSITION; 
	float2 tex: TEXCOORD0; 
	float3 normal: NORMAL; 
	float3 tangent: TANGENT;
	float3 binormal: BINORMAL;
};

PixelInputType DeferredVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex; 

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	return output;
}