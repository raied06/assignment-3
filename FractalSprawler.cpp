#include"pch.h"
#include "FractalSprawler.h"
#include "world.h"

FractalSprawler::FractalSprawler(int startX, int startY, int w, int h, world* worldPtr)
    : organism(startX, startY, w, h, worldPtr)
{
}

FractalSprawler::~FractalSprawler() {
    // organism base destructor handles shape deallocation
}

void FractalSprawler::tryReproduce() {
    if (!owner) return;

    // Search in an outward spiral for an empty patch big enough for a 2x2 organism
    int spawnW = 2, spawnH = 2;

    for (int dist = 1; dist <= SPAWN_SEARCH_DIST; dist++) {
        // Try positions around the perimeter at this distance
        for (int dy = -dist; dy <= dist; dy++) {
            for (int dx = -dist; dx <= dist; dx++) {
                if (abs(dx) != dist && abs(dy) != dist) continue; // only perimeter
                int nx = x + dx;
                int ny = y + dy;
                if (owner->canPlaceOrganism(nx, ny, spawnW, spawnH)) {
                    FractalSprawler* child = new FractalSprawler(nx, ny, spawnW, spawnH, owner);
                    owner->addOrganism(child);
                    return;
                }
            }
        }
    }
    // No empty spot found nearby — just reset size without spawning
}

void FractalSprawler::update(tile** grid, int worldWidth, int worldHeight) {
    if (!alive) return;

    // Scan radius = 2 * width around centre
    int scanRadius = 2 * width;

    // Gather average nutrients from tiles within scanRadius of the organism's centre
    // (We use the base class helper which checks the organism's own footprint,
    //  but for scanning we also sample the broader neighbourhood)
    float avgNutrients = getAverageNutrients(grid, worldWidth, worldHeight);

    // Also sample from the scan radius to get a richer picture
    float scanSum = 0.0f;
    int   scanCount = 0;
    int   cx = x + width / 2;
    int   cy = y + height / 2;

    for (int dy = -scanRadius; dy <= scanRadius; dy++) {
        for (int dx = -scanRadius; dx <= scanRadius; dx++) {
            int wx = cx + dx;
            int wy = cy + dy;
            if (wx >= 0 && wx < worldWidth && wy >= 0 && wy < worldHeight) {
                scanSum += grid[wy][wx].nutrientLevel;
                scanCount++;
            }
        }
    }
    if (scanCount > 0) avgNutrients = scanSum / scanCount;

    // ── Reproduction check first: reached 5x5 ────────────────────────────────
    if (width >= REPRO_SIZE && height >= REPRO_SIZE) {
        tryReproduce();
        // Reset own size to 2x2
        resizeShape(2, 2, true);
        return;
    }

    // ── Growth: avgNutrients > 70 ─────────────────────────────────────────────
    if (avgNutrients > GROW_THRESHOLD) {
        int newW = width + GROW_AMOUNT;
        int newH = height + GROW_AMOUNT;

        // Clamp to world bounds
        if (x + newW > worldWidth)  newW = worldWidth - x;
        if (y + newH > worldHeight) newH = worldHeight - y;
        if (newW > 0 && newH > 0) {
            resizeShape(newW, newH, true);
        }
        return;
    }

    // ── Shrink: avgNutrients < 30 ─────────────────────────────────────────────
    if (avgNutrients < SHRINK_THRESHOLD) {
        int newW = width - SHRINK_AMOUNT;
        int newH = height - SHRINK_AMOUNT;

        if (newW <= 0 || newH <= 0) {
            // Die
            alive = false;
            return;
        }
        resizeShape(newW, newH, true);
        return;
    }

    // ── Survival zone (30-70): do nothing this tick ───────────────────────────
}

int FractalSprawler::abs(int x) const
{
    if (x < 0)
        return -x;
    return x;
}
