//
// LSystem.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

// Constants
//   Cogwheel thickness     : 0.5f
//   Number of subdivisions : 24
//   Min inner radius       : 0.5f
//   Min tube thickness     : 0.25f
//   Min distance between tube and inner cylinder/tube : 0.5f
//   Min radius to spawn inner cylinder/tube : 0.5f + 0.25f + 0.5f = 1.25f

// Modules
//   C(r, b, w, h)      : Add cylinder mesh (parameters: radius, number of boxes, box width, box height)
//   T(r1, r2, b, w, h) : Add tube mesh (parameters: inner radius, outer radius, number of boxes, box width, box height)
//   B(w, h)            : Add box mesh (parameters: width, height)
//   ^(d)               : Translate up (parameter: distance)
//   /(a)               : Rotate clockwise (parameter: angle)
//   o                  : Go back to origin

// Rules
//   C(r, b, i, w, h)       :  b > 1               ->  C(r, b-1, i, w, h) /(360/b*i) B(w, h)
//                          :  b == 1              ->  C(r, 0, i, w, h) ^(r + (h/2) - 0.2) B(w, h)
//
//   T(r1, r2, b, i, w, h)  :  b > 1               ->  T(r1, r2, b-1, i, w, h) /(360/b*i) B(w, h)
//                          :  b == 1              ->  T(r1, r2, 0, i, w, h) ^(r + (h/2) - 0.2) B(w, h)
//                          :  b == 0, r1 >= 1.25  ->  T(r1, r2, 0, i, w, h)
//                                                 ->  C(r2/3, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  C(r2/4, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  C(r2/3, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  C(r2/4, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//
//                                                 ->  T(r2/3*0.5, r2/3, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/3*0.667, r2/3, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/3*0.334, r2/3, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//
//                                                 ->  T(r2/4*0.5, r2/4, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/4*0.667, r2/4, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/4*0.334, r2/4, round(b/2), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//
//                                                 ->  T(r2/3*0.5, r2/3, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/3*0.667, r2/3, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/3*0.334, r2/3, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//
//                                                 ->  T(r2/4*0.5, r2/4, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/4*0.667, r2/4, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)
//                                                 ->  T(r2/4*0.334, r2/4, round(b/4), i, w/2, r1 - r) T(r1, r2, 0, w, h)

#pragma once

#include <functional>
#include <vector>
#include <map>
#include "Model.h"
#include "Utils.h"

#define CYLINDER_SYMBOL 'C'
#define TUBE_SYMBOL 'T'
#define BOX_SYMBOL 'B'
#define TRANSLATE_UP_SYMBOL '^'
#define ROTATE_CW_SYMBOL '/'
#define ORIGIN_SYMBOL 'o'

#define COGWHEEL_THICKNESS 0.5f
#define SUBDIVISION_COUNT 24
#define MIN_RADIUS_TO_SPAWN 1.25f
#define COGWHEEL_ROTATION_MATRIX XMMatrixRotationRollPitchYaw(XM_PI * 0.5f, XM_PI * 0.0f, XM_PI * 0.0f)

enum CylinderParameters : int
{
	CylinderRadius = 0,
	CylinderBoxCount,
	CylinderBoxIterator,
	CylinderBoxWidth,
	CylinderBoxHeight
};

enum TubeParameters : int
{
	TubeInnerRadius = 0,
	TubeOuterRadius,
	TubeBoxCount,
	TubeBoxIterator,
	TubeBoxWidth,
	TubeBoxHeight
};

enum BoxParameters : int
{
	BoxWidth = 0,
	BoxHeight
};

struct Module
{
	char symbol;
	std::vector<float> parameters;

	Module() {}

	Module(char c, std::vector<float> params)
	{
		symbol = c;
		parameters = params;
	}
};

using Word = std::vector<Module>;

using ConditionFunction = std::function<bool(std::vector<float>)>;
using SuccessorFunction = std::function<Word(Module)>;

struct Rule
{
	ConditionFunction conditionFunction;
	std::vector<SuccessorFunction> successorFunctions; // Equal probability

	Rule() {}

	Rule(ConditionFunction func, std::vector<SuccessorFunction> words)
	{
		conditionFunction = func;
		successorFunctions = words;
	}
};

class LSystem
{
public:
	LSystem();
	~LSystem();

	void GenerateModel(Word axiom, Model *pModel);

private:
	std::map<char, std::vector<Rule>> m_rules;

	void AddRules();
	Word ApplyRules(Word word);
	Word ApplyRule(Module module);
};
