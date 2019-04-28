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

	// Cylinder rules

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
		Module translate(TRANSLATE_UP_SYMBOL, { fCylinderRadius + (fBoxHeight / 2) - 0.2f });
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

	//   T(r1, r2, b, i, w, h)  :  b > 1               ->  T(r1, r2, b-1, i, w, h) /(360/b*i) B(w, h)
	//                          :  b == 1              ->  T(r1, r2, 0, i, w, h) ^(r + (h/2) - 0.2) B(w, h)
	//                          :  b == 0, r1 >= 1.25  ->  T(r1, r2, 0, i, w, h)

	SuccessorFunction TubeRule1Successor = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });
		Module rotate(ROTATE_CW_SYMBOL, { 2 * XM_PI / fBoxCount * (fBoxIterator + 1) });
		Module box(BOX_SYMBOL, { fBoxWidth, fBoxHeight });

		return { nextTube, rotate, box };
	};

	SuccessorFunction TubeRule2Successor = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });
		Module translate(TRANSLATE_UP_SYMBOL, { fTubeOuterRadius + (fBoxHeight / 2) - 0.2f });
		Module box(BOX_SYMBOL, { fBoxWidth, fBoxHeight });

		return { nextTube, translate, box };
	};

	SuccessorFunction TubeRule3Successor1 = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { nextTube };
	};

	//                                                 ->  C(r2/3, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  C(r2/4, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  C(r2/3, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  C(r2/4, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)

	SuccessorFunction TubeRule3Successor2 = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fCylinderRadius = fTubeOuterRadius / 3;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module cylinder(CYLINDER_SYMBOL, { fCylinderRadius, fSpokeCount, 0, fBoxWidth/2, fTubeInnerRadius - fCylinderRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { cylinder, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor3 = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fCylinderRadius = fTubeOuterRadius / 4;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module cylinder(CYLINDER_SYMBOL, { fCylinderRadius, fSpokeCount, 0, fBoxWidth / 2, fTubeInnerRadius - fCylinderRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { cylinder, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor4 = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fCylinderRadius = fTubeOuterRadius / 3;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module cylinder(CYLINDER_SYMBOL, { fCylinderRadius, fSpokeCount, 0, fBoxWidth / 2, fTubeInnerRadius - fCylinderRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { cylinder, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor5 = [this](Module tube) -> Word
	{
		float fTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fCylinderRadius = fTubeOuterRadius / 4;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module cylinder(CYLINDER_SYMBOL, { fCylinderRadius, fSpokeCount, 0, fBoxWidth / 2, fTubeInnerRadius - fCylinderRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fTubeInnerRadius, fTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { cylinder, origin, nextTube };
	};

	//                                                 ->  T(r2/3*0.5, r2/3, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/3*0.667, r2/3, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/3*0.334, r2/3, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)

	SuccessorFunction TubeRule3Successor6 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.5f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor7 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.667f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor8 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.334f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	//                                                 ->  T(r2/4*0.5, r2/4, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/4*0.667, r2/4, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/4*0.334, r2/4, round(b/2), i, w/2, r1 - r) o T(r1, r2, 0, w, h)

	SuccessorFunction TubeRule3Successor9 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.5f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor10 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.667f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor11 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.334f;
		float fSpokeCount = max(roundf(fBoxCount / 2), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	//                                                 ->  T(r2/3*0.5, r2/3, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/3*0.667, r2/3, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/3*0.334, r2/3, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)

	SuccessorFunction TubeRule3Successor12 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.5f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor13 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.667f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor14 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 3;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.334f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	//                                                 ->  T(r2/4*0.5, r2/4, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/4*0.667, r2/4, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)
	//                                                 ->  T(r2/4*0.334, r2/4, round(b/4), i, w/2, r1 - r) o T(r1, r2, 0, w, h)

	SuccessorFunction TubeRule3Successor15 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.5f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor16 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.667f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	SuccessorFunction TubeRule3Successor17 = [this](Module tube) -> Word
	{
		float fNextTubeInnerRadius = tube.parameters[TubeParameters::TubeInnerRadius];
		float fNextTubeOuterRadius = tube.parameters[TubeParameters::TubeOuterRadius];
		float fBoxCount = tube.parameters[TubeParameters::TubeBoxCount];
		float fBoxIterator = tube.parameters[TubeParameters::TubeBoxIterator];
		float fBoxWidth = tube.parameters[TubeParameters::TubeBoxWidth];
		float fBoxHeight = tube.parameters[TubeParameters::TubeBoxHeight];

		float fSmallTubeOuterRadius = fNextTubeOuterRadius / 4;
		float fSmallTubeInnerRadius = fSmallTubeOuterRadius * 0.334f;
		float fSpokeCount = max(roundf(fBoxCount / 4), MIN_SPOKE_COUNT);

		Module smallTube(TUBE_SYMBOL, { fSmallTubeInnerRadius, fSmallTubeOuterRadius, fSpokeCount, 0, fBoxWidth / 2, fNextTubeInnerRadius - fSmallTubeOuterRadius + 0.5f });
		Module origin(ORIGIN_SYMBOL, {});
		Module nextTube(TUBE_SYMBOL, { fNextTubeInnerRadius, fNextTubeOuterRadius, fBoxCount, fBoxIterator + 1, fBoxWidth, fBoxHeight });

		return { smallTube, origin, nextTube };
	};

	Rule tubeRule1(BoxCountGreaterThanOne, { TubeRule1Successor });
	Rule tubeRule2(BoxCountEqualToOne, { TubeRule2Successor });
	Rule tubeRule3(BoxCountEqualToZero, { TubeRule3Successor1, TubeRule3Successor2, TubeRule3Successor3, TubeRule3Successor4,
										  TubeRule3Successor5, TubeRule3Successor6, TubeRule3Successor7, TubeRule3Successor8,
										  TubeRule3Successor9 , TubeRule3Successor10 , TubeRule3Successor11,
										  TubeRule3Successor12 , TubeRule3Successor13 , TubeRule3Successor14,
										  TubeRule3Successor15 , TubeRule3Successor16 , TubeRule3Successor17 });

	std::vector<Rule> tubeRules;
	tubeRules.push_back(tubeRule1);
	tubeRules.push_back(tubeRule2);
	tubeRules.push_back(tubeRule3);

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
		Utils utils;
		int index = utils.GetRandomInt(0, size - 1);
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
