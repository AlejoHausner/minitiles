////////////////////////////////////////////////////
// minitile.cpp
//
// Uses Lloyd's method for centroidal Voronoi diagrams,
// Which are computed with graphics hardware using
// Hoff et al's Siggraph 99 paper.
// The CVD is used to place (initially) square tiles.
// The tiles' placement is defined by a direction field,
// which is defined a priori.  Normally VD's assume the
// Euclidean distance metric.  This corresponds to
// attaching a circular cone to each site, in Hoff et al's paper.
// We're attaching other types of cones, which describe a
// different distance metric.  In particular, we're using
// square pyramids, oriented with the direction field.
// The hope is that, if the direction field corresponds
// to distance from an edge, that the tiles will line up
// in a sort of grid, near the edge.  Kind of what artisans
// do when they tile mosaics by hand.
//
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>

#include "minitiles.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

FILE *dbgfp = fopen("debug.dat","w");


Canvas canvas;
Tiling tiling;

extern void display(void);
extern void setupGlui (int mainWindowID, int dx, int dy);
extern void handleReshape(int w, int h);
extern void handleMouse (int button, int state, int x, int y);
extern void handleKey(unsigned char key, int x, int y);


/*********************************************************
    PROC: main()
    DOES: calls initialization, then hands over control
          to the event handler, which calls
          display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) {

  int w = 400;
  int h = 400;

  glutInit(&argc, argv);

  tiling.init(10);
  canvas.setWindowDimensions(w,h);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowPosition (0, 0);
  glutInitWindowSize(w,h);

  int drawWinID = glutCreateWindow(argv[0]);
  canvas.setWinID(drawWinID);

  canvas.initLights();
  canvas.setOrthoView();

  glutReshapeFunc (handleReshape);
  glutKeyboardFunc(handleKey);
  glutMouseFunc   (0);
  glutDisplayFunc (display);

  setupGlui (drawWinID, w+10, 0);

  return 0;         // never reached
}
