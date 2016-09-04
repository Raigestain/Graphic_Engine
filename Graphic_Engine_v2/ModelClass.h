#pragma once

#ifndef _MODEL_CLASS
#define _MODEL_CLASS

/************************************************************************/
/* INCLUDES                                                             */
/************************************************************************/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class ModelClass
{
private:
	/*	Definition of the vertex type that will be used by the vertex buffer. 
		This one has to match the layout in the ColorShader class.			 */
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};
#endif

