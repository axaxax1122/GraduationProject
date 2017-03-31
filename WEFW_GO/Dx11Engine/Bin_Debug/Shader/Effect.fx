
#include "Share.fx"

cbuffer UVFrame : register(b1)
{
	unsigned int	g_iFrameX;
	unsigned int	g_iFrameY;
	unsigned int	g_iMaxFrameX;
	unsigned int	g_iMaxFrameY;
	unsigned int	g_iAnimType;
	float3			g_vUVEmpty;
}

Texture2D		g_DiffuseTex	:	register(t0);
SamplerState	g_DiffuseSmp	:	register(s0);
SamplerState	g_LinearSmp		:	register(s1)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_INPUT
{
	float3	vPos	: POSITION;
	float2	vUV		: TEXCOORD;
};

struct VS_OUTPUT
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
};

VS_OUTPUT EffectVS(VS_INPUT input)
{
	VS_OUTPUT	output = (VS_OUTPUT)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);

	if (g_iAnimType != 0)
	{
		float2 vUV = float2(g_iFrameX / (float)g_iMaxFrameX, g_iFrameY / (float)g_iMaxFrameY);

		if (input.vUV.x > 0.f)
			vUV.x = (g_iFrameX + 1) / (float)g_iMaxFrameX;

		if (input.vUV.y > 0.f)
			vUV.y = (g_iFrameY + 1) / (float)g_iMaxFrameY;

		output.vUV = vUV;
	}
	else
		output.vUV = input.vUV;

	return output;
}

struct PS_OUTPUT
{
	float4	vColor	: SV_Target;
};

PS_OUTPUT EffectPS(VS_OUTPUT input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);

	return output;
}