#include"pch.h"
#include "world.h"
#include "Organism.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ─── Constructor / Destructor

world::world(int w, int h, int capacity)
    : worldWidth(w), worldHeight(h),
    population(nullptr), populationCount(0), populationCapacity(capacity)
{
    grid = nullptr;
    allocateGrid();

    population = new organism * [populationCapacity];
    for (int i = 0; i < populationCapacity; i++)
        population[i] = nullptr;
}

world::~world() {
    // Delete all organism objects
    for (int i = 0; i < populationCount; i++) {
        delete population[i];
        population[i] = nullptr;
    }
    delete[] population;
    population = nullptr;

    deallocateGrid();
}

// ─── Private helpers ──────────────────────────────────────────────────────────

void world::allocateGrid() {
    grid = new tile * [worldHeight];
    for (int i = 0; i < worldHeight; i++) {
        grid[i] = new tile[worldWidth];  // tile default constructor sets nutrients=50, tox=0
    }
}

void world::deallocateGrid() {
    if (!grid) return;
    for (int i = 0; i < worldHeight; i++) {
        delete[] grid[i];
    }
    delete[] grid;
    grid = nullptr;
}

void world::expandPopulation() {
    // Double the capacity
    int newCap = populationCapacity * 2;
    organism** newPop = new organism * [newCap];
    for (int i = 0; i < newCap; i++) newPop[i] = nullptr;
    for (int i = 0; i < populationCount; i++) newPop[i] = population[i];
    delete[] population;
    population = newPop;
    populationCapacity = newCap;
}

// Remove dead organisms from the population array (compacts the array)
void world::removeDeadOrganisms() {
    int alive = 0;
    for (int i = 0; i < populationCount; i++) {
        if (population[i] && population[i]->isAlive()) {
            population[alive++] = population[i];
        }
        else {
            delete population[i];
            population[i] = nullptr;
        }
    }
    // Null out the remainder
    for (int i = alive; i < populationCount; i++) {
        population[i] = nullptr;
    }
    populationCount = alive;
}

// Rebuild tile occupant pointers from scratch each iteration
void world::rebuildOccupants() {
    // Clear all occupants
    for (int i = 0; i < worldHeight; i++)
        for (int j = 0; j < worldWidth; j++)
            grid[i][j].occupant = nullptr;

    // For each organism, mark the tiles it occupies
    // Hunters are "dominant" — written last so they overwrite Sprawlers
    // Pass 1: Sprawlers
    for (int k = 0; k < populationCount; k++) {
        organism* org = population[k];
        if (!org || !org->isAlive() || org->isHunter()) continue;
        for (int i = 0; i < org->getHeight(); i++) {
            for (int j = 0; j < org->getWidth(); j++) {
                int wx = org->getX() + j;
                int wy = org->getY() + i;
                if (wx >= 0 && wx < worldWidth && wy >= 0 && wy < worldHeight) {
                    if (org->occupiesWorldTile(wx, wy)) {
                        grid[wy][wx].occupant = org;
                    }
                }
            }
        }
    }
    // Pass 2: Hunters (dominant — overwrite)
    for (int k = 0; k < populationCount; k++) {
        organism* org = population[k];
        if (!org || !org->isAlive() || !org->isHunter()) continue;
        for (int i = 0; i < org->getHeight(); i++) {
            for (int j = 0; j < org->getWidth(); j++) {
                int wx = org->getX() + j;
                int wy = org->getY() + i;
                if (wx >= 0 && wx < worldWidth && wy >= 0 && wy < worldHeight) {
                    if (org->occupiesWorldTile(wx, wy)) {
                        grid[wy][wx].occupant = org;
                    }
                }
            }
        }
    }
}

// ─── Public API ───────────────────────────────────────────────────────────────

void world::addOrganism(organism* org) {
    if (!org) return;
    if (populationCount >= populationCapacity) expandPopulation();
    population[populationCount++] = org;
}

bool world::canPlaceOrganism(int startX, int startY, int w, int h) const {
    // Check bounds
    if (startX < 0 || startY < 0) return false;
    if (startX + w > worldWidth)  return false;
    if (startY + h > worldHeight) return false;
    // Check no existing occupant on any of these tiles
    for (int i = startY; i < startY + h; i++) {
        for (int j = startX; j < startX + w; j++) {
            if (grid[i][j].occupant != nullptr) return false;
        }
    }
    return true;
}

