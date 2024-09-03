cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer
{
	float4 clipPlane;
	float rotationAngle; 
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
	float clip: SV_ClipDistance0;
};

PixelInputType ClipPlaneVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	// float cosAngle = cos(rotationAngle);
	// float sinAngle = sin(rotationAngle);
	// 
	// float3 rotatedNormal = float3(
    //     clipPlane.x,
    //     clipPlane.y * cosAngle - clipPlane.z * sinAngle,
    //     clipPlane.y * sinAngle + clipPlane.z * cosAngle
    // );
	// 
	// output.clip = dot(rotatedNormal, input.position.xyz) + clipPlane.w;
	// 

	output.clip = dot(mul(input.position, worldMatrix), clipPlane);

	return output;
}