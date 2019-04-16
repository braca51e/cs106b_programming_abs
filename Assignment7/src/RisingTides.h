#ifndef RisingTides_Included
#define RisingTides_Included

#include "grid.h"
#include "vector.h"

/* Type: Terrain
 * ----------------------------------------------------------------------------------
 * Type representing a terrain as a Grid<double> subdivided into individual cells,
 * each with an associated height, along with a list of sources from which the water
 * floods out.
 *
 * The GridLocation type is a simple struct that contains just a row and a column.
 * It's used as a way of tracking a position in the Grid as a single object.
 */
struct Terrain {
    Grid<double> heights;              // Height of each point on the map, in meters.
    Vector<GridLocation> waterSources; // Which locations, if any, are water sources.
};

/**
 * Given a terrain and an altitude, returns a Grid<bool> indicating whether each cell
 * is under water. Cells under water are marked true; cells above water are marked false.
 * A cell ends up under water if the water can reach it from a source via a path that
 * never flows uphill. In particular, a point that's at the same height as the water
 * source with no intervening higher-altitude cells will be underwater.
 *
 * Remember that water can only flow in cardinal directions (up, down, left, and right),
 * not diagonally.
 *
 * You can assume that all the water sources in the terrain are found within the bounds
 * of the terrain. There may be any number of water sources, including zero.
 *
 * Your solution needs to work on terrains of arbitrary sizes, including terrains that
 * contain millions of cells.
 *
 * @param terrain The terrain map.
 * @param height The water height, in meters.
 * @return A Grid indicating which cells are flooded, with true meaning "flooded" and
 *         false meaning "above water."
 */
Grid<bool> floodedRegionsIn(const Terrain& terrain, double height);

#endif
