#pragma once
class organism;
struct tile {
	float nutrientLevel;
	float toxicity;
	organism* occupant;
	tile() {
		nutrientLevel = 50.0;
		toxicity = 0.0;
		occupant = 0;

	}
};