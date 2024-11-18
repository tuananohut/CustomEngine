cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix lightViewMatrix2;
	matrix lightProjectionMatrix2;
	matrix lightViewMatrix3;
	matrix lightProjectionMatrix3;
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
	float4 lightViewPosition: TEXCOORD1;
	float4 lightViewPosition2: TEXCOORD2;
	float4 lightViewPosition3: TEXCOORD3;
};

PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	output.lightViewPosition2 = mul(input.position, worldMatrix);
	output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix2);
	output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix2);

	output.lightViewPosition3 = mul(input.position, worldMatrix);
	output.lightViewPosition3 = mul(output.lightViewPosition3, lightViewMatrix3);
	output.lightViewPosition3 = mul(output.lightViewPosition3, lightProjectionMatrix3);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}