// Seed the world with random nutrient/toxicity variation and initial organisms
void world::seedWorld() {
    // Randomise tile nutrients and toxicity a little
    for (int i = 0; i < worldHeight; i++) {
        for (int j = 0; j < worldWidth; j++) {
            grid[i][j].nutrientLevel = 40.0f + (float)(rand() % 40);  // 40-80
            grid[i][j].toxicity = (float)(rand() % 20);           // 0-20
        }
    }
    // Note: derived organisms are added by main() after seedWorld()
}

// runIteration: update all organisms, then clean up dead ones
void world::runIteration() {
    rebuildOccupants();

    // Snapshot population count BEFORE updates so new spawns this tick
    // don't update in the same iteration they were born
    int countThisTick = populationCount;

    for (int i = 0; i < countThisTick; i++) {
        if (population[i] && population[i]->isAlive()) {
            population[i]->update(grid, worldWidth, worldHeight);
        }
    }

    removeDeadOrganisms();
    rebuildOccupants();
}

// Display the grid in the console using plain characters
// Each tile prints 2 chars wide so the grid looks square in most terminals.
// The display character ('.', 'G', 'H', etc.) comes from the organism's
// virtual getSymbol() - no organism means an empty tile shown as '..'
void world::display() const {
    cout << "\n";

    // Top border: one '+' then two dashes per column then '+'
    cout << "+";
    for (int j = 0; j < worldWidth; j++) cout << "--";
    cout << "+\n";

    for (int i = 0; i < worldHeight; i++) {
        cout << "|";
        for (int j = 0; j < worldWidth; j++) {
            organism* occ = grid[i][j].occupant;
            if (!occ) {
                // Empty tile - two dots so width matches occupied tiles
                cout << "..";
            }
            else {
                // Each subclass returns its own symbol via getSymbol()
                char sym = occ->getSymbol();
                cout << sym << sym;   // print twice to keep 2-char width
            }
        }
        cout << "|\n";
    }

    // Bottom border
    cout << "+";
    for (int j = 0; j < worldWidth; j++) cout << "--";
    cout << "+\n";

    // Stats line
    int   pop = getPopulationCount();
    float tox = getAverageWorldToxicity();
    cout << "Total Population: " << pop
        << "  |  Average World Toxicity: "
        << fixed << setprecision(2) << tox << "\n";

    // Legend
    cout << "[Legend]  GG = FractalSprawler (Producer)"
        << "  |  HH = KineticHunter (Consumer)"
        << "  |  .. = Empty\n";
}

// Find the nearest FractalSprawler within searchRadius tiles of the hunter's centre
organism* world::findNearestSprawler(organism* hunter, int searchRadius) {
    if (!hunter) return nullptr;

    int hcx = hunter->getX() + hunter->getWidth() / 2;
    int hcy = hunter->getY() + hunter->getHeight() / 2;

    organism* nearest = nullptr;
    float     bestDist2 = (float)(searchRadius * searchRadius + 1);

    for (int k = 0; k < populationCount; k++) {
        organism* org = population[k];
        if (!org || !org->isAlive() || !org->isSprawler()) continue;

        int cx = org->getX() + org->getWidth() / 2;
        int cy = org->getY() + org->getHeight() / 2;

        float dx = (float)(cx - hcx);
        float dy = (float)(cy - hcy);
        float dist2 = dx * dx + dy * dy;

        float r2 = (float)(searchRadius * searchRadius);
        if (dist2 <= r2 && dist2 < bestDist2) {
            bestDist2 = dist2;
            nearest = org;
        }
    }
    return nearest;
}

// Shrink an organism to newW x newH (called by Hunter after eating)
void world::shrinkOrganism(organism* org, int newW, int newH) {
    if (!org) return;
    org->resizeShape(newW, newH, true);
}

float world::getAverageWorldToxicity() const {
    float sum = 0.0f;
    int   count = worldWidth * worldHeight;
    for (int i = 0; i < worldHeight; i++)
        for (int j = 0; j < worldWidth; j++)
            sum += grid[i][j].toxicity;
    return (count > 0) ? sum / count : 0.0f;
}

tile** world::getGrid()   const { return grid; }
int    world::getWidth()  const { return worldWidth; }
int    world::getHeight() const { return worldHeight; }

int world::getPopulationCount() const { return populationCount; }

organism* world::getPopulationAt(int idx) const {
    if (idx < 0 || idx >= populationCount) return nullptr;
    return population[idx];
}