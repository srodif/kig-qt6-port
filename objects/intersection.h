#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "point.h"

class Segment;
class Line;
class Circle;

class IntersectionPoint
  : public Point
{
  Segment* segment1;
  Segment* segment2;
  Line* line1;
  Line* line2;
  Circle* circle1;
  Circle* circle2;
public:
  IntersectionPoint() : segment1(0), segment2(0), line1(0), line2(0), circle1(0), circle2(0) {};
  ~IntersectionPoint() {};
  virtual QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return "IntersectionPoint"; };

  std::map<QCString,double> getParams() { return std::map<QCString, double>(); };
  void setParams( const std::map<QCString,double>& /*m*/) {};

  void moved( ) { calc(); };
  QString wantArg(const Object* o) const;
  bool selectArg(Object* o);
  void unselectArg(Object* o);
  Objects getParents() const;
  
  void startMove(const Coordinate& ) {};
  void moveTo(const Coordinate& ) {};
  void stopMove() {};
  void cancelMove() {};
  
  void calc();
};

#endif
