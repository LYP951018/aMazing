#include"Buffers.hlsl"
interface iMateral
{
	//get the ambient color of material
	float3 getAmbientColor(float2 vTexcoord);
	//get the diffuse color of material
	float3 getDiffuseColor(flaot2 vTexcoord);
	//get the specular factor
	int getSpecularFactor();
};

class cBaseMaterial : iMaterial
{
	float3 vColor;
	int iSpecFactor;
	float3 getAmbientColor(flaot2 vTexcoord)
	{
		return vColor;
	}
	float3 getDiffuseColor(float2 vTexcoord)
	{
		return (float3)vColor;
	}
	int getSpecularFactor()
	{
		return iSpecFactor;
	}
};

class cSmoothMaterial : cBaseMaterial{};

class cSmoothTexturedMaterial : cSmoothMaterial
{
	float3 getAmbientColor(float2 vTexcoord);
	float3 getDiffuseColor(float2 vTexcoord);
};

class cSmoothLightingOnlyMaterial : cBaseMaterial
{
	float3 getAmbientColor(float2 vTexcoord)
	{
		return (float3)1.0f;
	}
	float3 getDiffuseColor(float2 vTexcoord)
	{
		return (float3)1.0f;
	}
};

class cRoughMaterial :cBaseMaterial
{
	int getSpecularFactor()
	{
		return iSpecFactor;
	}
};
