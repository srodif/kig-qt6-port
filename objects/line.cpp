#include "line.h"

#include <klocale.h>
#include <kdebug.h>

#include "segment.h"

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a>b)?b:a)
#define SIGN(x) (x < 0 ? -1 : 1)

bool Line::contains(const QPoint& o, bool strict) const
{
    if ( !complete ) return false;
    // check your math theory ( homogeneous co�rdinates ) for this
    double tmp = fabs( o.x() * (qp1.getY()-qp2.getY()) + o.y()*(qp2.getX()-qp1.getX()) + x1y2_y1x2 );
    return tmp < (( strict ? 4 : 2 )* (qp2-Point(qp1)).length()); // if o is on the line ( if the determinant of the matrix
      //       |---|---|---|
      //       | x | y | z |
      //       |---|---|---|
      //       | x1| y1| z1|
      //       |---|---|---|
      //       | x2| y2| z2|
      //       |---|---|---|
      // equals 0, then p(x,y,z) is on the line containing points p1(x1,y1,z1) and p2
      // here, we're working with normal coords, no homogeneous ones, so all z's equal 1
}

void Line::draw(QPainter& p, bool showSelection) const
{
  if ( !shown ) return;
  p.setPen( QPen( Qt::blue, 1 ));
  if (showSelection && selected)
    p.setPen (QPen (Qt::red,1));
  drawLineTTP(p,qp1, qp2);
}

bool Line::inRect(const QRect& p) const
{
    if ( !complete ) return false;
    // TODO: implement for real...
    if ( p.contains( qp1.toQPoint() ) || p.contains( qp2.toQPoint() ) ) return true;
    return false;
}

Point Line::getPoint(double p) const
{
  // this code is copied from the KSeg source (with some
  // modifications), thanks to the author Ilya Baran
  double c = (p - 0.5) * 2;

  if(c >= 0) c = pow(c, 1./81.) / 2.;
  else c = -pow(-c, 1./81.) / 2.;

  c = c * M_PI;

  return qp1 + Point(qp1-qp2).normalize() * tan(c);
};

double Line::getParam(const Point& p) const
{
  // this code is copied from the KSeg source (with some
  // modifications), thanks to the author Ilya Baran
  Point dir = p-Point(qp1);
  double c = atan((p - p1).length()) * SIGN((p - qp1).getX()/(qp1 - qp2).getX());
  c/=M_PI;
  c = pow (2*c,81);
  //  return c / M_PI * (1 + SMALL) + 0.5;
  return c / 2 +0.5;
}

void Line::calcVars()
{
  x1y2_y1x2 = qp1.getX()*qp2.getY() - qp1.getY()*qp2.getX();
}

LineTTP::~LineTTP()
{
}

QString LineTTP::wantArg( const Object* o) const
{
  if ( !o->toPoint()) return 0;
  if ( !p1 ) return i18n( "Point 1" );
  if ( !p2 ) return i18n( "Point 2" );
  return 0;
}

bool LineTTP::selectArg(Object* o)
{
  Point* p=o->toPoint();
  assert(p);
  assert (!(p1 && p2));
  if ( !p1 ) p1 = p;
  else p2 = p;
  o->addChild(this);
  if (p2) calc();
  return complete=p2;
}

void LineTTP::unselectArg(Object* which)
{
  if ( which == p1 )
    {
      p1 = p2; p2 = 0;
      which->delChild(this);
    }
  if ( which == p2 )
    {
      p2 = 0;
      which->delChild(this);
    };
  complete = false;
}

void LineTTP::startMove(const QPoint& p)
{
  pwwsm = p;
  assert ( p1 && p2 );
  p1->startMove( p );
  p2->startMove( p );
}

void LineTTP::moveTo(const QPoint& p)
{
  p1->moveTo( p );
  p2->moveTo( p );
}

void LineTTP::stopMove()
{
}

// void LineTTP::cancelMove()
// {
//   p1->cancelMove();
//   p2->cancelMove();
//   calc();
// }

void LineTTP::calc()
{
  qp1 = p1->toQPoint();
  qp2 = p2->toQPoint();
  calcVars();
};

LinePerpend::~LinePerpend()
{
}

QString LinePerpend::wantArg( const Object* o) const
{
  if (o->toSegment() && !segment && !line ) return i18n("On segment");
  if (o->toLine() && !segment && !line ) return i18n("On line");
  if (o->toPoint() && !point) return i18n("Through point");
  return 0;
}

bool LinePerpend::selectArg(Object* o)
{
  Segment* s;
  assert (!(point && (line || segment)));
  if ((s= o->toSegment()))
    segment = s;
  Line* l;
  if ((l = o->toLine()))
    line = l;
  Point* p;
  if ((p = o->toPoint()))
    point = p;
  o->addChild(this);
  if (point && (line || segment)) { complete = true; };
  return complete;
}

// void LinePerpend::unselectArg(Object* o)
// {
//   if ( o == segment) { segment->delChild(this); segment = 0; complete = false; return;};
//   if ( o == line) { line->delChild(this); line = 0; complete = false; return;};
//   if ( o == point) { point->delChild(this); point = 0; complete = false; return;};
// }
void LinePerpend::startMove(const QPoint&)
{

}
void LinePerpend::moveTo(const QPoint&)
{

}
void LinePerpend::stopMove()
{

}
void LinePerpend::cancelMove()
{

}

