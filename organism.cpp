#include "Organism.h"
#include "world.h"
#include "Tile.h"

//constructor
organism::organism(int startX, int startY, int w, int h, world* worldPtr) {
    x = startX;
    y = startY;
    width = w;
    height = h;
    owner = worldPtr;
    alive = true;
    internalShape = 0;
    allocateShape(height, width, true);
}
//destrucotr
organism::~organism()
{
    deallocateShape();
}
//give organism a new shape
void organism::allocateShape(int h, int w, bool fillTrue) {
    if (h <= 0 || w <= 0) {
        internalShape = 0;
        return;
    }
    internalShape = new bool* [h];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            internalShape[i][j] = fillTrue; //fill shape with true 
        }
    }
}
//remove the organism's shape from memory
void organism::deallocateShape() {
    if (!internalShape)
        return;
    for (int i = 0; i < height; i++) {              //deletion done in reverse order to allocation
        delete[] internalShape[i];
    }
    delete[] internalShape;
    internalShape = nullptr;
}
//make shape bigger/smaller
void organism::resizeShape(int newW, int newH, bool fillTrue) {
    deallocateShape();
    width = newW;
    height = newH;
    if (width <= 0 || height <= 0) {
        width = 0;
        height = 0;
        alive = false;
        internalShape = nullptr;
        return;
    }
    allocateShape(height, width, fillTrue);
}

float organism::getAverageNutrients(tile** world, int worldWidth, int worldHeight) {
    float sum = 0.0;
    int count = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!internalShape || !internalShape[i][j]) //if shape pointer doesn’t exist, skip, if this internal cell is false, skip
                continue;
            int wx = x + j;
            int wy = y + i;
            if (wx >= 0 && wy >= 0 && wx < worldWidth && wy < worldHeight) {
                sum += world[wy][wx].nutrientLevel;
                count++;
            }
        }
    }
    if (count == 0)
        return 0.0f;
    return sum / count;
}
//same logic as nutrients, made just in case maybe will come handy later
float organism::getAverageToxicity(tile** world, int worldWidth, int worldHeight) {
    int sum = 0.0f;
    int count = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!internalShape || !internalShape[i][j])
                continue;
            int wx = x + j;
            int wy = y + i;
            if (wx >= 0 && wx < worldWidth && wy >= 0 && wy < worldHeight) {
                sum += world[wy][wx].toxicity;
                count++;
            }
        }
    }
    if (count == 0)
        return 0.0;
    return sum / count;
}
//check if organism occupies world tile at x,y
bool organism::occupiesWorldTile(int worldX, int worldY) const {
    int relX = worldX - x; //relative X and Y (relative to starting X and Y coordinate)
    int relY = worldY - y;
    if (relX < 0 || relY < 0 || relX >= width || relY >= height)
        return false;
    if (!internalShape)
        return false;
    return internalShape[relY][relX];
}
//check alive/dead
bool organism::isAlive() const {
    return alive;
}

void organism::kill() {
    alive = false;
}
//getters
int organism::getX() const { return x; }
int organism::getY() const { return y; }
int organism::getWidth() const { return width; }
int organism::getHeight() const { return height; }
//update position
void organism::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}
