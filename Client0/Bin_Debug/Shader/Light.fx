
// �ؽ�ó �������� ����
Texture2D		g_DiffuseTex	: register(t0);
Texture2D		g_SpecularTex	: register(t1);
Texture2D		g_NormalMapTex	: register(t2);
Texture2D		g_BoneTex		: register(t3);


// ��� ���� ����ü //
// Material
cbuffer Material	: register(b1)
{
	float4	g_vMtrlDif;
	float4	g_vMtrlAmb;
	float4	g_vMtrlSpc;
	float4	g_vMtrlEmv;
	float	g_fFadeAmount;
	float3	g_vMtrlEmpty;
}

// LightInfo
cbuffer LightInfo	: register(b2)	// 16����Ʈ(128bit)�� �����־����
{
	float4	g_vLightDif;			// 16����Ʈ
	float4	g_vLightAmb;			// 16����Ʈ
	float4	g_vLightSpc;			// 16����Ʈ
	int		g_iLightType;			// 4����Ʈ
	float3	g_vLightEmpty;			// 12����Ʈ
}

// �𷺼ų� ����Ʈ
cbuffer	LightDir	: register(b3)
{
	float3	g_vLightDir;			// 12����Ʈ
	float	g_fLightDirEmpty;		// 4����Ʈ
}

// ����Ʈ ����Ʈ
cbuffer LightPoint	: register(b4)
{
	float3	g_vLightPos;
	float	g_fLightRange;
}


cbuffer	FogInfo		: register(b5)
{
	float4	g_vFogColor;
	float	g_vEyePos;
	float3	g_fFogStart;
	float	g_fFogRange;
	float3	g_fFogEmpty;
}

cbuffer Bone		: register(b6)
{
	matrix	g_matBones[90];
	int		g_iBoneCount;
	float3	g_vBoneEmpty;
};


// ���� ����

void ComputeDirectionalLight(float3 vViewNormal, float3 vViewPos, float2 vUV, out float4 vDif, out float4 vAmb, out float4 vSpc)
{

	// ������ ���� ���͸� ���Ѵ�. (�� ��ȯ���ְ� ���������� ����ȭ)
	float3	vLightDir = (float3)0.f;
	vLightDir = mul(float4(g_vLightDir, 0.f), g_matView).xyz;
	vLightDir = -normalize(vLightDir);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = g_vLightDif * g_vMtrlDif * max(0, dot(vLightDir, vViewNormal));
	vAmb = g_vLightAmb * g_vMtrlAmb;
	
	// Specualr Texture
	if (g_vMtrlDif.w != 0.f)
	{
		float4	vSpcColor = g_SpecularTex.Sample(g_DiffuseSmp, vUV);
		vSpc = g_vLightSpc * vSpcColor * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w);
	}
	else
	{
		vSpc = g_vLightSpc * g_vMtrlSpc * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w);
	}
}

void ComputePointLight(float3 vViewNormal, float3 vViewPos, float2 vUV, out float4 vDif, out float4 vAmb, out float4 vSpc)
{


	// ������ ��ġ�� ��������� ��ȯ.
	float3	vLightPos = mul(float4(g_vLightPos, 1.f), g_matView).xyz;
	float3	vLightDir = (float3)0.f;

	// ������ ���⺤�͸� ���Ѵ�.
	vLightDir = vLightPos - vViewPos;

	// ������ �Ѿ�� 0�� ����
	float	fDist = length(vLightDir);
	if (fDist > g_fLightRange)
	{
		return;
	}
	// ���̰��� ���������� �ٷ� ����ȭ
	vLightDir = normalize(vLightDir);

	// ���� ������ ����
	float fAtt = 1.f - (fDist / g_fLightRange);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = g_vLightDif * g_vMtrlDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	vAmb = g_vLightAmb * g_vMtrlAmb;

	// Specualr Texture
	if (g_vMtrlDif.w != 0.f)
	{
		float4	vSpcColor = g_SpecularTex.Sample(g_DiffuseSmp, vUV);
		vSpc = g_vLightSpc * vSpcColor * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w) * fAtt;
	}
	else
	{
		vSpc = g_vLightSpc * g_vMtrlSpc * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w) * fAtt;
	}
}



