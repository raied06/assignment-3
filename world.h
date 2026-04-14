#pragma once
#include "Tile.h"

class organism;

class world {
private:
    tile** grid;
    int worldWidth;
    int worldHeight;

    organism** population;
    int populationCount;
    int populationCapacity;

    void allocateGrid();
    void deallocateGrid();
    void expandPopulation();
    void removeDeadOrganisms();
    void rebuildOccupants();

public:
    world(int w, int h, int capacity = 10);
    ~world();

    void seedWorld();
    void runIteration();
    void display() const;

    void addOrganism(organism* org);
    bool canPlaceOrganism(int startX, int startY, int w, int h) const;

    organism* findNearestSprawler(organism* hunter, int radius);
    void shrinkOrganism(organism* org, int newW, int newH);

    float getAverageWorldToxicity() const;

    tile** getGrid() const;
    int getWidth() const;
    int getHeight() const;

    int getPopulationCount() const;
    organism* getPopulationAt(int index) const;
};