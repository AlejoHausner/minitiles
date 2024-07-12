#include "ColourSet.h"
#include <math.h>
#include <stdio.h>

extern FILE* dbgfp;

//////////////////////////////////////////////////////
//
// Sets two parameters for defining a linear-probing
// hashing scheme.
//
void getHashFunc(int dataSize, int *tableSize, int *probeStep) {
  int p,s,d,m,found;

  //
  // Round the table size up to s, the next power of 2.
  //
  found = 0;
  for (s=2; s<2147483647; s*=2) {
    if (s > dataSize) {
      found = 1;
      break;
    }
  }
  if (!found) {
    fprintf(stderr,"Fatal: dataSize %d exceeds 2^31-1\n",
            dataSize);
    exit(1);
  }

  //
  // Now find a prime number which is at least 1.5 times bigger than s.
  //
  for (p=(s*3)/2; ; p++) {
    if (p % 2 == 0)
      continue;
    else {
      m = (int)sqrt((double)(p+1));
      found = 0;
      for (d=3; d<=m; d+=2) {
        if (p % d == 0) {
          found = 1;
          break;
        }
      }
      if (!found)
        break;
    }
  }
  *tableSize = p;
  *probeStep = s;
}

//////////////////////////////////////////////////////
//
// Reads a list of colours into the hash table.
//
void ColourSet::readFile(char *filename) {
  int r,g,b;
  int n;
  FILE *fp;
  if ((fp = fopen(filename,"r")) == NULL) {
    fprintf(stderr,"Can't open %s for read access\n",
            filename);
      exit(1);
  }
  fscanf(fp,"%d",&n);
  init(n);
  for (int i=0; i<n; i++) {
    fscanf(fp,"%d %d %d",&r, &g, &b);
    addColour(r,g,b, i);
  }
}

//////////////////////////////////////////////////////
//
// Given a color, returns its index (or -1 if not found).
//
int ColourSet::findColour(int r, int g, int b) {
  /*
   *  int ic = r << 16 | g << 8 | b;  // The hash key
   *  int a = ic % hashSize;          // the initial probe location.
   *  int d = ic % hashStep + 1;      // the secondary probe step size
   *  int i,k;
   *
   * //  fprintf(dbgfp,"findColour: c=(%d %d %d) a=%d d=%d\n",c.r,c.g,c.b, a,d);
   * //  fprintf(dbgfp,"hash contains:\n");
   * //  for (i=0; i<VDHashSize; i++) {
   * //    k = VDColourHash[i];
   * //    if (k == -1)
   * //      fprintf(dbgfp,"%d: %d\n",i,k);
   * //    else
   * //      fprintf(dbgfp,"%d: (%d %d %d)\n",i,colours[i].r,colours[i].g,colours[i].b);
   * //  }
   *
   *  for (i=0; i<hashSize; i++) {
   *        k = colourHash[a];
   *    if (k == -1)
   *      return -1;
   *    else {
   *      if (colours[k].equals(r,g,b))
   *        return k;
   *    }
   *    a = (a+d) % hashSize;
   *  }
   *  return -1;
   */

  int key = r << 16 | g << 8 | b;

  //  fprintf(dbgfp,"findColour: (r g b)=(%d %d %d) key=%d\n",r,g,b,key);

  int *loc = colourHash->get(key);
  if (loc == NULL) {
    return -1;
  }
  else {
    return *loc;
  }
}

//////////////////////////////////////////////////////
//
// Given a color, and an index, stores the index at hash(color)
//
//int ColourSet::insertColourInHash(int r, int g, int b, int loc) {
int ColourSet::insertColourInHash(Colour &c, int loc) {

  /*  
   *  int ic = r << 16 | g << 8 | b;
   *  int a = ic % hashSize;
   *  int d = ic % hashStep + 1;
   *  int i;
   *
   * //  cout << "ic=" << ic << " a=" << a << " d=" << d << endl;
   *
   *  for (i=0; i<hashSize; i++) {
   *
   * //      cout << "probing " << a;
   *
   *    if (colourHash[a] == -1) {
   *      colourHash[a] = loc;
   *      return a;
   *    }
   *    a = (a+d) % hashSize;
   *  }
   *  fprintf(stderr,"fatal: colour hash overflow\n");
   *  a = ic % hashSize;
   *  fprintf(stderr,"(r g b)=(%d %d %d) ic=%d a=%d d=%d hashSize=%d\n",
   *          r,g,b,ic,a,d,hashSize);
   *  for (i=0; i<hashSize; i++)
   *    fprintf(stderr,"%d %d\n",i,colourHash[i]);
   *
   *  exit(1);
   */

  int *key = new int;
  *key = c.getR() << 16 | c.getG() << 8 | c.getB();

  //  fprintf(dbgfp,"insertInHash: (r g b)=(%d %d %d) key=%d\n",c.getR(),c.getG(),c.getB(),*key);

  int *newLoc = new int;
  *newLoc = loc;

  colourHash->add(*key,*newLoc);
  return loc;
}

