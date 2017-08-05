
#include "Share.fx"
#include "Light.fx"


struct VS_INPUT_COLOR
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float4	vColor	: COLOR;
};

struct VS_OUTPUT_COLOR
{
	float4	vPos	: SV_POSITION;
	float4	vColor	: COLOR;
	float3	vViewPos	: POSITION;
	float3	vViewNormal : NORMAL;
};

VS_OUTPUT_COLOR ColorVS(VS_INPUT_COLOR input)
{
	VS_OUTPUT_COLOR	output = (VS_OUTPUT_COLOR)0;

	// 정점의 위치에 월드 * 뷰 * 투영을 한 행렬을 곱해서 모든 정점을 변환
	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vColor = input.vColor;

	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;
	output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWV)).xyz;

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
	
	float4 vDif, vAmb, vSpc;
	float2 vUV = (float2)0.f;
	ComputeLight(input.vViewNormal, input.vViewPos, vUV, vDif, vAmb, vSpc);

	float4 vColor = input.vColor;
	vColor.xyz = vColor.xyz * (vDif.xyz + vAmb.xyz) + vSpc.xyz;
	vColor.a = vColor.a;
	
	output.vColor = vColor * g_fFadeAmount;
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
	float3 vViewPos		: POSITION;
	float3 vViewNormal	: NORMAL;
};

VS_OUTPUT_TEX DefaultTexVS(VS_INPUT_TEX input)
{
	VS_OUTPUT_TEX	output = (VS_OUTPUT_TEX)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;

	//	뷰 변환 까지만 해준다.
	output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWV)).xyz;
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;

	return output;
}

PS_OUTPUT DefaultTexPS(VS_OUTPUT_TEX input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 vTexColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);
	//	clip(vTexColor.a - 0.1f);

	float4	vDif, vAmb, vSpc;
	ComputeLight(input.vViewNormal, input.vViewPos, input.vUV, vDif, vAmb, vSpc);

	//	float4 vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);
	float4 vColor = vTexColor;
	vColor.xyz = vColor.xyz * (vDif.xyz + vAmb.xyz) + vSpc.xyz;

	//--안개--
	float fogLerp = saturate((g_vEyePos) / g_fFogRange);
	vColor = lerp(vColor, g_vFogColor, fogLerp);
	vColor.a = g_vLightDif.a * vColor.a;

	output.vColor = vColor * g_fFadeAmount;
	return output;
}

// ========================== Bump ==============================

struct VS_INPUT_BUMP
{
	float3	vPos	 : POSITION;
	float3	vNormal  : NORMAL;
	float2	vUV		 : TEXCOORD;
	float3	vTangent : TANGENT;
	float3	vBinormal: BINORMAL;
};

struct VS_OUTPUT_BUMP
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
	float3	vViewPos: POSITION;
	float3	vViewNormal : NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};

VS_OUTPUT_BUMP DefaultBumpVS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;
	output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWV)).xyz;
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV).xyz;

	// NormalMap Texture가 있는지 식별
	if (g_vMtrlAmb.w != 0.f)
	{
		output.vTangent = normalize(mul(float4(input.vTangent, 0.f), g_matWV)).xyz;
		output.vBinormal = normalize(mul(float4(input.vBinormal, 0.f), g_matWV)).xyz;
	}

	return output;
}

PS_OUTPUT DefaultBumpPS(VS_OUTPUT_BUMP input)
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 vTexColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);

	float3 vNormal = input.vViewNormal;
	if (g_vMtrlAmb.w != 0.f)
	{
		vNormal = ComputeBumpNormal(input.vTangent, input.vBinormal, input.vViewNormal, input.vUV);
	}

	float4	vDif, vAmb, vSpc;
	ComputeLight(vNormal, input.vViewPos, input.vUV, vDif, vAmb, vSpc);


	float4 vColor = vTexColor;
	vColor.xyz = vColor.xyz * (vDif.xyz + vAmb.xyz) + vSpc.xyz;

	//--안개--
	float fogLerp = saturate((g_vEyePos) / g_fFogRange);
	vColor = lerp(vColor, g_vFogColor, fogLerp);
	vColor.a = g_vLightDif.a * vColor.a;

	output.vColor = vColor * g_fFadeAmount;
	//output.vColor = vColor;

	return output;
}

// ========================== Animation ==============================

VS_OUTPUT_BUMP DefaultAnimationVS(VS_INPUT_BUMP_ANIM input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	_tagSkinningInfo	tSkinning = (_tagSkinningInfo)0;

	tSkinning = SkinningBump(input.vPos, input.vNormal,
		input.vTangent, input.vBinormal, input.vWeights,
		input.vIndices);

	output.vPos = mul(float4(tSkinning.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;

	// 조명 처리를 위한 정보
	output.vViewNormal = normalize(mul(float4(tSkinning.vNormal, 0.f), g_matWV));
	output.vViewPos = mul(float4(tSkinning.vPos, 1.f), g_matWV);

	// NormalMap Texture가 있는지 식별
	if (g_vMtrlAmb.w != 0.f)
	{
		output.vTangent = normalize(mul(float4(tSkinning.vTangent, 0.f), g_matWV));
		output.vBinormal = normalize(mul(float4(tSkinning.vBinormal, 0.f), g_matWV));
	}

	return output;
}