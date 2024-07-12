#include "minitiles.h"

#include "Canvas.h"
#include "Point2.h"
#include "ColourSet.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

extern FILE *dbgfp;
extern void checkGLError(char *msg);

//////////////////////////////////////////////////////
//
// Constructor.  Initialize all the state variables.
//
Canvas::Canvas() {
//  cout << "Canvas constructor." << endl;

  minX = 0;
  minY = 0;
  maxX = 1;
  maxY = 1;

  tileShape = TILE_SHAPE_SQUARE;

  updateCentre();
}

//////////////////////////////////////////////////////
//
// Draws a line outline of a square tile.
//
void Canvas::drawSquareTileOutline() {
  glBegin(GL_LINE_LOOP);
  glVertex3d( 0.5, -0.5, 0.0);
  glVertex3d( 0.5,  0.5, 0.0);
  glVertex3d(-0.5,  0.5, 0.0);
  glVertex3d(-0.5, -0.5, 0.0);
  glEnd();
}

//////////////////////////////////////////////////////
//
// Draws an approximate circle, the outline of a round tile.
//
void Canvas::drawRoundTileOutline() {
  int i;
  double theta=0.0,dtheta=2.0*3.1416/nWedges, x,y;

  glBegin(GL_LINE_LOOP);
  for (i=0; i<nWedges; i++) {
    x = 0.5 * cos(theta);
    y = 0.5 * sin(theta);
    glVertex3d(x,y,0.0);
    theta += dtheta;
  }
  glEnd();
}

//////////////////////////////////////////////////////
//
// Draw a circular cone.
//
void Canvas::drawCone () {
  double dtheta=2.0*3.1416/nWedges;
  int i;
  double x,y;

  glBegin(GL_TRIANGLE_FAN);
  glVertex3d(0.0, 0.0, 0.0);
  for (i=0; i<=nWedges; i++) {
    x = 2.0* cos(i*dtheta);
    y = 2.0* sin(i*dtheta);
    glVertex3d(x,y,-coneHeight*2.0);
  }
  glEnd();
}

//////////////////////////////////////////////////////
//
// Draws a square pyramid instead of a round cone.
//
void Canvas::drawPyramid () {
  glBegin(GL_TRIANGLE_FAN);
  {
    glVertex3d(0.0,0.0,0.0);

    glVertex3d( 1.0, 1.0,-1.0);
    glVertex3d(-1.0, 1.0,-1.0);
    glVertex3d(-1.0,-1.0,-1.0);
    glVertex3d( 1.0,-1.0,-1.0);
    glVertex3d( 1.0, 1.0,-1.0);
  }
  glEnd();
}

//////////////////////////////////////////////////////
//
// Draws a cone (round, square, diamond or general Stroke)
// for the given tile.
//
void Canvas::drawTileCone(double x, double y, 
			  double angle, double scale,
			  Colour& colour) {
  glColor3f(colour.r/255.0, colour.g/255.0, colour.b/255.0);
  glPushMatrix(); 
  {
    glTranslated(x, y, 0.0);
    glRotated(angle, 0.0, 0.0, 1.0);
    glScaled(scale, scale, 1);

    if (tileShape == TILE_SHAPE_ROUND)
      drawCone();
    else if (tileShape == TILE_SHAPE_SQUARE)
      drawPyramid();
  }
  glPopMatrix();
}

//////////////////////////////////////////////////////
//
// Draw all the cones needed to create a voronoi diagram.
//
void Canvas::drawVD (int n, Point2 *sites, 
		     double *angles, ColourSet& coneColours) {
  int i;
  double x, y;
  double canvasWidth  = maxX - minX;
  double canvasHeight = maxY - minY;
  double scale = 2.0 * radius /sqrt((double)n);

  for (i=0; i<n; i++) {
    Point2& pos = sites[i];
    x = pos.x;
    y = pos.y;
    drawTileCone(x, y, angles[i], scale, *(coneColours.at(i)));
  }

  checkGLError("After drawTileCone(s)");
}

