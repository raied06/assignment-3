#include"pch.h"
#include "KineticHunter.h"
#include "world.h"

KineticHunter::KineticHunter(int startX, int startY, int w, int h, world* worldPtr)
    : organism(startX, startY, w, h, worldPtr)
{
}

KineticHunter::~KineticHunter() {
    // base destructor handles shape
}

bool KineticHunter::overlapsWithOrganism(organism* other) const {
    if (!other || !other->isAlive()) return false;

    // Check every tile this hunter occupies against every tile the other occupies
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!internalShape || !internalShape[i][j]) continue;
            int wx = x + j;
            int wy = y + i;
            if (other->occupiesWorldTile(wx, wy)) return true;
        }
    }
    return false;
}

void KineticHunter::eatSprawler(organism* sprawler, tile** grid, int worldWidth, int worldHeight) {
    if (!sprawler || !sprawler->isAlive()) return;

    // Drain nutrientLevel from every world tile the sprawler occupies
    for (int i = 0; i < sprawler->getHeight(); i++) {
        for (int j = 0; j < sprawler->getWidth(); j++) {
            int wx = sprawler->getX() + j;
            int wy = sprawler->getY() + i;
            if (wx < 0 || wy < 0 || wx >= worldWidth || wy >= worldHeight) continue;
            if (!sprawler->occupiesWorldTile(wx, wy)) continue;

            grid[wy][wx].nutrientLevel -= (float)EAT_NUTRIENT_DRAIN;
            if (grid[wy][wx].nutrientLevel < 0.0f)
                grid[wy][wx].nutrientLevel = 0.0f;
        }
    }

    // Shrink the sprawler by PREY_SHRINK_AMOUNT in each dimension
    int newW = sprawler->getWidth() - PREY_SHRINK_AMOUNT;
    int newH = sprawler->getHeight() - PREY_SHRINK_AMOUNT;

    if (newW <= 0 || newH <= 0) {
        sprawler->kill();
    }
    else {
        owner->shrinkOrganism(sprawler, newW, newH);
    }
}

void KineticHunter::update(tile** grid, int worldWidth, int worldHeight) {
    if (!alive) return;

    // Scan radius = 4 * width
    int scanRadius = 4 * width;

    // ── 1. Find nearest FractalSprawler within scan radius ────────────────────
    organism* target = owner->findNearestSprawler(this, scanRadius);

    if (target) {
        // ── 2. Interaction: check for overlap first ───────────────────────────
        if (overlapsWithOrganism(target)) {
            eatSprawler(target, grid, worldWidth, worldHeight);
            return; // don't also move this tick
        }

        // ── 3. Movement: move one step closer to the target's centre ──────────
        int tcx = target->getX() + target->getWidth() / 2;
        int tcy = target->getY() + target->getHeight() / 2;
        int hcx = x + width / 2;
        int hcy = y + height / 2;

        int dx = 0, dy = 0;
        if (tcx > hcx) dx = MOVE_SPEED;
        else if (tcx < hcx) dx = -MOVE_SPEED;

        if (tcy > hcy) dy = MOVE_SPEED;
        else if (tcy < hcy) dy = -MOVE_SPEED;

        // Try to move; check bounds
        int newX = x + dx;
        int newY = y + dy;

        // Clamp to world bounds
        if (newX < 0) newX = 0;
        if (newY < 0) newY = 0;
        if (newX + width > worldWidth)  newX = worldWidth - width;
        if (newY + height > worldHeight) newY = worldHeight - height;

        setPosition(newX, newY);
    }
    // If no target found in range, hunter stays put this tick
}