void ComputeLight(float3 vViewNormal, float3 vViewPos, float2 vUV, out float4 vDif, out float4 vAmb, out float4 vSpc)
{
	// �ʱ�ȭ
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	//	Directional Light
	if (g_iLightType == 0)
		ComputeDirectionalLight(vViewNormal, vViewPos, vUV, vDif, vAmb, vSpc);

	//	Point Light
	if (g_iLightType == 1)
		ComputePointLight(vViewNormal, vViewPos, vUV, vDif, vAmb, vSpc);

	//	Spot Light
//	if (g_iLightType == 2)
}

float3 ComputeBumpNormal(float3 vTangent, float3 vBinormal, float3 vViewNormal, float2 vUV)
{
	float3 vNormal = vViewNormal;
	float4 vNormalColor = g_NormalMapTex.Sample(g_PointSmp, vUV) * 2.f - 1.f;

	float3x3	matTangent =
	{
		vTangent,
		vBinormal,
		vViewNormal
	};

	vNormal = normalize(mul(vNormalColor.xyz, matTangent));
	return vNormal;
}


struct VS_INPUT_BUMP_ANIM
{
	float3	vPos	: POSITION;
	float3	vNormal	: NORMAL;
	float2	vUV		: TEXCOORD;
	float3	vTangent: TANGENT;
	float3	vBinormal: BINORMAL;
	float4	vWeights : WEIGHTS;
	float4	vIndices : BLENDINDICES;
};

struct _tagSkinningInfo
{
	float3	vPos;
	float3	vNormal;
	float3	vTangent;
	float3	vBinormal;
};


matrix GetBoneMatrix(float idx)
{
	matrix	matBone =
	{
		g_BoneTex.SampleLevel(g_PointSmp, float4((idx * 4 + 0.5f) / (g_iBoneCount * 4), 0.f, 0.f, 0.f), 0),
		g_BoneTex.SampleLevel(g_PointSmp, float4((idx * 4 + 1 + 0.5f) / (g_iBoneCount * 4), 0.f, 0.f, 0.f), 0),
		g_BoneTex.SampleLevel(g_PointSmp, float4((idx * 4 + 2 + 0.5f) / (g_iBoneCount * 4), 0.f, 0.f, 0.f), 0),
		g_BoneTex.SampleLevel(g_PointSmp, float4((idx * 4 + 3 + 0.5f) / (g_iBoneCount * 4), 0.f, 0.f, 0.f), 0)
	};

	return matBone;
}

_tagSkinningInfo SkinningBump(float3 vPos, float3 vNormal,
	float3 vTangent, float3 vBinormal, float4 vWeights, float4 vIndices)
{
	_tagSkinningInfo	tInfo = (_tagSkinningInfo)0;

	// ����ġ �迭�� �����.
	float	fWeights[4];
	fWeights[0] = vWeights.x;
	fWeights[1] = vWeights.y;
	fWeights[2] = vWeights.z;
//	fWeights[3] = 1.f - fWeights[0] - fWeights[1] - fWeights[2];
	fWeights[3] = vWeights.w;

	for (int i = 0; i < 4; ++i)
	{
		matrix	matBone = (matrix)0;

		if (g_iBoneCount > 90)
		{
			matBone = GetBoneMatrix(vIndices[i]);
		}
		else
		{
			matBone = g_matBones[vIndices[i]];
		}


		tInfo.vPos += fWeights[i] * mul(float4(vPos, 1.f),
			matBone).xyz;
		tInfo.vNormal += fWeights[i] * mul(float4(vNormal, 0.f),
			matBone).xyz;
		tInfo.vTangent += fWeights[i] * mul(float4(vTangent, 0.f),
			matBone).xyz;
		tInfo.vBinormal += fWeights[i] * mul(float4(vBinormal, 0.f),
			matBone).xyz;
	}

	tInfo.vNormal = normalize(tInfo.vNormal);
	tInfo.vTangent = normalize(tInfo.vTangent);
	tInfo.vBinormal = normalize(tInfo.vBinormal);

	return tInfo;
}