void LinePerpend::calc()
{
  assert (point && (segment || line));
  qp1 = point->toQPoint();
  Point qpt1, qpt2;
  if (segment)
    {
      qpt1 = segment->getP1();
      qpt2 = segment->getP2();
    }
  else
    {
      qpt1 = line->getP1();
      qpt2 = line->getP2();
    };
  double x2,y2; // coords of another point on the perpend line
  double xa = qpt1.getX();
  double xb = qpt2.getX();
  double ya = qpt1.getY();
  double yb = qpt2.getY();
  if (yb != ya)
    {
      x2 = 0;
      y2 = (xb-xa)*(qp1.getX())/(yb-ya) + qp1.getY();
    }
  else // the line/segment = horizontal, so the perpend is vertical
    {
      x2 = qp1.getX();
      y2 = 0;
    };
  qp2 = Point(x2,y2);
  calcVars();
}

Objects LineTTP::getParents() const return objs;
{
  objs.push( p1 );
  objs.push( p2 );
}

Objects LinePerpend::getParents() const return objs;
{
    if ( segment ) objs.push( segment );
    else objs.push( line );
    objs.push( point );
}

void LineTTP::drawPrelim( QPainter& p, const QPoint& pt) const
{
  if (!p1 || !shown) return;
  p.setPen (QPen (Qt::red,1));
  drawLineTTP(p, *p1, Point(pt));
}

void LinePerpend::drawPrelim( QPainter& p, const QPoint& pt) const
{
  if (!(segment || line) || !shown) return;
  p.setPen (QPen (Qt::red,1));
  Point qpt1, qpt2;
  if (segment)
    {
      qpt1 = segment->getP1();
      qpt2 = segment->getP2();
    }
  else
    {
      qpt1 = line->getP1();
      qpt2 = line->getP2();
    };
  
  drawLineTTP(p, Point(pt), calcPointOnPerpend(qpt1, qpt2, Point(pt)));
}

void Line::lineOverlay(const Point& p1, const Point& p2, QPtrList<QRect>& list, const QRect& border)
{
  double xa = p1.getX(), xb = p2.getX(), ya = p1.getY(), yb = p2.getY();
  calcEndPoints (xa, xb, ya, yb, border);
  Segment::segmentOverlay(Point(xa,ya), Point(xb,yb), list, border);
};

void LineTTP::getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const
{
  if (!p1 || !shown) return;
  double xa = p1->getX(), xb = pt.x(), ya = p1->getY(), yb = pt.y();
  calcEndPoints (xa, xb, ya, yb, border);
  Segment::segmentOverlay(Point(xa,ya), Point(xb,yb), list, border);
}

void LinePerpend::getPrelimOverlay(QPtrList<QRect>& list, const QRect& border, const QPoint& pt) const
{
  if (!(segment || line) || !shown) return;
  Point qpt1, qpt2;
  if (segment)
    {
      qpt1 = segment->getP1();
      qpt2 = segment->getP2();
    }
  else
    {
      qpt1 = line->getP1();
      qpt2 = line->getP2();
    };
  
  Point p1(pt);
  Point p2 = calcPointOnPerpend(qpt1, qpt2, p1);
  double xa = p1.getX(), xb = p2.getX(), ya = p1.getY(), yb = p2.getY();
  calcEndPoints (xa, xb, ya, yb, border);
  Segment::segmentOverlay(Point(xa,ya), Point(xb,yb), list, border);
}

void Line::calcEndPoints(double& xa, double& xb, double& ya, double& yb, const QRect& r)
{
  // we calc where the line through a(xa,ya) and b(xb,yb) intersects with r:
  double left = (r.left()-xa)*(yb-ya)/(xb-xa)+ya;
  double right = (r.right()-xa)*(yb-ya)/(xb-xa)+ya;
  double top = (r.top()-ya)*(xb-xa)/(yb-ya)+xa;
  double bottom = (r.bottom()-ya)*(xb-xa)/(yb-ya)+xa;

  // now we go looking for valid points
  int novp = 0; // number of valid points we have already found

  if (!(top <= r.left() || top -2 >= r.right())) {
    // the line intersects with the top side of the rect.
    ++novp;
    xa = top; ya = r.top();
  };
  if (!(left <= r.top() || left-2 >= r.bottom())) {
    // the line intersects with the left side of the rect.
    if (novp) { xb = r.left(); yb=left; }
    else { xa = r.left(); ya=left; };
    ++novp;
  };
  if (!(right <= r.top() || right-2 >= r.bottom())) {
    // the line intersects with the right side of the rect.
    if (novp) { xb = r.right(); yb=right; }
    else { xa = r.right(); ya=right; };
    ++novp;
  };
  if (!(bottom <= r.left() || bottom-2 >= r.right())) {
    // the line intersects with the bottom side of the rect.
    ++novp;
    xb = bottom; yb = r.bottom();
  };
  if (novp < 2) {
    kdError()<< k_funcinfo << "novp < 2 :(( " << endl;
    xa = r.left(); ya=r.top(); xb=r.right(); yb=r.bottom();
  };
};

Point LinePerpend::calcPointOnPerpend (const Point& p1, const Point& p2, const Point& q)
{
  double x2,y2; // coords of another point on the perpend line
  double xa = p1.getX();
  double xb = p2.getX();
  double ya = p1.getY();
  double yb = p2.getY();
  if (yb != ya)
    {
      x2 = 0;
      y2 = (xb-xa)*(q.getX())/(yb-ya) + q.getY();
    }
  else // the line/segment = horizontal, so the perpend is vertical
    {
      x2 = q.getX();
      y2 = 0;
    };
  return Point(x2,y2);
};


void Line::drawLineTTP (QPainter& p, const Point& q, const Point& r)
{
  double xa= q.getX();
  double xb = r.getX();
  double ya = q.getY();
  double yb = r.getY();
  calcEndPoints(xa, xb, ya, yb, p.window());
  p.drawLine(xa,ya,xb,yb);
};
