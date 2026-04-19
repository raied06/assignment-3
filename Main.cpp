#include"pch.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "world.h"
#include "FractalSprawler.h"
#include "KineticHunter.h"

int main() {
    srand((unsigned int)time(nullptr));

    // ── World dimensions ──────────────────────────────────────────────────────
    const int WORLD_W = 30;
    const int WORLD_H = 20;
    const int ITERATIONS = 20;

    world simWorld(WORLD_W, WORLD_H, 20);
    simWorld.seedWorld();

    // ── Place initial FractalSprawlers (Producers) ────────────────────────────
    // Spread them around the grid so they don't overlap
    simWorld.addOrganism(new FractalSprawler(2, 2, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(10, 2, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(20, 2, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(2, 10, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(14, 10, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(24, 10, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(2, 16, 2, 2, &simWorld));
    simWorld.addOrganism(new FractalSprawler(18, 16, 2, 2, &simWorld));

    // ── Place initial KineticHunters (Consumers) ─────────────────────────────
    simWorld.addOrganism(new KineticHunter(7, 7, 2, 2, &simWorld));
    simWorld.addOrganism(new KineticHunter(22, 7, 2, 2, &simWorld));

    // ── Display initial state ─────────────────────────────────────────────────
    printf("========================================\n");
    printf("   THE PRIMORDIAL GRID - INITIAL STATE  \n");
    printf("========================================\n");
    simWorld.display();

    // ── Run simulation ────────────────────────────────────────────────────────
    for (int iter = 1; iter <= ITERATIONS; iter++) {
        printf("\n========================================\n");
        printf("            ITERATION %d                 \n", iter);
        printf("========================================\n");

        simWorld.runIteration();
        simWorld.display();

        // Small pause hint (you can add platform sleep here if desired)
        // e.g. on Windows: Sleep(300);  on Linux: usleep(300000);
    }

    printf("\nSimulation complete.\n");
    return 0;
}