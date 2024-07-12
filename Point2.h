#ifndef POINT2_H
#define POINT2_H

class Point2 {
public:
  double x,y;

  Point2();
  Point2(double x, double y);
  void set(double px, double py);
  void set(Point2& p);
  double getX() {return x;}
  double getY() {return y;}
  void add(Point2& b);
  void add(Point2& b, Point2& result);
  void sub(Point2& b, Point2& result);
  double dot(Point2& b);
  double dist(Point2& b);
  void scale(double s, Point2& result);
  void scale(double s);
  void normalize();
  void linComb(double s, Point2& b, Point2& result);
};

#endif
