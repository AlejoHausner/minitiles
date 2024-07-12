#ifndef TILING_H
#define TILING_H

#ifdef WIN32
#include <windows.h>
#endif

#include "Image.h"
#include "ColourSet.h"
#include "Point2.h"

class Tiling {
public:
  Tiling();
  void init(int newSize);
  void setOrients(int orientation);
  void moveSites();
  Point2* getSites() { return sites; };
  Point2* getCentroids() { return centroids; };
  int getNTiles() { return nTiles; };
  ColourSet& getConeColours() { return coneColours; };
  double *getAngles() { return angles; };
  void computeCentroids(Image& img,
			double sx, double sy,
			double tx, double ty);
private:
  int nTiles;
  Point2 *centroids;
  Point2 *sites;
  ColourSet coneColours;
  double *angles;
};
#endif
