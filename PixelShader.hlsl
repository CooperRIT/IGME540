cbuffer DataFromCPP : register(b0)
{
	float time;
}




// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;
	float4 color			: COLOR;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float2 target = float2(200,200);
	float2 pixel = input.screenPosition.xy;

	float dist = distance(target, pixel) * 10;
	float c = sin(dist * .01f);

	float4 fullColor = float4(c, c, c, c);
	float4 colorTint = float4(0.5f, .2f, 1.0f, 1);



	return fullColor * colorTint;
}