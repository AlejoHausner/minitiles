#ifndef IMAGE_H
#define IMAGE_H

#ifdef WIN32
#include <windows.h>
#endif

#include "Colour.h"

typedef unsigned char byte;

#include <vector>
using namespace std;

class Image {
public:
  Image();
  ~Image();
  void ensureSpace(int width, int height);
  void readFile(char *filename);
  void readFile_OLD(char *filename);
  void readScreen(int width, int height);
  void drawScreen(int width, int height);
  void writeFile();
  void writeFile(char *filename);
  void setScale(double xC, double yC, double radius);
  void setDimensions(int height, int width);
  byte **getBuf() {return buf;};
  int getWidth() {return width;};
  int getHeight() {return height;};
  void rc2xy(int row, int col, double *x, double *y);
  void xy2rc(double x, double y, int *row, int *col);
  void setCoordSystem(double xMin, double xMax, double yMin, double yMax);
  double getRadius() {return radius;};
  Colour getValue(double x, double y);
  void clear(int r, int g, int b);
  void setTransparency(int r, int g, int b);
  void init(Image& other);

protected:
  byte **buf;
  int width,height;
  double maxX,minX,maxY,minY;
  double xC,yC,radius;
};

#endif
