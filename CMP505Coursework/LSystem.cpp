//
// LSystem.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "LSystem.h"

LSystem::LSystem()
{
	AddRules();
}

LSystem::~LSystem()
{
}

void LSystem::AddRules()
{
	// Condition functions

	ConditionFunction BoxCountGreaterThanOne = [this](std::vector<float> args) -> bool
	{
		return args[0] > 1;
	};

	ConditionFunction BoxCountEqualToOne = [this](std::vector<float> args) -> bool
	{
		return args[0] == 1;
	};

	ConditionFunction BoxCountEqualToZero = [this](std::vector<float> args) -> bool
	{
		return args[0] == 0 && args[1] > MIN_RADIUS_TO_SPAWN;
	};

	// Successor functions

	//   C(r, b, i, w, h)       :  b > 1               ->  C(r, b-1, i, w, h) /(360/b*i) B(w, h)
	//                          :  b == 1              ->  C(r, 0, i, w, h) ^(r + (h/2) - 0.2) B(w, h)

	SuccessorFunction CylinderRule1Successor = [this](Module cylinder) -> Word
	{
		float fCylinderRadius = cylinder.parameters[CylinderParameters::CylinderRadius];
		float fBoxCount = cylinder.parameters[CylinderParameters::CylinderBoxCount];
		float fBoxIterator = cylinder.parameters[CylinderParameters::CylinderBoxIterator];
		float fBoxWidth = cylinder.parameters[CylinderParameters::CylinderBoxWidth];
		float fBoxHeight = cylinder.parameters[CylinderParameters::CylinderBoxHeight];

		Module nextCylinder(CYLINDER_SYMBOL, { fCylinderRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });
		Module rotate(ROTATE_CW_SYMBOL, { 2 * XM_PI / fBoxCount * (fBoxIterator + 1) });
		Module box(BOX_SYMBOL, { fBoxWidth, fBoxHeight });

		return { nextCylinder, rotate, box };
	};

	SuccessorFunction CylinderRule2Successor = [this](Module cylinder) -> Word
	{
		float fCylinderRadius = cylinder.parameters[CylinderParameters::CylinderRadius];
		float fBoxCount = cylinder.parameters[CylinderParameters::CylinderBoxCount];
		float fBoxIterator = cylinder.parameters[CylinderParameters::CylinderBoxIterator];
		float fBoxWidth = cylinder.parameters[CylinderParameters::CylinderBoxWidth];
		float fBoxHeight = cylinder.parameters[CylinderParameters::CylinderBoxHeight];

		Module nextCylinder(CYLINDER_SYMBOL, { fCylinderRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });
		Module translate(TRANSLATE_UP_SYMBOL, { fCylinderRadius + (fBoxHeight/2) - 0.2f });
		Module box(BOX_SYMBOL, { fBoxWidth, fBoxHeight });

		return { nextCylinder, translate, box };
	};

	Rule cylinderRule1(BoxCountGreaterThanOne, { CylinderRule1Successor });
	Rule cylinderRule2(BoxCountEqualToOne, { CylinderRule2Successor });

	std::vector<Rule> cylinderRules;
	cylinderRules.push_back(cylinderRule1);
	cylinderRules.push_back(cylinderRule2);

	m_rules[CYLINDER_SYMBOL] = cylinderRules;

	// Tube rules

	Rule rule3(BoxCountGreaterThanOne, {});
	Rule rule4(BoxCountEqualToOne, {});
	Rule rule5(BoxCountEqualToZero, {});

	std::vector<Rule> tubeRules;
	tubeRules.push_back(rule3);
	tubeRules.push_back(rule4);
	tubeRules.push_back(rule5);

	m_rules[TUBE_SYMBOL] = tubeRules;
}

Word LSystem::ApplyRules(Word currentWord)
{
	Word nextWord;

	bool bHasNonTerminalModule = false;

	for (Module module : currentWord)
	{
		Word word = ApplyRule(module);
		int size = static_cast<int>(word.size());
		if (size == 0)
		{
			nextWord.push_back(module);
		}
		else 
		{
			bHasNonTerminalModule = true;
			nextWord.insert(nextWord.end(), word.begin(), word.end());
		}
	}

	if (bHasNonTerminalModule)
	{
		nextWord = ApplyRules(nextWord);
	}

	return nextWord;
}

Word LSystem::ApplyRule(Module module)
{
	Word nextWord;

	std::vector<Rule> rules = m_rules[module.symbol];
	std::vector<SuccessorFunction> successorFunctions;

	for (Rule rule : rules)
	{
		float fCurrentBoxCount = 0.0f;
		float fRadius = 0.0f;

		if (module.symbol == CYLINDER_SYMBOL)
		{
			fCurrentBoxCount = module.parameters[CylinderParameters::CylinderBoxCount] - module.parameters[CylinderParameters::CylinderBoxIterator];
		}
		else if (module.symbol = TUBE_SYMBOL)
		{
			fCurrentBoxCount = module.parameters[TubeParameters::TubeBoxCount] - module.parameters[TubeParameters::TubeBoxIterator];
			fRadius = module.parameters[TubeParameters::TubeInnerRadius];
		}

		bool condition = rule.conditionFunction({ fCurrentBoxCount, fRadius });
		if (condition)
		{
			successorFunctions = rule.successorFunctions;
			break;
		}
	}
	
	int size = static_cast<int>(successorFunctions.size());
	if (size == 1)
	{
		SuccessorFunction successorFunction = successorFunctions[0];
		nextWord = successorFunction(module);
	}
	else if (size > 1)
	{
		int index = Utils::GetRandomInt(0, size - 1);
		SuccessorFunction successorFunction = successorFunctions[index];
		nextWord = successorFunction(module);
	}

	return nextWord;
}

void LSystem::GenerateModel(Word axiom, Model *pModel)
{
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	Word word = ApplyRules(axiom);
	for (Module module : word)
	{
		switch (module.symbol)
		{
		case CYLINDER_SYMBOL:
			pModel->AddCylinderMesh(module.parameters[CylinderParameters::CylinderRadius], 
									COGWHEEL_THICKNESS, SUBDIVISION_COUNT, COGWHEEL_ROTATION_MATRIX);
			break;
		case TUBE_SYMBOL:
			pModel->AddTubeMesh(module.parameters[TubeParameters::TubeInnerRadius], 
								module.parameters[TubeParameters::TubeOuterRadius], 
								COGWHEEL_THICKNESS, SUBDIVISION_COUNT, COGWHEEL_ROTATION_MATRIX);
			break;
		case BOX_SYMBOL:
			pModel->AddBoxMesh(XMFLOAT3(module.parameters[BoxParameters::BoxWidth], 
										module.parameters[BoxParameters::BoxHeight], 
										COGWHEEL_THICKNESS * 0.99),
							   translationMatrix * rotationMatrix);
			break;
		case TRANSLATE_UP_SYMBOL:
			translationMatrix = XMMatrixTranslation(0.0f, module.parameters[0], 0.0f);
			break;
		case ROTATE_CW_SYMBOL:
			rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, module.parameters[0]);
			break;
		case ORIGIN_SYMBOL:
			translationMatrix = XMMatrixIdentity();
			rotationMatrix = XMMatrixIdentity();
			break;
		}
	}
}
