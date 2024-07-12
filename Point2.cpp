#include "Point2.h"
#include <math.h>

Point2::Point2() {
//  cout << "Point2 default constr." << endl;

  Point2(0,0);
}

Point2::Point2(double x, double y) {
  set(x,y);
}

void Point2::set(double x, double y) {
  this->x = x;
  this->y = y;
}

void Point2::set(Point2& p) {
  set(p.x, p.y);
}

void Point2::add(Point2& b, Point2& result) {
  result.x = x + b.x;
  result.y = y + b.y;
}

void Point2::add(Point2& b) {
  x += b.x;
  y += b.y;
}

void Point2::sub(Point2& b, Point2& result) {
  result.x = x - b.x;
  result.y = y - b.y;
}

double Point2::dot(Point2& b) {
  return x * b.x + y * b.y;
}

double Point2::dist(Point2& b) {
  double dx = x - b.x;
  double dy = y - b.y;
  return sqrt(dx*dx + dy*dy);
}

void Point2::scale(double s, Point2& result) {
  result.x = x * s;
  result.y = y * s;
}

void Point2::scale(double s) {
  x = x * s;
  y = y * s;
}

void Point2::normalize() {
  double d = dist(*this);
  if (d > 0.0)
    scale(1.0/d, *this);
}

void Point2::linComb(double s, Point2& b, Point2& result) {
  result.x = x + s * b.x;
  result.y = y + s * b.y;
}
