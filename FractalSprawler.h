#pragma once
#include "Organism.h"

class world;

// ─── FractalSprawler (The Producer) ──────────────────────────────────────────
// Stationary but expands. Scans nutrients in 2*width radius and
// grows, shrinks, or reproduces based on average nutrient level.

class FractalSprawler : public organism {
private:
    static const int  GROW_THRESHOLD = 70;   // avgNutrients > 70 → grow
    static const int  SHRINK_THRESHOLD = 30;   // avgNutrients < 30 → shrink
    static const int  REPRO_SIZE = 5;    // at 5x5 → reproduce, reset to 2x2
    static const int  GROW_AMOUNT = 2;    // grow by +2 in each dimension
    static const int  SHRINK_AMOUNT = 1;    // shrink by -1 in each dimension
    static const int  SPAWN_SEARCH_DIST = 8;    // how far out to look for empty spawn spot

    // Try to find a nearby empty location and spawn a new FractalSprawler there
    void tryReproduce();

public:
    FractalSprawler(int startX, int startY, int w, int h, world* worldPtr);
    ~FractalSprawler();

    void update(tile** grid, int worldWidth, int worldHeight) override;

    bool isHunter()   const override { return false; }
    bool isSprawler() const override { return true; }
    char getSymbol()  const override { return 'G'; }  // G = Green Producer

    int abs(int x)const;

    // Expose resizeShape so world can call it via shrinkOrganism
    void resizeShape(int newW, int newH, bool fill = true) {
        organism::resizeShape(newW, newH, fill);
    }
};