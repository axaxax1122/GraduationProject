
// �ؽ�ó, ���÷� �������� ����
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

// ����Ʈ ����Ʈ
cbuffer LightSpot	: register(b5)
{
	float3	g_vSpotPos;
	float	g_fSpotRange;
	float3	g_vSpotDir;
	float	g_fSpotSpot;
}

cbuffer	FogInfo		: register(b6)
{
	float4	g_vFogColor;
	float	g_fFogStart;
	float3	g_vEyePos;
	float	g_fFogRange;
	float3	g_fFogEmpty;
}


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
		float4	vSpcColor = g_SpecularTex.Sample(g_LinearSmp, vUV);
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
		float4	vSpcColor = g_SpecularTex.Sample(g_LinearSmp, vUV);
		vSpc = g_vLightSpc * vSpcColor * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w) * fAtt;
	}
	else
	{
		vSpc = g_vLightSpc * g_vMtrlSpc * pow(max(0, dot(vView, vRef)), g_vMtrlSpc.w) * fAtt;
	}
}


void ComputeSpotLight(float3 vViewNormal, float3 vViewPos, float2 vUV, out float4 vDif, out float4 vAmb, out float4 vSpc)
{


	// ������ ��ġ�� �� ��ȯ
	float3 vLightPos = mul(float4(g_vLightPos, 1.f), g_matView).xyz;
	float3 vLightDir = (float3)0.f;

	// ������ ���⺤�͸� ���Ѵ�.
	vLightDir = vLightPos - vViewPos;

	// ������ �Ѿ�� 0�� ����
	float	fDist = length(vLightDir);
	if (fDist > g_fSpotRange)
	{
		return;
	}

	vLightDir = normalize(vLightDir);

	// ���� ������ ����
	float fSpot = pow(max(0, dot(vLightDir, g_vSpotDir)), g_fSpotSpot);
	float fAtt = fSpot / (fDist * fDist);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = g_vLightDif * g_vMtrlDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	vAmb = g_vLightAmb * g_vMtrlAmb * fSpot;
	
	
	// Specualr Texture
	if (g_vMtrlDif.w != 0.f)
	{
		float4	vSpcColor = g_SpecularTex.Sample(g_LinearSmp, vUV);
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
	if (g_iLightType == 2)
		ComputeSpotLight(vViewNormal, vViewPos, vUV, vDif, vAmb, vSpc);
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