//////////////////////////////////////////////////////
//
// Draw all the tiles, in outline.
//
void Canvas::drawTileOutlines (int n, Point2* sites, double* angles) {
  int i;
  double scale = 2.0 * radius /sqrt((double)n);

  glColor3f(0,0,0);

  for (i=0; i<n; i++) {
    glPushMatrix(); 
    {
      Point2& site = sites[i];
      glTranslated(site.x,site.y,0.01);

      glRotated(angles[i],0.0,0.0,1.0);
      glScaled(scale, scale, 1);

      if (tileShape == TILE_SHAPE_ROUND)
	drawRoundTileOutline();
      else if (tileShape == TILE_SHAPE_SQUARE)
	drawSquareTileOutline();
    }
    glPopMatrix();
  }
}

//////////////////////////////////////////////////////
//
// Draw a filled circle, used to mark the center of the tile,
// or the centroid.
//
void Canvas::drawDot () {
  glBegin(GL_POLYGON);
  glVertex3d(-0.5,-1.0, 0.0);
  glVertex3d( 0.5,-1.0, 0.0);
  glVertex3d( 1.0,-0.5, 0.0);
  glVertex3d( 1.0, 0.5, 0.0);
  glVertex3d( 0.5, 1.0, 0.0);
  glVertex3d(-0.5, 1.0, 0.0);
  glVertex3d(-1.0, 0.5, 0.0);
  glVertex3d(-1.0,-0.5, 0.0);
  glEnd();
}

//////////////////////////////////////////////////////
//
// For each tile, draw a dot at the tile center.
//
void Canvas::drawSites (int n, Point2* sites, Colour& c) {
  int i;
  double size = radius/sqrt((double)n) * 0.1;

  glColor3f(c.r/255.0, c.g/255.0, c.b/255.0);
  for (i=0; i<n; i++) {
    Point2& p = sites[i];
    glPushMatrix(); 
    {
      glTranslated(p.x, p.y, 0.01);
      glScaled(size,size,1.0);
      drawDot();
    }
    glPopMatrix();
  }
}

//////////////////////////////////////////////////////
//
// Update variables that depend on the width and height
// of the current window.
//
void Canvas::setWindowDimensions(int w, int h) {
  width = w;
  height = h;

  if (h > w) {
    minX = 0;
    maxX = w / (double) h;
    minY = 0;
    maxY = 1;
  }
  else {
    minX = 0;
    maxX = 1;
    minY = 0;
    maxY = h / (double) w;
  }

  updateCentre();
}

//////////////////////////////////////////////////////
//
// Re-compute the center and span of the drawing area.
//
void Canvas::updateCentre() {
  xC = (minX+maxX) / 2.0;
  yC = (minY+maxY) / 2.0;
  double dx = maxX-minX;
  double dy = maxY-minY;
  radius = dx > dy ? dy/2.0 : dx/2.0;

  resetCamera();
}


//////////////////////////////////////////////////////
//
// Sets up some variables that control how finely the
// cones are drawn.  For cones used int the tile VD, we don't
// need a lot of precision.
//
void Canvas::initCones(int nTiles) {
  nWedges = 12;
  coneHeight = 10.0*radius / sqrt((double)nTiles);
}

/*********************************************************
    PROC: initLights()
    DOES: Sets the lighting parameters.
**********************************************************/

void Canvas::initLights(void) {
  glFrontFace (GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_LIGHTING);
}


//////////////////////////////////////////////////////////////////
//
// Places the camera and lights for an overhead view, with
// flat shading
//
void Canvas::setOrthoView() {

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

  //
  // Now set the projection matrix stack
  //
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double dx = (maxX - minX)/2;
  double dy = (maxY - minY)/2;
  glOrtho(-dx, dx, -dy, dy,
          0.1,radius*10.0);
}

//////////////////////////////////////////////////////
//
// Places the camera at a position that views the whole
// tiling.
//
void Canvas::resetCamera() {
  eye[0] = xC;
  eye[1] = yC;
  eye[2] = radius;
  at[0] = eye[0];
  at[1] = eye[1];
  at[2] = 0.0;
  up[0] = 0;
  up[1] = 1;
  up[2] = 0;
}

//////////////////////////////////////////////////////
//
// Returns the parameters of the current device-to-world
// transformation.  This is needed in Tiling.computeCentroids
// to properly map device-based pixel moments to world-based
// region shapes.
//
void Canvas::getDtoWtransform(double *sx, double *sy, double *tx, double *ty) {
  double dx = maxX - minX;
  double dy = maxY - minY;
  double xL = xC - dx/2;
  double yB = yC - dy/2;
  *sx = dx/width;
  *sy = dy/height;
  *tx = xL + dx/width *0.5;
  *ty = yB + dy/height*0.5;
}

