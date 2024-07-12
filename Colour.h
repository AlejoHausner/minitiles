#ifndef COLOUR_H
#define COLOUR_H

#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdlib.h>

typedef unsigned char byte;


class Colour {
public:
  unsigned char r,g,b;

  Colour() {
    set(0,0,0);
  }

  Colour(byte r, byte g, byte b) {
    set(r,g,b);
  }

  void set(int rr, int gg, int bb) {
    r = rr;
    g = gg;
    b = bb;
  }
    
  void set(float r, float g, float b) {
    set((int)(r*255), (int)(g*255), (int)(b*255));
  }

  void set(const Colour &c) {
    set(c.r, c.g, c.b);
  }

  bool equals(Colour &c) {
    return r == c.r && g == c.g && b == c.b;
  };
  
  bool equals(int rr, int gg, int bb) {
    return r == rr && g == gg && b == bb;
  };

  inline byte getR() {return r;};
  inline byte getG() {return g;};
  inline byte getB() {return b;};

  int dist(int ar, int ag, int ab) {
    int dr = abs(ar - r);
    int dg = abs(ag - g);
    int db = abs(ab - b);
    return dr + dg + db;
  }

  bool operator==(Colour& c) { 
    return equals(c);
  };

  Colour& operator=(const Colour& other) {
    if (this == &other)
      return *this;
    this->set(other);
    return *this;
  }

  static Colour Red() {
    Colour result;
    result.set(255,0,0);
    return result;
  }

  static Colour Green() {
    Colour result;
    result.set(0,255,0);
    return result;
  }

  static Colour Blue() {
    Colour result;
    result.set(0,0,255);
    return result;
  }

  static Colour White() {
    Colour result;
    result.set(255,255,255);
    return result;
  }

  static Colour Black() {
    Colour result;
    result.set(0,0,0);
    return result;
  }

  static Colour Yellow() {
    Colour result;
    result.set(255,255,0);
    return result;
  }
  static Colour Cyan() {
    Colour result;
    result.set(0,255,255);
    return result;
  }

  static Colour Magenta() {
    Colour result;
    result.set(255,0,255);
    return result;
  }
};


#endif
