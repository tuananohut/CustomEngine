cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};

PixelInputType SpecMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    // Extract the upper-left 3x3 matrix from the 4x4 worldMatrix
    float3x3 worldMatrix3x3 = (float3x3)worldMatrix;

    // Transform normal, tangent, and binormal vectors with the 3x3 matrix
    output.normal = mul(input.normal, worldMatrix3x3);
    output.normal = normalize(output.normal);

    output.tangent = mul(input.tangent, worldMatrix3x3);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, worldMatrix3x3);
    output.binormal = normalize(output.binormal);

    worldPosition = mul(input.position, worldMatrix);

    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}
