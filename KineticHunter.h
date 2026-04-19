#pragma once
#include "Organism.h"

class world;

// ─── KineticHunter (The Consumer) ────────────────────────────────────────────
// Moves and eats FractalSprawlers. Scans 4*width radius for prey,
// moves toward it, and drains nutrients + shrinks the prey on overlap.

class KineticHunter : public organism {
private:
    static const int EAT_NUTRIENT_DRAIN = 10;  // how much nutrient is drained per tile eaten
    static const int PREY_SHRINK_AMOUNT = 1;  // how much prey shrinks each eating interaction
    static const int MOVE_SPEED = 1;  // tiles moved per update toward prey

    // Returns true if this hunter's shape overlaps with the given sprawler
    bool overlapsWithOrganism(organism* other) const;

    // Drain nutrients from tiles the sprawler occupies and shrink it
    void eatSprawler(organism* sprawler, tile** grid, int worldWidth, int worldHeight);

public:
    KineticHunter(int startX, int startY, int w, int h, world* worldPtr);
    ~KineticHunter();

    void update(tile** grid, int worldWidth, int worldHeight) override;

    bool isHunter()   const override { return true; }
    bool isSprawler() const override { return false; }
    char getSymbol()  const override { return 'H'; }  // H = Hunter
};