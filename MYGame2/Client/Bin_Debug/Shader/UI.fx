Texture2D		g_DiffuseTex	: register(t0);
SamplerState	g_DiffuseSmp	: register(s0);


cbuffer	Transform	: register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matWV;
	matrix	g_matWVP;
};

cbuffer Material	: register(b1)
{
	float4	g_vMtrlDif;
	float4	g_vMtrlAmb;
	float4	g_vMtrlSpc;
	float4	g_vMtrlEmv;
}



struct VS_INPUT_UI
{
	float3	vPos	: POSITION;
	float2	vUV		: TEXCOORD;
};

struct VS_OUTPUT_UI
{
	float4	vPos	: SV_POSITION;
	float2	vUV	: TEXCOORD;
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
	output.vColor = vColor * g_vMtrlDif;
	output.vColor.a = vColor.a;

	return output;
}
