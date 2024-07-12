#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "minitiles.h"

extern Canvas canvas;
extern Tiling tiling;

Image frameBuffer;

int showSites = 1;
int showCentroids = 1;
int showOutlines;
int tileShape;
int nTiles;
int nTilesCreated;
int orientation;

enum {EVENT_SHOW_SITES,
      EVENT_SHOW_CENTROIDS,
      EVENT_SHOW_OUTLINES,
      EVENT_TILE_SHAPE,
      EVENT_ORIENTATION,
      EVENT_NTILES,
      EVENT_RANDOMIZE,
      EVENT_COMPUTE_CENTROIDS,
      EVENT_MOVE_TILES,
      EVENT_RECENTER,
      EVENT_QUIT
};

void checkGLError(char *msg) {
  int error = glGetError();
  if(error != GL_NO_ERROR) {
    fprintf(stderr, 
	    "%s, Error: %s\n", msg,gluErrorString(error));
  }
}

///////////////////////////////////////////////////////////

void computeCentroids() {
  int w = canvas.width;
  int h = canvas.height;
  frameBuffer.readScreen(w,h);
  double sx,sy,tx,ty;
  canvas.getDtoWtransform(&sx,&sy, &tx,&ty);
  tiling.computeCentroids(frameBuffer, sx,sy, tx,ty);
}

///////////////////////////////////////////////////////////

void display(void) {
  canvas.displayAll(nTilesCreated,
		    tiling.getSites(),
		    tiling.getCentroids(),
		    tiling.getAngles(),
		    tiling.getConeColours(),
		    showOutlines,
		    showSites,
		    showCentroids);
}

/*********************************************************
    PROC: handleReshape()
    DOES: handles the window being resized -- don't change
**********************************************************/

void handleReshape(int w, int h) {
  //  fprintf(dbgfp,"handleReshape: w=%d h=%d\n",w,h);

  canvas.setWindowDimensions(w,h);
  //
  // Set the viewport
  //
  glViewport(0, 0, w, h);
  canvas.setOrthoView();
  //
  // Now set the camera coordinate system
  //
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /*** this sets the virtual camera          ***/
  /*** gluLookAt( x,y,z,   x,y,z   x,y,z );  ***/
  /***            camera  look-at camera-up  ***/
  /***            pos'n    point   vector    ***/

  canvas.resetCamera();
  float *eye = canvas.getEyePoint();
  float *at  = canvas.getLookat();
  gluLookAt(eye[0],eye[1],eye[2], at[0],at[1],at[2], 0,1,0);
}

//////////////////////////////////////////////////////

void handleMouse (int button, int state, int x, int y) {
  return;
}

//////////////////////////////////////////////////////

void instructions() {
  printf("Q  : quit\n");
  printf("S  : toggle square/round tiles\n");
  printf("O  : toggle vertical/radial orientation\n");
  printf("C  : compute centroids\n");
  printf("M  : move tiles\n");
  printf("R  : recenter tiles\n");
  printf("+/-: more/fewer tiles\n");
}

//////////////////////////////////////////////////////

void handleKey(unsigned char key, int x, int y) {

  switch (key) {
  case 'Q':
  case 27: // esc
  case 'q': 
    exit(0);

  case 'S':
  case 's':
    if (tileShape == TILE_SHAPE_ROUND)
      tileShape = TILE_SHAPE_SQUARE;
    else
      tileShape = TILE_SHAPE_ROUND;
    canvas.setTileShapes(tileShape);
    break;

  case 'O':
  case 'o':
    if (orientation == ORIENTATION_VERTICAL)
      orientation = ORIENTATION_RADIAL;
    else
      orientation = ORIENTATION_VERTICAL;
    tiling.setOrients(orientation);
    break;

  case 'C':
  case 'c':
    computeCentroids();
    break;

  case 'M':
  case 'm':
    tiling.moveSites();
    break;

  case 'R':
  case 'r':
    computeCentroids();
    tiling.moveSites();
    break;

  case '+':
    nTiles++;
    if (nTiles > 1000)
      nTiles = 1000;
    printf ("%d tiles\n",nTiles);
    break;


  case '-':
    nTiles--;
    if (nTiles < 1) 
      nTiles = 1;
    printf ("%d tiles\n",nTiles);
    break;

  case ' ':
  case 13: // Enter key
    tiling.init(nTiles);
    tiling.setOrients(orientation);
    nTilesCreated = nTiles;
    break;

  }

  instructions();
  glutPostRedisplay();
}


