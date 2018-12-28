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
	if (!canvasShader)
	{
		canvasShader = std::make_shared<GLTexturedProgram>();
	}

	quad = std::make_shared<GLQuad>(properties);
	texture = std::make_shared<GLTexture>(properties.width, properties.height);
}

/*
	Canvas drawing methods
*/
void Canvas2D::Fill(Color& color)
{
	bDirty = true;
	texture->Fill(color);
}

void Canvas2D::DrawLine(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, Color& color)
{
	bDirty = true;
	Line(*texture, float(startX), float(startY), float(endX), float(endY), color);
}