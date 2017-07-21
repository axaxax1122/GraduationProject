
#include "Share.fx"

// 텍스처 큐브로 설정
TextureCube		g_DiffuseTex	: register(t0);

struct VS_INPUT_SKY
{
	float3	vPos	: POSITION;
};

struct VS_OUTPUT_SKY
{
	float4	vPos	: SV_POSITION;
	float3	vUV		: TEXCOORD;
};


VS_OUTPUT_SKY SkyVS(VS_INPUT_SKY input)
{
	VS_OUTPUT_SKY	output = (VS_OUTPUT_SKY)0;

	// 투영 공간에서 원근감을 표현하기 위해 
	// 내부적으로 동차 공간으로 바꿔주는데, 이때 x,y,z값에 w를 나눔.
	// 투영 공간의 w = 뷰 공간의 z
	// 다음과 같이 설정하면 x,y는 w값으로 나눠져 확대, 축소가 일어나고,
	// z값은 w/w 즉 1이 나오게 되어 최대 깊이값이 설정이 된다.
	// 따라서 제일 멀리있는 상태가 된다.
	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP).xyww;
	output.vUV = input.vPos;

	return output;
}

struct PS_OUTPUT_SKY
{
	float4	vColor	:	SV_Target;	// 현재 타겟은 백버퍼
};


PS_OUTPUT_SKY SkyPS(VS_OUTPUT_SKY input)
{
	PS_OUTPUT_SKY output = (PS_OUTPUT_SKY)0;

	output.vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);
	
	return output;
}