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

cbuffer LightPositionBuffer
{
	float3 lightPosition;
	float padding;
	float3 lightPosition2;
	float padding2;
	float3 lightPosition3;
	float padding3;
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
	float3 lightPos: TEXCOORD2;
	float4 lightViewPosition2: TEXCOORD3;
	float3 lightPos2: TEXCOORD4;
	float4 lightViewPosition3: TEXCOORD5;
	float3 lightPos3: TEXCOORD6;
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

	worldPosition = mul(input.position, worldMatrix);

	output.lightPos = lightPosition.xyz - worldPosition.xyz;
	output.lightPos = normalize(output.lightPos);
	
	output.lightPos2 = lightPosition2.xyz - worldPosition.xyz;
	output.lightPos2 = normalize(output.lightPos2);

	output.lightPos3 = lightPosition3.xyz - worldPosition.xyz;
	output.lightPos3 = normalize(output.lightPos3);

	return output;
};
