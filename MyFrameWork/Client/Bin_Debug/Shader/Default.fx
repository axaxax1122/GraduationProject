
// 텍스처, 샘플러 레지스터 설정
Texture2D		g_DiffuseTex	: register(t0);
SamplerState	g_DiffuseSmp	: register(s0);


// 상수 버퍼 구조체
cbuffer	Transform	: register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matWV;
	matrix	g_matWVP;
};


struct VS_INPUT_COLOR
{
	float3	vPos	: POSITION;
	float4	vColor	: COLOR;
};

struct VS_OUTPUT_COLOR
{
	float4	vPos	: SV_POSITION;
	float4	vColor	: COLOR;
};

VS_OUTPUT_COLOR ColorVS(VS_INPUT_COLOR input)
{
	VS_OUTPUT_COLOR	output = (VS_OUTPUT_COLOR)0;

	// xyz는 input에 들어온vPos의 xyz로 설정되고 w가 1.f로 설정된다.
	// output.vPos = float4(input.vPos, 1.f);
	// 정점의 위치에 월드 * 뷰 * 투영을 한 행렬을 곱해서 모든 정점을 변환
	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vColor = input.vColor;

	return output;
}

struct PS_OUTPUT
{
	float4	vColor	: SV_Target;
};

// 입력으로 들어온 SV_POSITION 속성은 절대 사용해선 안된다.
PS_OUTPUT ColorPS(VS_OUTPUT_COLOR input)
{
	PS_OUTPUT	output = (PS_OUTPUT)0;

	output.vColor = input.vColor;
	//output.vColor = float4(1.f, 1.f, 0.f, 1.f);

	return output;
}


// ========================== Texture ==============================

struct VS_INPUT_TEX
{
	float3 vPos		: POSITION;
	float3 vNormal	: NORMAL;
	float2 vUV		: TEXCOORD;
};

struct VS_OUTPUT_TEX
{
	float4 vPos		: SV_POSITION;
	float2 vUV		: TEXCOORD;
};

VS_OUTPUT_TEX DefaultTexVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;

	return output;
}

PS_OUTPUT DefaultTexPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);

	return output;
}