Texture2D		g_DiffuseTex	: register(t0);
Texture2D		g_SpecularTex	: register(t1);
Texture2D		g_NormalMapTex	: register(t2);
SamplerState	g_DiffuseSmp	: register(s0);
SamplerState	g_PointSmp
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState	g_LinearSmp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_INPUT_BUMP
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};
struct VS_OUTPUT_BUMP
{
	float4	vPos	: SV_POSITION;
	float2	vUV		: TEXCOORD;
	float3	vViewPos	: POSITION;
	float3	vViewNormal	: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};

struct PS_OUTPUT
{
	float4	vColor	: SV_Target;
};

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

cbuffer LightInfo	: register(b2)
{
	float4	g_vLightDif;
	float4	g_vLightAmb;
	float4	g_vLightSpc;
	int		g_iLightType;
	float3	g_vLightEmpty;
}

cbuffer LightDir	: register(b3)
{
	float3	g_vLightDir;
	float	g_fLightDirEmpty;
}

cbuffer LightPoint	: register(b4)
{
	float3	g_vLightPos;
	float	g_fLightRange;
}



// 조명 연산 모듈
struct LightState
{
	float3	vDif;
	float3	vAmb;
	float3	vSpc;
};

LightState ComputeLight(float3 vViewPos, float3 vViewNormal, float2 vUV)
{
	LightState	tInfo = (LightState)0;

	float3	vLightDir = (float3)0.f;
	float	fAtt = 1.f;

	// Directional Light
	if (g_iLightType == 0)
	{
		vLightDir = -normalize(mul(float4(g_vLightDir, 0.f), g_matView).xyz);
	}

	// Point Light
	if (g_iLightType == 1)
	{
		// 조명의 위치를 뷰공간으로 변환한다.
		float3	vLightPos = mul(float4(g_vLightPos, 1.f), g_matView).xyz;
		vLightDir = vLightPos - vViewPos;

		float	fDist = length(vLightDir);

		if (fDist > g_fLightRange)
		{
			//output.vColor = float4(0.f, 0.f, 0.f, 1.f);
			return tInfo;
		}

		//fAtt = 1.f / (fDist * fDist);
		fAtt = 1.f - fDist / g_fLightRange;

		vLightDir = normalize(vLightDir);
		//vLightDir = normalize(mul(float4(g_vLightDir, 0.f), g_matView).xyz);
	}

	// Spot Light
	if (g_iLightType == 2)
	{
	}

	tInfo.vDif = g_vLightDif.xyz * g_vMtrlDif.xyz * max(0, dot(vLightDir, vViewNormal)) *
		fAtt;
	tInfo.vAmb = g_vLightAmb.xyz * g_vMtrlAmb.xyz;

	// 반사벡터를 구한다.
	float3	vR = 2.f * dot(vLightDir, vViewNormal) * vViewNormal -
		vLightDir;
	vR = normalize(vR);

	float3	vView = -normalize(vViewPos);

	// Specular Texture가 있을 경우
	if (g_vMtrlDif.w != 0.f)
	{
		float4	vSpcCol = g_SpecularTex.Sample(g_LinearSmp, vUV);
		tInfo.vSpc = g_vLightSpc.xyz * vSpcCol.xyz * pow(max(0, dot(vView, vR)), g_vMtrlSpc.w) *
			fAtt;
	}

	// 없을 경우
	else
	{
		tInfo.vSpc = g_vLightSpc.xyz * g_vMtrlSpc.xyz * pow(max(0, dot(vView, vR)), g_vMtrlSpc.w) *
			fAtt;
	}

	return tInfo;
}

float3 ComputeBumpNormal(float3 vTangent, float3 vBinormal, float3 vViewNormal, float2 vUV)
{
	float3	vNormal = vViewNormal;

	if (g_vMtrlAmb.w != 0.f)
	{
		float4	vNormalCol = g_NormalMapTex.Sample(g_PointSmp, vUV) * 2.f - 1.f;

		float3x3	matTangent =
		{
			vTangent,
			vBinormal,
			vViewNormal
		};

		vNormal = normalize(mul(vNormalCol.xyz, matTangent));
	}

	return vNormal;
}

VS_OUTPUT_BUMP DefaultBumpVS(VS_INPUT_BUMP input)
{
	VS_OUTPUT_BUMP	output = (VS_OUTPUT_BUMP)0;

	output.vPos = mul(float4(input.vPos, 1.f), g_matWVP);
	output.vUV = input.vUV;
	output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), g_matWV)).xyz;
	output.vViewPos = mul(float4(input.vPos, 1.f), g_matWV);

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

	float3	vNormal = ComputeBumpNormal(input.vTangent, input.vBinormal, input.vViewNormal,
		input.vUV);

	LightState	tLightInfo = ComputeLight(input.vViewPos, vNormal, input.vUV);

	//clip(-1.f);

	float4 vColor = g_DiffuseTex.Sample(g_DiffuseSmp, input.vUV);
	vColor.xyz = vColor.xyz * (tLightInfo.vDif.xyz + tLightInfo.vAmb.xyz) + tLightInfo.vSpc.xyz;
	output.vColor = vColor;

	return output;
}
