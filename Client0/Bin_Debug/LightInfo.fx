
struct LightInfo
{
	float4	vDif;		
	float4	vAmb;		
	float4	vSpc;			
	int		iLightType;		
	float3	vEyePos;				

	float4	vFogColor;
	float	fFogStart;
	float	fFogRange;
	float	fFogEmpty1;
	float	fFogEmpty2;
};

struct Material
{
	float4	vDif;
	float4	vAmb;
	float4	vSpc;
	float4	vEmiv;
};

struct	LightDir
{
	float3	vDir;			
	float	fEmpty;
};

struct LightPoint
{
	float3	vPos;
	float	fRange;
};

struct LightSpot
{
	float3	vPos;
	float	fRange;
	float3	vDir;
	float	fSpot;
};


// ���� ����

void ComputeDirectionalLight(Material M, LightInfo L, LightDir D, matrix vViewMatrix,
	float3 vViewNormal, float3 vViewPos, out float4 vDif, out float4 vAmb, out float4 vSpc)
{
	// �ʱ�ȭ
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// ������ ���� ���͸� ���Ѵ�. (�� ��ȯ���ְ� ���������� ����ȭ)
	float3	vLightDir = (float3)0.f;
	vLightDir = mul(float4(D.vDir, 0.f), vViewMatrix).xyz;
	vLightDir = -normalize(vLightDir);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = L.vDif * M.vDif * max(0, dot(vLightDir, vViewNormal));
	vAmb = L.vAmb * M.vAmb;
	vSpc = L.vSpc * M.vSpc * pow(max(0, dot(vView, vRef)), M.vSpc.w);
}

void ComputePointLight(Material M, LightInfo L, LightPoint P, matrix vViewMatrix,
	float3 vViewNormal, float3 vViewPos, out float4 vDif, out float4 vAmb, out float4 vSpc)
{
	// �ʱ�ȭ
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// ������ ��ġ�� ��������� ��ȯ.
	float3	vLightPos = mul(float4(P.vPos, 1.f), vViewMatrix).xyz;
	float3	vLightDir = (float3)0.f;

	// ������ ���⺤�͸� ���Ѵ�.
	vLightDir = vLightPos - vViewPos;

	// ������ �Ѿ�� 0�� ����
	float	fDist = length(vLightDir);
	if (fDist > P.fRange)
	{
		return;
	}
	// ���̰��� ���������� �ٷ� ����ȭ
	vLightDir = normalize(vLightDir);

	// ���� ������ ����
	float fAtt = 1.f - (fDist / P.fRange);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = L.vDif * M.vDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	vAmb = L.vAmb * M.vAmb;
	vSpc = L.vSpc * M.vSpc * pow(max(0, dot(vView, vRef)), M.vSpc.w) * fAtt;
}


void ComputeSpotLight(Material M, LightInfo L, LightSpot S, matrix vViewMatrix,
	float3 vViewNormal, float3 vViewPos, out float4 vDif, out float4 vAmb, out float4 vSpc)
{
	// �ʱ�ȭ
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// ������ ��ġ�� �� ��ȯ
	float3 vLightPos = mul(float4(S.vPos, 1.f), vViewMatrix).xyz;
	float3 vLightDir = (float3)0.f;

	// ������ ���⺤�͸� ���Ѵ�.
	vLightDir = vLightPos - vViewPos;

	// ������ �Ѿ�� 0�� ����
	float	fDist = length(vLightDir);
	if (fDist > S.fRange)
	{
		return;
	}

	vLightDir = normalize(vLightDir);

	// ���� ������ ����
	float fSpot = pow(max(0, dot(vLightDir, S.vDir)), S.fSpot);
	float fAtt = fSpot / (fDist * fDist);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = L.vDif * M.vDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	vAmb = L.vAmb * M.vAmb * fSpot;
	vSpc = L.vSpc * M.vSpc * pow(max(0, dot(vView, vRef)), M.vSpc.w) * fAtt;

}