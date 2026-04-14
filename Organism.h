#pragma once
#include "Tile.h"

class world;

class organism {
protected:
	int x, y;
	int width, height;
	bool** internalShape;
	bool alive;
	world* owner;
    void allocateShape(int h, int w, bool fillTrue = true);
    void deallocateShape();
    void resizeShape(int newW, int newH, bool fillTrue = true);

public:
    organism(int startX, int startY, int w, int h, world* worldPtr);
    virtual ~organism();

    virtual void update(tile** world, int worldWidth, int worldHeight) = 0;

    float getAverageNutrients(tile** world, int worldWidth, int worldHeight);
    float getAverageToxicity(tile** world, int worldWidth, int worldHeight);

    bool occupiesWorldTile(int worldX, int worldY) const;
    bool isAlive() const;
    void kill();

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    void setPosition(int newX, int newY);

    virtual bool isHunter() const = 0;
    virtual bool isSprawler() const = 0;
};