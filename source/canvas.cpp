#include "canvas.h"
#include "opengl/program.h"

#include "canvas/line.h"

std::shared_ptr<GLTexturedProgram> canvasShader;

Canvas2D::Canvas2D()
{
	GLQuadProperties properties;
	properties.MatchWindowDimensions();
	Initialize(properties);
}

Canvas2D::Canvas2D(GLQuadProperties properties)
{
	Initialize(properties);
}

void Canvas2D::RenderToScreen()
{
	if (bDirty)
	{
		bDirty = false;
		texture->CopyToGPU();
	}

	canvasShader->Use();
	texture->UseForDrawing();
	quad->Draw();
}

void Canvas2D::Initialize(GLQuadProperties properties)
{
	minX = int(properties.positionX);
	minY = int(properties.positionY);
	maxX = minX + int(properties.width);
	maxY = minY + int(properties.height);

	if (!canvasShader)
	{
		canvasShader = std::make_shared<GLTexturedProgram>();
	}

	quad = std::make_shared<GLQuad>(properties);
	texture = std::make_shared<GLTexture>(int(properties.width), int(properties.height));
}

/*
	Canvas drawing methods
*/
void Canvas2D::Fill(Color& color)
{
	bDirty = true;
	texture->Fill(color);
}

void Canvas2D::DrawLine(glm::fvec2 start, glm::fvec2 end, Color& color)
{
	bDirty = true;
	Line(*texture, start.x, start.y, end.x, end.y, color);
}