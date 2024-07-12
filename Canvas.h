#ifndef CANVAS_H
#define CANVAS_H

#ifdef WIN32
#include <windows.h>
#endif

#include "Colour.h"
#include "Point2.h"
#include "ColourSet.h"

class Canvas {
public:
  Canvas();
  void updateCentre();
  void resetCamera();
  void setWindowDimensions(int w, int h);
  void setWinID(int ID);
  void initLights(void);
  void setOrthoView();

  void initCones(int nTiles);

  void rc2xy(int row, int col, double *x, double *y);
  void rc2xy(double row, double col, double *x, double *y);
  void xy2rc(double x, double y, int *row, int *col);

  void displayAll(int nTiles,
		  Point2* sites, 
		  Point2* centroids, 
		  double* angles,
		  ColourSet& coneColours,
		  int showOutlines,
		  int showSites,
		  int showCentroids);

  void startDraw();

  void drawSquareTileOutline();
  void drawRoundTileOutline();

  void drawTileCone(double x, double y, 
		    double angle, double scale, 
		    Colour& colour);
  void drawCone ();
  void drawPyramid ();

  void drawVD (int nTiles, Point2 *sites, 
	       double *angles, ColourSet& coneColours);
  void drawDot ();
  void drawSites (int n, Point2* sites, Colour& c);
  void drawTileOutlines (int n, Point2* sites, double* angles);

  double xC,yC,radius;
  int width, height;     // window (x,y) size

  int getWinID() {return winID;};

  double getMinX() {return minX;};
  double getMinY() {return minY;};
  double getMaxX() {return maxX;};
  double getMaxY() {return maxY;};

  double getDX() {return maxX - minX;};
  double getDY() {return maxY - minY;};

  void getWtoDtransform(double *sx, double *sy, double *tx, double *ty);
  void getDtoWtransform(double *sx, double *sy, double *tx, double *ty);

  void setTileShapes(int newShape) {tileShape = newShape; };

  float *getEyePoint() { return eye; };
  float *getLookat() { return at; };

private:
  double dTheta;
  int nWedges;
  double coneHeight;
  double maxX,minX,maxY,minY;
  float eye[3];
  float at[3];
  float up[3];
  int winID;
  int tileShape;
};

#endif
