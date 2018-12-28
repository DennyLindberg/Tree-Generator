#pragma once
#include "opengl/texture.h"
#include "opengl/mesh.h"
#include <memory>

class Canvas2D
{
protected:
	bool bDirty = true;
	std::shared_ptr<GLQuad> quad;
	std::shared_ptr<GLTexture> texture;

public:
	Canvas2D();
	Canvas2D(GLQuadProperties properties);
	~Canvas2D() = default;

	std::shared_ptr<GLTexture> GetTexture()
	{
		return texture;
	}

	void Draw();

protected:
	void Initialize(GLQuadProperties properties);
};