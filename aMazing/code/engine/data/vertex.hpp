#pragma once
#include<d3d11.h>
#include<D3DX11.h>
#include<xnamath.h>

namespace aMazing
{

class Vertex
{
public:
	Vertex()
	{
		position = { .0f, .0f, .0f };
		normal = { .0f, .0f, .0f }; 
		texture = {.0f,.0f}; 
	}
	Vertex(XMFLOAT3 pos,XMFLOAT3 nor,XMFLOAT2 tex):position(pos),
		normal(nor),
		texture(tex){}
	~Vertex(){}
	
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

	class SkinVertex 
	{
	public:
		SkinVertex()
		{
#ifdef BONE_UINT4_INDEX
			memset(boneIndices, 0, sizeof(boneIndices));
#else
			boneIndices = 0;
#endif
			weights = { 0, 0, 0, 0 };
		}

		void setBoneIndex(unsigned int slot,unsigned char value)
		{
#ifdef BONE_UINT4_INDEX
			boneIndices[slot] = value;
#else
			auto uiValue = (unsigned int)value << (slot * 8);
			switch (slot)
			{
			case 0:
				boneIndices &= 0xffffff00; break;
			case 1:
				boneIndices &= 0xffff00ff; break;
			case 2:
				boneIndices &= 0xff00ffff; break;
			case 3:
				boneIndices &= 0x00ffffff; break;
			}
			boneIndices |= uiValue;
#endif
		}
		XMFLOAT4 weights;
		XMFLOAT4 position;
		XMFLOAT4 normal;
		XMFLOAT4 texture;
#ifdef BONE_UINT4_INDEX
		unsigned int boneIndices[4];
#else
		unsigned int boneIndices;
#endif
	};
}
