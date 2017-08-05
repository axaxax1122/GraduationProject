
#include "Share.fx"
#include "Light.fx"


struct VS_INPUT_UI
{
	float3	vPos	: POSITION;
	float2	vUV		: TEXCOORD;
};

struct VS_OUTPUT_UI
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
};

VS_OUTPUT_UI UIVS(VS_INPUT_UI input)
{
	VS_OUTPUT_UI	output = (VS_OUTPUT_UI)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;

	return output;
}

struct PS_OUTPUT
{
	float4	vColor	: SV_Target;
};

PS_OUTPUT UIPS(VS_OUTPUT_UI input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	float4	vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);
	
	output.vColor = vColor * g_vMtrlDif * g_fFadeAmount;
	output.vColor.a = vColor.a;
	
	return output;
}
