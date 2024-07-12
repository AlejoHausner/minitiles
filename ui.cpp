#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <GL/gl.h>

#ifdef WIN32
#include <GL/glui.h>
#else
#include <glui.h>
#endif


using namespace std;

#include "minitiles.h"

extern Canvas canvas;
extern Tiling tiling;

Image frameBuffer;

int showSites;
int showCentroids;
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

#define MAX_NTILES 1000

GLUI *glui;

void checkGLError(char *msg) {
  int error = glGetError();
  if(error != GL_NO_ERROR) {
    fprintf(stderr, 
	    "%s, Error: %s\n", msg,gluErrorString(error));
  }
}

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

///////////////////////////////////////////////////////////

void computeCentroids() {
  int w = canvas.width;
  int h = canvas.height;
  frameBuffer.readScreen(w,h);
  double sx,sy,tx,ty;
  canvas.getDtoWtransform(&sx,&sy, &tx,&ty);
  tiling.computeCentroids(frameBuffer, sx,sy, tx,ty);
}

void keyUsage() {
  cout << "Key Pressed    Action\n";
  cout << endl;
  cout << "    o          Toggle tile orientation\n";
  cout << "    s          Toggle tile shape\n";
  cout << endl;
  cout << "    c          Compute centroids\n";
  cout << "    m          Move sites\n";
  cout << "  r,space      Recenter (c, then m)\n";
  cout << endl;
  cout << "   +,-         #tiles +1 or -1\n";
  cout << " 0,1,2,...,9   set #tiles\n";
  cout << "  i,enter      Re-initialize tiles\n";
  cout << endl;
  cout << "    q          Quit\n";
  cout << "    ?          Show this help again\n";

}  
  
//////////////////////////////////////////////////////

void handleKey(unsigned char key, int x, int y) {

  int tileSizes[] = {1,2,5,10,25,50,100,250,500,1000};

  switch (key) {
  case 'Q':
  case 27: // esc
  case 'q': 
    exit(0);

  case 'O':
  case 'o':
    orientation++;
    if (orientation == ORIENTATION_N)
      orientation = 0;
    tiling.setOrients(orientation);

    if (orientation == ORIENTATION_VERTICAL)
      cout << "Vertical orientation\n";
    else
      cout << "Radial orientation\n";
      
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
  case ' ':
    computeCentroids();
    tiling.moveSites();
    break;

  case 'I':
  case 'i':
  case '\r':
  case '\n':
    tiling.init(nTiles);
    tiling.setOrients(orientation);
    nTilesCreated = nTiles;
    break;

  case 'S':
  case 's':
    tileShape++;
    if (tileShape == TILE_SHAPE_N)
      tileShape = 0;
    canvas.setTileShapes(tileShape);
    break;

  case '+':
    nTiles++;
    if (nTiles > MAX_NTILES)
      nTiles = MAX_NTILES;

    cout << "nTiles=" << nTiles << endl;

    break;

  case '-':
    nTiles--;
    if (nTiles < 1)
      nTiles = 1;

    cout << "nTiles=" << nTiles << endl;

    break;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    nTiles = tileSizes[key - '0'];
    cout << "nTiles=" << nTiles << endl;
    break;

  case '?':
    keyUsage();
    return;

  }

  glui->sync_live(); // show changed control variables visually on UI.
  glutPostRedisplay();
}


///////////////////////////////////////////////////////////

void handleGLUIEvent( int ID ) {
  switch(ID) {

  case EVENT_NTILES:
  case EVENT_SHOW_SITES:
  case EVENT_SHOW_CENTROIDS:
  case EVENT_SHOW_OUTLINES:
    break;

  case EVENT_ORIENTATION:
    tiling.setOrients(orientation);
    break;

  case EVENT_COMPUTE_CENTROIDS:
    computeCentroids();
    break;

  case EVENT_MOVE_TILES:
    tiling.moveSites();
    break;

  case EVENT_RECENTER:
    computeCentroids();
    tiling.moveSites();
    break;

  case EVENT_RANDOMIZE:
    tiling.init(nTiles);
    tiling.setOrients(orientation);
    nTilesCreated = nTiles;
    break;

  case EVENT_TILE_SHAPE:
    canvas.setTileShapes(tileShape);
    break;

  case EVENT_QUIT:
    exit(0);
  }

  glutPostRedisplay();
}

////////////////////////////////////////////////////////////

void setupGlui (int mainWindowID, int dx, int dy) {

  keyUsage();

  glui = GLUI_Master.create_glui( "Tiles", 0 , dx,dy);

  nTiles = 10;
  nTilesCreated = nTiles;
  tileShape = TILE_SHAPE_SQUARE;
  orientation = ORIENTATION_VERTICAL;
  showSites = 1;
  showCentroids = 1;
  showOutlines = 1;

  tiling.init(nTiles);
  tiling.setOrients(orientation);

  glui->add_checkbox("Show sites",
		     &showSites, EVENT_SHOW_SITES,
		     handleGLUIEvent);
  glui->add_checkbox("Show centroids",
		     &showCentroids, EVENT_SHOW_CENTROIDS,
		     handleGLUIEvent);
  glui->add_checkbox("Show outlines",
		     &showOutlines, EVENT_SHOW_OUTLINES,
		     handleGLUIEvent);

  GLUI_Spinner *nTilesSpinner = glui->add_spinner("# tiles",
						  GLUI_SPINNER_INT,
						  &nTiles,
						  EVENT_NTILES,
						  handleGLUIEvent);
  nTilesSpinner->set_int_limits(1, MAX_NTILES, GLUI_LIMIT_CLAMP );

  GLUI_RadioGroup *shapeGroup =  glui->add_radiogroup(&tileShape,
						      EVENT_TILE_SHAPE,
						      handleGLUIEvent);
  glui->add_radiobutton_to_group( shapeGroup, "Square tiles" );
  glui->add_radiobutton_to_group( shapeGroup, "Round tiles" );

  GLUI_RadioGroup *orientGroup =  glui->add_radiogroup(&orientation,
						       EVENT_ORIENTATION,
						       handleGLUIEvent);
  glui->add_radiobutton_to_group( orientGroup, "Vertical orientation" );
  glui->add_radiobutton_to_group( orientGroup, "Radial orientation" );

  glui->add_button("Randomize", EVENT_RANDOMIZE, handleGLUIEvent );
  glui->add_button("Get Centroids", EVENT_COMPUTE_CENTROIDS, handleGLUIEvent );
  glui->add_button("Move tiles", EVENT_MOVE_TILES, handleGLUIEvent );
  glui->add_button("Recenter", EVENT_RECENTER, handleGLUIEvent );
  glui->add_button("QUIT", EVENT_QUIT, handleGLUIEvent );


  glui->set_main_gfx_window( mainWindowID );
  GLUI_Master.set_glutIdleFunc( NULL );

  glutMainLoop();
}
