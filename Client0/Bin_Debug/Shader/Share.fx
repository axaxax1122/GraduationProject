
// Transform 
cbuffer	Transform	: register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matWV;
	matrix	g_matWVP;
};

SamplerState	g_DiffuseSmp	: register(s0);
SamplerState	g_PointSmp		: register(s1);