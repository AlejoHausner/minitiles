#ifndef COLOURSET_H
#define COLOURSET_H

#ifdef WIN32
#include <windows.h>
#endif

#include "Colour.h"
#include "Hash.h"

#include <vector>
using namespace std;

class ColourSet {
public:
  ColourSet();
  ~ColourSet();
  ColourSet(int size);
  void addColour(int r, int g, int b, int loc);
  int insertColourInHash(Colour& c, int loc);
  int findColour(int r, int g, int b);
  void readFile(char *filename);
  void readFile(FILE *infp);
  void init(int size);
  void initColourHash(int size);
  void initRandomly(int size);
  Colour *at(int i) {return &colours[i];};
  int size() {return colours.size();};
  Colour *closest(int r, int g, int b);
  void dump();
  void clear() {
    colours.clear();
    colourHash->clear();
  };

private:
  std::vector<Colour> colours;
  Hash<int,int>* colourHash;
};

#endif
