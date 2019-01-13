#include "canvas.h"
#include "opengl/program.h"

#include "canvas/line.h"
#include "core/utilities.h"
#include "core/application.h"

std::shared_ptr<GLProgram> canvasShader;

Canvas2D::Canvas2D()
{
	GLQuadProperties properties;
	properties.MatchWindowDimensions();
	Initialize(properties);
}

Canvas2D::Canvas2D(int width, int height)
{
	Initialize(GLQuadProperties{ 0.0f, 0.0f, float(width), float(height) });
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
		ApplicationSettings s = GetApplicationSettings();

		canvasShader = std::make_shared<GLProgram>();
		std::string fragment, vertex;
		if (LoadText(s.contentPath/"basic_fragment.glsl", fragment) && LoadText(s.contentPath/"basic_vertex.glsl", vertex))
		{
			canvasShader->LoadFragmentShader(fragment);
			canvasShader->LoadVertexShader(vertex);
			canvasShader->CompileAndLink();
		}
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