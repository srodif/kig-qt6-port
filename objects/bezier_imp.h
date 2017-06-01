// Copyright (C)  2009  Petr Gajdos <pgajdos@suse.cz> and
// Maurizio Paolini <paolini@dmf.unicatt.it>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_OBJECTS_BEZIER_IMP_H
#define KIG_OBJECTS_BEZIER_IMP_H

#include "curve_imp.h"

#include "object_imp.h"
#include "../misc/coordinate.h"
#include <vector>

/**
 * An ObjectImp representing polynomial Bézier Curve.
 */
class BezierImp
  : public CurveImp
{
  uint mnpoints;
  std::vector<Coordinate> mpoints;
  Coordinate mcenterofmass;
  
  Coordinate deCasteljau( unsigned int m, unsigned int k, double p ) const;

public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the BezierImp type.
   */
  static const ObjectImpType* stype();
  static const ObjectImpType* stype2();
  static const ObjectImpType* stype3();

  /**
   * Constructs a Bézier curve.
   */
  BezierImp( const std::vector<Coordinate>& points );
  ~BezierImp();
  BezierImp* copy() const Q_DECL_OVERRIDE;

  Coordinate attachPoint() const Q_DECL_OVERRIDE;
  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;
  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold, 
                              const KigDocument& doc ) const;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  /**
   * Returns the vector with control points.
   */
  const std::vector<Coordinate> points() const; 
  /**
   * Returns the center of mass of the control polygon.
   */
  const Coordinate centerOfMass() const; 
  /**
   * Returns the number of control points.
   */
  uint npoints() const;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
};

/**
 * An ObjectImp representing a rational Bézier curve.
 */
class RationalBezierImp
  : public CurveImp
{
  uint mnpoints;
  std::vector<Coordinate> mpoints;
  std::vector<double> mweights;
  Coordinate mcenterofmass;
  
  Coordinate deCasteljauPoints( unsigned int m, unsigned int k, double p ) const;
  double deCasteljauWeights( unsigned int m, unsigned int k, double p ) const;

public:
  typedef CurveImp Parent;
  /**
   * Returns the ObjectImpType representing the RationalBezierImp type.
   */
  static const ObjectImpType* stype();
  static const ObjectImpType* stype2();
  static const ObjectImpType* stype3();

  /**
   * Constructs a rational Bézier curve.
   */
  RationalBezierImp( const std::vector<Coordinate>& points, const std::vector<double>& weights );
  ~RationalBezierImp();
  RationalBezierImp* copy() const Q_DECL_OVERRIDE;

  Coordinate attachPoint() const Q_DECL_OVERRIDE;
  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  const Coordinate getPoint( double param, const KigDocument& ) const Q_DECL_OVERRIDE;
  bool containsPoint( const Coordinate& p, const KigDocument& doc ) const Q_DECL_OVERRIDE;
  bool internalContainsPoint( const Coordinate& p, double threshold, 
                              const KigDocument& doc ) const;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  /**
   * Returns the vector with control points.
   */
  const std::vector<Coordinate> points() const; 
  /**
   * Returns the center of mass of the control polygon.
   */
  const Coordinate centerOfMass() const; 
  /**
   * Returns the number of control points.
   */
  uint npoints() const;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
};


#endif
