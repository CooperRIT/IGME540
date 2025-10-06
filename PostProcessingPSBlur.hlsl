cbuffer externalData : register(b0)
{
	int type;

	//Blur Stuff
	int blurRadius;
	float pixelWidth;
	float pixelHeight;

	//Chromatic Aberation
	float aberrationAmount;
	float2 aberrationCenter;
}

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);


float4 main(VertexToPixel input) : SV_TARGET
{
	switch (type)
	{
		case 0:
			// Track the total color and number of samples
			float4 total = 0;
			int sampleCount = 0;
			// Loop through the "box"
			for (int x = -blurRadius; x <= blurRadius; x++)
			{
				for (int y = -blurRadius; y <= blurRadius; y++)
				{
					// Calculate the uv for this sample
					float2 uv = input.uv;
					uv += float2(x * pixelWidth, y * pixelHeight);
					// Add this color to the running total
					total += Pixels.Sample(ClampSampler, uv);
					sampleCount++;
				}
			}
			// Return the average
			return total / sampleCount;
			break;
		case 1:
			// Calculate direction from center
			float2 direction = input.uv - aberrationCenter;

			float2 redOffset = aberrationCenter + direction * (1.0 + aberrationAmount);
			float2 blueOffset = aberrationCenter + direction * (1.0 - aberrationAmount);
			float2 greenOffset = input.uv;

			float r = Pixels.Sample(ClampSampler, redOffset).r;
			float g = Pixels.Sample(ClampSampler, greenOffset).g;
			float b = Pixels.Sample(ClampSampler, blueOffset).b;

			return float4(r, g, b, 1.0);
			break;
	}

	return float4(0, 0, 0, 0);
}