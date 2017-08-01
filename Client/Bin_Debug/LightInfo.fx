
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


// 조명 연산

void ComputeDirectionalLight(Material M, LightInfo L, LightDir D, matrix vViewMatrix,
	float3 vViewNormal, float3 vViewPos, out float4 vDif, out float4 vAmb, out float4 vSpc)
{
	// 초기화
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// 조명의 방향 벡터를 구한다. (뷰 변환해주고 역방향으로 정규화)
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
	// 초기화
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// 조명의 위치를 뷰공간으로 변환.
	float3	vLightPos = mul(float4(P.vPos, 1.f), vViewMatrix).xyz;
	float3	vLightDir = (float3)0.f;

	// 조명의 방향벡터를 구한다.
	vLightDir = vLightPos - vViewPos;

	// 범위를 넘어가면 0을 리턴
	float	fDist = length(vLightDir);
	if (fDist > P.fRange)
	{
		return;
	}
	// 길이값을 저장했으니 바로 정규화
	vLightDir = normalize(vLightDir);

	// 감쇠 범위를 지정
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
	// 초기화
	vDif = (float4)0.f;
	vAmb = (float4)0.f;
	vSpc = (float4)0.f;

	// 조명의 위치를 뷰 변환
	float3 vLightPos = mul(float4(S.vPos, 1.f), vViewMatrix).xyz;
	float3 vLightDir = (float3)0.f;

	// 조명의 방향벡터를 구한다.
	vLightDir = vLightPos - vViewPos;

	// 범위를 넘어가면 0을 리턴
	float	fDist = length(vLightDir);
	if (fDist > S.fRange)
	{
		return;
	}

	vLightDir = normalize(vLightDir);

	// 감쇠 범위를 지정
	float fSpot = pow(max(0, dot(vLightDir, S.vDir)), S.fSpot);
	float fAtt = fSpot / (fDist * fDist);

	float3	vRef = reflect(-vLightDir, vViewNormal);
	float3	vView = -normalize(vViewPos);

	vDif = L.vDif * M.vDif * max(0, dot(vLightDir, vViewNormal)) * fAtt;
	vAmb = L.vAmb * M.vAmb * fSpot;
	vSpc = L.vSpc * M.vSpc * pow(max(0, dot(vView, vRef)), M.vSpc.w) * fAtt;

}