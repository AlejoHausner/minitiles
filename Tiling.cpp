#include "minitiles.h"
#include "Tiling.h"

extern FILE *dbgfp;

///////////////////////////////////////////////////////////////
//
// Constructor.
//
Tiling::Tiling() {
  sites = NULL;
  centroids = NULL;
  angles = NULL;
  init(10);
}

void Tiling::init(int n) {
  nTiles = n;
  if (sites != NULL)
    delete[] sites;
  if (centroids != NULL)
    delete[] centroids;
  if (angles != NULL)
    delete[] angles;

  sites = new Point2[n];
  centroids = new Point2[n];
  angles = new double[n];

  for (int i=0; i<n; i++) {
    double x = rand()/(double)RAND_MAX;
    double y = rand()/(double)RAND_MAX;
    sites[i].set(x,y);
    centroids[i].set(x,y);
    angles[i] = 0;
  }

  coneColours.initRandomly(n);
}


void Tiling::setOrients(int orientation) {
  int i;

  if (orientation == ORIENTATION_VERTICAL) {
    for (i=0; i<nTiles; i++) {
      angles[i] = 0;
    }
  }
  else if (orientation == ORIENTATION_RADIAL) {
    for (i=0; i<nTiles; i++) {
      double x = sites[i].x;
      double y = sites[i].y;
      double dx = x - 0.5;
      double dy = y - 0.5;
      angles[i] = atan2(dy, dx) * 180.0 / 3.1416;
    }
  }
}

void Tiling::moveSites() {
  for (int i=0; i<nTiles; i++) {
    sites[i] = centroids[i];
  }
}

void Tiling::computeCentroids(Image& image,
			      double sx, double sy,
			      double tx, double ty) {
  int k, r,g,b;
  int ix,iy;
  byte **buf,*pixp;

  buf = image.getBuf();
  int w = image.getWidth();
  int h = image.getHeight();

  double *sumX = new double[nTiles];
  double *sumY = new double[nTiles];
  double *nPix = new double[nTiles];

  for (k=0; k<nTiles; k++) {
    sumX[k] = sumY[k] = nPix[k] = 0.0;
  }

  for (iy=0; iy<h; iy++) {
    pixp = buf[iy];

    for (ix=0; ix<w; ix++) {
      r = *pixp++;
      g = *pixp++;
      b = *pixp++;
      pixp++;    // Skip the A byte of RGBA pixel.
      k = coneColours.findColour(r,g,b);

      if (k != -1) {
	sumX[k] += ix;
	sumY[k] += iy;
	nPix[k]++;
      }
    }
  }

  //
  // Ok, got the sums of pixel counts, pixel X's and pixel Y's.
  // The X's and Y's are all in device coords.  We need to
  // transform them to world [0-1] x [0-1] coords
  //
  for (k=0; k<nTiles; k++) {
    double area = nPix[k];

    //    fprintf(dbgfp,"#%d (sumX sumY nPix)=(%lf %lf %lf)\n",
    //	    k,sumX[k],sumY[k],nPix[k]);

    if (area == 0.0) {
      //
      // Due to overcrowding, there is no voronoi region
      // for this tile.  Can't do much about it, so just
      // set the centroid to be the same as the site.
      //
      centroids[k] = sites[k];
    }
    else {
      double xc = sumX[k] / area;
      double yc = sumY[k] / area;

      //      fprintf(dbgfp,"#%d (%lf %lf)\n",k,xc,yc);

      //
      // (xc yc) is in device coords.  Transform to world coords.
      //

      xc = sx * xc + tx;
      yc = sy * yc + ty;

      //      fprintf(dbgfp,"#%d (%lf %lf)\n",k,xc,yc);

      centroids[k].set(xc, yc);
    }
  }

  delete[] sumX;
  delete[] sumY;
  delete[] nPix;
}

