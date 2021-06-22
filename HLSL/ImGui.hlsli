struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
	row_major float4x4	wvp;
};
