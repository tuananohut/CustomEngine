cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};

struct HullInputType
{
	float3 position: POSITION;
	float4 color: COLOR;
};

struct ConstantOutputType
{
	float edges[3]: SV_TessFactor;
	float inside: SV_InsideTessFactor;
};

struct HullOutputType
{
	float3 position: POSITION;
	float4 color: COLOR;
};

ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId: SV_PrimitiveID)
{
	ConstantOutputType output;

	output.edges[0] = tessellationAmount;
	output.edges[1] = tessellationAmount;
	output.edges[2] = tessellationAmount;

	output.inside = tessellationAmount;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 3> patch, uint pointId: SV_OutputControlPointID, uint patchId: SV_PrimitiveID)
{
	HullOutputType output;

	output.position = patch[pointId].position;

	output.color = patch[pointId].color;

	return output;
}
