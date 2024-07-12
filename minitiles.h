#ifdef WIN32
#include <windows.h>
#endif

#include "Colour.h"
#include "ColourSet.h"
#include "Point2.h"
#include "Tiling.h"
#include "Canvas.h"

enum {TILE_SHAPE_SQUARE, TILE_SHAPE_ROUND, TILE_SHAPE_N};
enum {ORIENTATION_VERTICAL, ORIENTATION_RADIAL, ORIENTATION_N};

