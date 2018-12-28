#include "examples.h"

void DrawFractalTree(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString fractalTree;
	fractalTree.axiom = "0";
	fractalTree.productionRules['0'] = "1[0]0";
	fractalTree.productionRules['1'] = "11";

	Turtle2D turtle;
	turtle.actions['0'] = [scale](Turtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.position + t.GetDirection() * scale;
		c.DrawLine(t.position, newPosition, Color{ 0,0,0,255 });
		t.position = newPosition;
	};
	turtle.actions['1'] = turtle.actions['0'];
	turtle.actions['['] = [scale](Turtle2D& t, Canvas2D& c) {
		t.PushState();
		t.Rotate(45.0f);
	};
	turtle.actions[']'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.PopState();
		t.Rotate(-45.0f);
	};

	turtle.Draw(
		canvas, 
		fractalTree.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawKochCurve(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString kochCurve;
	kochCurve.axiom = "F";
	kochCurve.productionRules['F'] = "F+F-F-F+F";

	Turtle2D turtle;
	turtle.actions['F'] = [scale](Turtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.position + t.GetDirection() * scale;
		c.DrawLine(t.position, newPosition, Color{ 0,0,0,255 });
		t.position = newPosition;
	};
	turtle.actions['+'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(90.0f);
	};
	turtle.actions['-'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(-90.0f);
	};

	turtle.Draw(
		canvas,
		kochCurve.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawSierpinskiTriangle(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString sierpinskiTriangle;
	sierpinskiTriangle.axiom = "F-G-G";
	sierpinskiTriangle.productionRules['F'] = "F-G+F+G-F";
	sierpinskiTriangle.productionRules['G'] = "GG";

	Turtle2D turtle;
	turtle.actions['F'] = [scale](Turtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.position + t.GetDirection() * scale;
		c.DrawLine(t.position, newPosition, Color{ 0,0,0,255 });
		t.position = newPosition;
	};
	turtle.actions['G'] = turtle.actions['F'];
	turtle.actions['+'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(120.0f);
	};
	turtle.actions['-'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(-120.0f);
	};

	turtle.Draw(
		canvas,
		sierpinskiTriangle.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawDragonCurve(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString dragonCurve;
	dragonCurve.axiom = "FX";
	dragonCurve.productionRules['X'] = "X+YF+";
	dragonCurve.productionRules['Y'] = "-FX-Y";

	Turtle2D turtle;
	turtle.actions['F'] = [scale](Turtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.position + t.GetDirection() * scale;
		c.DrawLine(t.position, newPosition, Color{ 0,0,0,255 });
		t.position = newPosition;
	};
	turtle.actions['+'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(-90.0f);
	};
	turtle.actions['-'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(90.0f);
	};

	turtle.Draw(
		canvas,
		dragonCurve.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawFractalPlant(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString fractalPlant;
	fractalPlant.axiom = "X";
	fractalPlant.productionRules['X'] = "F+[[X]-X]-F[-FX]+X";
	fractalPlant.productionRules['F'] = "FF";

	Turtle2D turtle;
	turtle.actions['F'] = [scale](Turtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.position + t.GetDirection() * scale;
		c.DrawLine(t.position, newPosition, Color{ 0,0,0,255 });
		t.position = newPosition;
	};
	turtle.actions['+'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(-25.0f);
	};
	turtle.actions['-'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.Rotate(25.0f);
	};
	turtle.actions['['] = [scale](Turtle2D& t, Canvas2D& c) {
		t.PushState();
	};
	turtle.actions[']'] = [scale](Turtle2D& t, Canvas2D& c) {
		t.PopState();
	};

	turtle.Draw(
		canvas,
		fractalPlant.RunProduction(iterations),
		origin,
		startAngle
	);
}