//////////////////////////////////////////////////////
//
// Convert from device to world coords.
//
void Canvas::rc2xy(int row, int col, double *x, double *y) {
  //
  // Convert device coords to world coords.
  // We should handle round-off by making sure that:
  // (row col)=((width-1)/2   (height-1)/2 ) maps to
  //     (x y)=((maxX+minX)/2 (maxY+minY)/2)
  // which it does,
  // and
  // (0 0) maps to (minX+dx/2/width minY+dy/2/height)
  // which it does.
  //
  double dx = maxX - minX;
  double dy = maxY - minY;
  double xL = xC - dx/2;
  double yB = yC - dy/2;
  *x = xL + (col+0.5) * dx / width;
  *y = yB + (row+0.5) * dy / height;
}

//////////////////////////////////////////////////////
//
// Convert from device to world coords.
//
void Canvas::rc2xy(double row, double col, double *x, double *y) {
  double dx = maxX - minX;
  double dy = maxY - minY;
  double xL = xC - dx/2;
  double yB = yC - dy/2;
  *x = xL + (col+0.5) * dx / width;
  *y = yB + (row+0.5) * dy / height;
}

//////////////////////////////////////////////////////
//
// Returns parameters describing the current world-to-device
// coordinate transformation.  Currently unused, but you never know.
//
void Canvas::getWtoDtransform(double *sx, double *sy, double *tx, double *ty) {
  double dx = maxX - minX;
  double dy = maxY - minY;
  double xL = xC - dx/2;
  double yB = yC - dy/2;
  *sx = width/dx;
  *sy = height/dy;
  *tx = -0.5 - xL*width/dx;
  *ty = -0.5 - yB*height/dy;
}

//////////////////////////////////////////////////////
//
// Map point from world to device coords.
//
void Canvas::xy2rc(double x, double y, int *row, int *col) {
  double dx = maxX - minX;
  double dy = maxY - minY;
  double xL = xC - dx/2;
  double yB = yC - dy/2;
  *col = (int)((x - xL)*width /dx - 0.5);
  *row = (int)((y - yB)*height/dy - 0.5);
}

//////////////////////////////////////////////////////
//
// Record the window ID give to us by GLUT when the
// drawing window was created.  Meant for multiple
// windows, but not currently used.
//
void Canvas::setWinID(int ID) {
  winID = ID;
}

//////////////////////////////////////////////////////
//
// Gathers together a bunch of OpenGL setup function calls
// that must be invoked to turn current settings on,
// place the camera, and erase the screen.
//
void Canvas::startDraw() {
  glClearColor(0,0,0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(eye[0],eye[1],eye[2],
            at[0],at[1],at[2], 0,1,0);
}

//////////////////////////////////////////////////////
//
// This is the master function for this class.
// Given a bunch of parameters describing what we need
// to draw, it calls all the necessary methods.
//
void Canvas::displayAll(int nTiles,
			Point2 *sites, 
			Point2 *centroids, 
			double *angles,
			ColourSet& coneColours,
			int showOutlines,
			int showSites,
			int showCentroids) {

  Colour white = Colour::White();
  Colour yellow = Colour::Yellow();

  initCones(nTiles);

  startDraw();
  checkGLError("After startDraw");

  drawVD(nTiles, sites, angles, coneColours);
  checkGLError("After drawVD");

  if (showOutlines) {
    drawTileOutlines(nTiles, sites, angles);
    checkGLError("After drawTileOutlines");
  }

  if (showSites) {
    drawSites(nTiles, sites, white);
    checkGLError("After drawSites");
  }

  if (showCentroids) {
    drawSites(nTiles, centroids, yellow);
    checkGLError("After drawCentroids");
  }

  int glError = glGetError();
  if (glError != GL_NO_ERROR) {
    fprintf(dbgfp,
            "Error at end of displayAll: %s\n",
            gluErrorString(glError));
    printf("Error at end of displayAll: %s\n",
           gluErrorString(glError));
  }

  glFlush();

  glutSwapBuffers();
  glFinish();
}