int hashColourKey(int& key, int n) {
  return key % n;
}


//////////////////////////////////////////////////////
//
// Default constructor.
//
ColourSet::ColourSet() {
  //  fprintf(dbgfp,"ColourSet default constructor.\n");
  //  fflush(dbgfp);

  ColourSet(10);
}

ColourSet::~ColourSet() {
  delete colourHash;
}  

//////////////////////////////////////////////////////
//
// Constructor for a given data size.
//
ColourSet::ColourSet(int n) {
  //  fprintf(dbgfp,"ColourSet explicit constructor n=%d.\n",n);
  //  fflush(dbgfp);

  //  colourHash = NULL;
  initColourHash(n);

  //  fprintf(dbgfp,"now, colourHash=%x\n",colourHash);
  //  fflush(dbgfp);

  init(n);

  //  fprintf(dbgfp,"now (constructor), colourHash=%x\n",colourHash);
  //  fflush(dbgfp);

}

//////////////////////////////////////////////////////
//
// Creates the hash table, and fills it with random
// colours.
//
void ColourSet::init(int dataSize) {
  colours = std::vector<Colour>();
  initColourHash(dataSize);
  initRandomly(dataSize);
}

//////////////////////////////////////////////////////
//
// Creates the hash table.
//
void ColourSet::initColourHash(int dataSize) {
  /*
   *  getHashFunc(dataSize, &hashSize, &hashStep);
   *
   *  //  fprintf(dbgfp,"initColourHash: dataSize=%d hashSize=%d hashStep=%d\n",
   *  //          dataSize,hashSize,hashStep);
   *  //  fprintf(dbgfp,"before re-alloc: colourHash=%x\n",colourHash);
   *  //  fflush(dbgfp);
   *
   *
   *  if (colourHash != NULL)
   *    delete[] colourHash;
   *  colourHash = new int[hashSize];
   *
   *  for (int i=0; i<hashSize; i++)
   *    colourHash[i] = -1;
   */
  colourHash = new Hash<int,int>(hashColourKey);
  colourHash->allocate(dataSize);
}

//////////////////////////////////////////////////////
//
// Generates n random colors, and stores them.
//
void ColourSet::initRandomly(int n) {
  int r,g,b, found,a;

  colours.clear();
  initColourHash(n);

  for (int i=0; i<n; i++) {

    //    printf("i=%d\n",i);

    found = 1;
    while (found) {
      r = byte(55.0 + 200.0*rand()/(double)RAND_MAX);
      g = byte(55.0 + 200.0*rand()/(double)RAND_MAX);
      b = byte(55.0 + 200.0*rand()/(double)RAND_MAX);
      if ((a=findColour(r,g,b)) == -1) {
        found = 0;
      }
    }
    addColour(r,g,b, i);
  }

  //  printf("ColourSet::initRandomly(%d): exiting\n",n);

}

//////////////////////////////////////////////////////
//
// Adds a color to the hash table, and also the
// list of colors.
//
void ColourSet::addColour(int r, int g, int b, int loc) {

  if (loc <= colours.size()) {
    Colour *c = new Colour;
    c->set(r,g,b);
    colours.push_back(*c);
    insertColourInHash(*c, loc);
  }
  else {
    fprintf(stderr,"Colour table overflow! \n");
    exit(1);
  }
}
//////////////////////////////////////////////////////
//
// Looks at all the colors in the list,
// and returns the color closest to the given rgb.
//
Colour *ColourSet::closest(int r, int g, int b) {
  int n = size();
  int dist,minDist = 100000;
  int k = 0;
  Colour *c;

  for (int i=0; i<n; i++) {
    c = at(i);
    dist = c->dist(r,g,b);
    if (dist < minDist) {
      k = i;
      minDist = dist;
    }
  }
  return at(k);
}

//////////////////////////////////////////////////////
//
// For debugging.
//
void ColourSet::dump() {
  // Iterator is used to loop through the vector.
  vector<Colour>::iterator iter;

  int i=0;
  for (iter = colours.begin(); iter != colours.end(); iter++) {
    Colour c = *iter;
    fprintf(dbgfp,"%2d(%3d %3d %3d)",i,c.getR(),c.getG(),c.getB());
    if (i % 5 == 4)
      fprintf(dbgfp,"\n");
    i++;
  }
  fprintf(dbgfp,"\n");
}

