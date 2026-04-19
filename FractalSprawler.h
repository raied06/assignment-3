#pragma once
#include "Organism.h"

class world;

class FractalSprawler : public organism {
private:
    static const int  growThreshold = 70;   // avgNutrients > 70 → grow
    static const int  shrinkThreshold = 30;   // avgNutrients < 30 → shrink
    static const int  reproduceSize = 5;    // at 5x5 → reproduce, reset to 2x2
    static const int  growAmount = 2;    // grow by +2 in each dimension
    static const int  shrinkAmount = 1;    // shrink by -1 in each dimension
    static const int  searchDistance = 8;    // how far out to look for empty spawn spot

    // Try to find a nearby empty location and spawn a new FractalSprawler there
    void tryReproduce();

public:
    FractalSprawler(int startX, int startY, int w, int h, world* worldPtr);
    ~FractalSprawler();

    void update(tile** grid, int worldWidth, int worldHeight) override;

    bool isHunter()   const  { return false; }
    bool isSprawler() const  { return true; }
    char getSymbol()  const  { return 'G'; }  // G = Green Producer

    int abs(int x)const;

    // Expose resizeShape so world can call it via shrinkOrganism
    void resizeShape(int newW, int newH, bool fill = true) {
        organism::resizeShape(newW, newH, fill);
    }
};