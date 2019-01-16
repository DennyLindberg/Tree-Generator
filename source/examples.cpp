#include "examples.h"
#include "thirdparty/glmGeom.h"

using BasicTurtle2D = Turtle2D<>;

void DrawFractalTree(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString fractalTree;
	fractalTree.axiom = "0";
	fractalTree.productionRules['0'] = "1[0]0";
	fractalTree.productionRules['1'] = "11";

	BasicTurtle2D turtle;
	turtle.actions['0'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['1'] = turtle.actions['0'];
	turtle.actions['['] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		t.PushState();
		t.Rotate(45.0f);
	};
	turtle.actions[']'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
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

	BasicTurtle2D turtle;
	turtle.actions['F'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(90.0f); };
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-90.0f); };

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

	BasicTurtle2D turtle;
	turtle.actions['F'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['G'] = turtle.actions['F'];
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(120.0f); };
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-120.0f); };

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

	BasicTurtle2D turtle;
	turtle.actions['F'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-90.0f); };
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(90.0f); };

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

	BasicTurtle2D turtle;
	turtle.actions['F'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-25.0f); };
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(25.0f); };
	turtle.actions['['] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PushState(); };
	turtle.actions[']'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PopState(); };

	turtle.Draw(
		canvas,
		fractalPlant.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawFractalTreeNezumiV1(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	// https://lazynezumi.com/lsystems

	LSystemString fractalTreeNezumi;
	fractalTreeNezumi.axiom = "[B]";
	fractalTreeNezumi.productionRules['B'] = "A[-B][+B]";

	BasicTurtle2D turtle;
	turtle.actions['A'] = [scale](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-20.0f); };
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(20.0f); };
	turtle.actions['['] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PushState(); };
	turtle.actions[']'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PopState(); };

	turtle.Draw(
		canvas,
		fractalTreeNezumi.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawFractalTreeNezumiV2(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	// https://lazynezumi.com/lsystems

	LSystemString fractalTreeNezumi;
	fractalTreeNezumi.axiom = "[B]";
	fractalTreeNezumi.productionRules['B'] = "A[!%-B][!%+B]!%AB";

	struct NezumiProps
	{
		float lengthFactor = 1.0f;
	};
	using NezumiTurtle = Turtle2D<NezumiProps>;

	NezumiTurtle turtle;
	turtle.actions['A'] = [scale](NezumiTurtle& t, Canvas2D& c)
	{
		NezumiProps& p = t.state.properties;
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale * p.lengthFactor;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['%'] = [](NezumiTurtle& t, Canvas2D& c)
	{
		NezumiProps& p = t.state.properties;
		p.lengthFactor /= 1.3f;
	};
	turtle.actions['-'] = [](NezumiTurtle& t, Canvas2D& c) { t.Rotate(-20.0f); };
	turtle.actions['+'] = [](NezumiTurtle& t, Canvas2D& c) { t.Rotate(20.0f); };
	turtle.actions['['] = [](NezumiTurtle& t, Canvas2D& c) { t.PushState(); };
	turtle.actions[']'] = [](NezumiTurtle& t, Canvas2D& c) { t.PopState(); };

	turtle.Draw(
		canvas,
		fractalTreeNezumi.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawFractalTreeNezumiV3(Canvas2D& canvas, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	// https://lazynezumi.com/lsystems

	UniformRandomGenerator uniformGenerator;
	LSystemString fractalTreeNezumi;
	fractalTreeNezumi.axiom = "[B]";
	fractalTreeNezumi.productionRules['B'] = "A[!%-B][!%+B]!%AB";

	bool skipBranch = false;

	struct NezumiProps
	{
		float lengthFactor = 1.0f;
	};
	using NezumiTurtle = Turtle2D<NezumiProps>;
	NezumiTurtle turtle;

	turtle.actions['A'] = [scale, &skipBranch, &uniformGenerator](NezumiTurtle& t, Canvas2D& c)
	{
		if (skipBranch) return;

		NezumiProps& p = t.state.properties;
		float randomLengthFactor = 1.0f + uniformGenerator.RandomFloat(0.0f, 0.15f);
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale * p.lengthFactor * randomLengthFactor;
		c.DrawLine(t.state.position, newPosition, Color{ 0,0,0,255 });
		t.state.position = newPosition;
	};
	turtle.actions['%'] = [&skipBranch](NezumiTurtle& t, Canvas2D& c)
	{
		if (skipBranch) return;

		NezumiProps& p = t.state.properties;
		p.lengthFactor /= 1.6f;
	};
	turtle.actions['-'] = [&skipBranch, &uniformGenerator](NezumiTurtle& t, Canvas2D& c)
	{ 
		if (skipBranch) return;

		t.Rotate(-20.0f + uniformGenerator.RandomFloat(-5.0f, 5.0f));
	};
	turtle.actions['+'] = [&skipBranch, &uniformGenerator](NezumiTurtle& t, Canvas2D& c)
	{ 
		if (skipBranch) return;

		t.Rotate(20.0f + uniformGenerator.RandomFloat(-5.0f, 5.0f)); 
	};
	turtle.actions['['] = [&skipBranch, &uniformGenerator](NezumiTurtle& t, Canvas2D& c)
	{ 
		skipBranch = uniformGenerator.RandomFloat() > 0.8;
		t.PushState(); 
	};
	turtle.actions[']'] = [&skipBranch](NezumiTurtle& t, Canvas2D& c)
	{ 
		t.PopState(); 
		skipBranch = false;
	};

	turtle.Draw(
		canvas,
		fractalTreeNezumi.RunProduction(iterations),
		origin,
		startAngle
	);
}

void DrawFractalLeaf(std::vector<glm::fvec3>& generatedHull, Canvas2D& canvas, Color color, int iterations, float scale, glm::fvec2 origin, float startAngle)
{
	LSystemString fractalLeaf;
	fractalLeaf.axiom = "0";
	fractalLeaf.productionRules['0'] = "1[-0][+0]1e";
	fractalLeaf.productionRules['1'] = "11";
	fractalLeaf.productionRules['e'] = fractalLeaf.productionRules['0'];

	BasicTurtle2D turtle;
	std::vector<glm::fvec3> leafPositions{ glm::fvec3{origin, 1.0f} };
	turtle.actions['0'] = [scale, &color](BasicTurtle2D& t, Canvas2D& c) {
		glm::fvec2 newPosition = t.state.position + t.GetDirection() * scale;
		c.DrawLine(t.state.position, newPosition, color);
		t.state.position = newPosition;
	};
	turtle.actions['1'] = turtle.actions['0'];
	turtle.actions['e'] = [scale, &leafPositions](BasicTurtle2D& t, Canvas2D& c) {
		leafPositions.push_back(glm::fvec3(t.state.position, 0.0f));
	};

	turtle.actions['['] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PushState(); };
	turtle.actions[']'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.PopState();  };
	turtle.actions['+'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(45.0f); };
	turtle.actions['-'] = [scale](BasicTurtle2D& t, Canvas2D& c) { t.Rotate(-45.0f); };

	turtle.Draw(
		canvas,
		fractalLeaf.RunProduction(iterations),
		origin,
		startAngle
	);

	generatedHull = getConvexHull(leafPositions);
}








void BuildBranchesForFractalTree3D(std::vector<FractalBranch>& branches, Bone<FractalTree3DProps>* bone)
{
	/*
		A lastChild is considered a continuation of the same branch for the FractalTree3D. 
		If there is more than one child in a node, it means there is a new branch. 
		This method traverses the nodes and builds branches for each lastChild chain.
	*/
	using TBone = Bone<FractalTree3DProps>;
	using BoneVector = std::vector<TBone*>;

	branches.clear();
	branches.shrink_to_fit();
	branches.push_back(FractalBranch{ bone });

	int activeIndex = 0;
	while (activeIndex < branches.size())
	{
		FractalBranch& currentBranch = branches[activeIndex];
		TBone* firstBone = currentBranch.nodes[0];

		// Build branch from chain of lastChild's
		BoneVector potentialBranchingPoints{};
		TBone* lastChild = firstBone->lastChild;
		while (lastChild)
		{
			currentBranch.Push(lastChild);
			potentialBranchingPoints.push_back(lastChild);
			lastChild = lastChild->lastChild;
		}

		// For each previous lastChild, check if there are siblings.
		// Whenever there is a sibling, it is a new branch.
		for (TBone* p : potentialBranchingPoints)
		{
			TBone* sibling = p->previousSibling;
			while (sibling)
			{
				branches.push_back(FractalBranch{ sibling });
				sibling = sibling->previousSibling;
			}
		}

		activeIndex++;
	}
}

void GenerateFractalTree3D(UniformRandomGenerator& uniformGenerator, int iterations, std::function<void(Bone<FractalTree3DProps>*, std::vector<FractalBranch>&)> onResultCallback)
{
	// https://lazynezumi.com/lsystems
	LSystemString fractalTree;
	fractalTree.axiom = "B";
	fractalTree.productionRules['B'] = "AC";
	fractalTree.productionRules['C'] = "AA[%+B][%++B][%+++B]%B";

	using Turtle = Turtle3D<FractalTree3DProps>;
	Turtle turtle;

	turtle.actions['A'] = [&uniformGenerator](Turtle& t, int repetitions)
	{
		FractalTree3DProps& p = t.transform.properties;
		t.Rotate(
			uniformGenerator.RandomFloat(0.0f, 45.0f),
			uniformGenerator.RandomFloat(-10.0f, 10.0f)
		);
		p.depth++;
		p.thickness *= 0.75;

		float randomLengthFactor = 1.0f + uniformGenerator.RandomFloat(0.0f, 0.5f);
		t.MoveForward(p.lengthFactor * randomLengthFactor);

	};
	turtle.actions['C'] = turtle.actions['A'];
	turtle.actions['%'] = [](Turtle& t, int repetitions) { t.transform.properties.lengthFactor /= 1.2f; };
	turtle.actions['['] = [&uniformGenerator](Turtle& t, int repetitions)
	{
		t.PushState();
		t.transform.properties.thickness *= 0.75f;
	};
	turtle.actions[']'] = [](Turtle& t, int repetitions) { t.PopState(); };

	turtle.actions['+'] = [&uniformGenerator, &iterations](Turtle& t, int repetitions)
	{
		float depth = float(t.transform.properties.depth);

		float yawRandOffset = uniformGenerator.RandomFloat(-5.0f, 5.0f);
		float yawBranchOffset = 45.0f*depth + uniformGenerator.RandomFloat(-25.0f, -25.0f);
		float pitchRandOffset = uniformGenerator.RandomFloat(-5.0f, 5.0f);

		t.Rotate(
			120.0f * repetitions + yawBranchOffset + yawRandOffset,
			25.0f + pitchRandOffset
		);

		// Weigh down the branch based on length from root
		glm::fvec3 rotVec = glm::cross(glm::fvec3{ 0.0f, 1.0f, 0.0f }, t.transform.forwardDirection);
		float degrees = depth * iterations;
		t.Rotate(degrees, rotVec);
	};

	turtle.GenerateSkeleton(fractalTree.RunProduction(iterations));

	std::vector<FractalBranch> branches;
	BuildBranchesForFractalTree3D(branches, turtle.rootBone);

	onResultCallback(turtle.rootBone, branches);
}