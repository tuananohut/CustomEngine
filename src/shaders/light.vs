#define NUM_LIGHTS 30

cbuffer MatrixBuffer
{	
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightPositionBuffer
{
	float4 lightPosition[NUM_LIGHTS];
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
	float3 lightPos[NUM_LIGHTS]: TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;
	int i;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	worldPosition = mul(input.position, worldMatrix); 

	for (i = 0; i < NUM_LIGHTS; i++)
	{
		output.lightPos[i] = lightPosition[i].xyz - worldPosition.xyz;
		output.lightPos[i] = normalize(output.lightPos[i]);
	}

	return output;
}