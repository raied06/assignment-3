#pragma once
class organism;
struct tile {
	float nutrientLevel;
	float toxicity;
	organism* occupant;

	tile() : nutrientLevel(50.0f), toxicity(0.0f), occupant(nullptr){}
};