// circle_type.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "circle_type.h"

#include "circle_imp.h"
#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/common.h"

static const char* constructcirclethroughpointstat = I18N_NOOP( "Construct a circle through this point" );

static const ArgParser::spec argsspecCircleBCP[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a circle with this center" ) },
  { ObjectImp::ID_PointImp, constructcirclethroughpointstat }
};

CircleBCPType::CircleBCPType()
  : ObjectABType( "CircleBCP", argsspecCircleBCP, 2 )
{
}

CircleBCPType::~CircleBCPType()
{
}

const CircleBCPType* CircleBCPType::instance()
{
  static const CircleBCPType s;
  return &s;
}

ObjectImp* CircleBCPType::calc( const Coordinate& a, const Coordinate& b ) const
{
  return new CircleImp( a, ( b - a ).length() );
}

const CircleBTPType* CircleBTPType::instance()
{
  static const CircleBTPType t;
  return &t;
}

static const ArgParser::spec argsspecCircleBTP[] =
{
  { ObjectImp::ID_PointImp, constructcirclethroughpointstat },
  { ObjectImp::ID_PointImp, constructcirclethroughpointstat },
  { ObjectImp::ID_PointImp, constructcirclethroughpointstat }
};

CircleBTPType::CircleBTPType()
  : ArgparserObjectType( "CircleBTP", argsspecCircleBTP, 3 )
{
}

CircleBTPType::~CircleBTPType()
{
}

ObjectImp* CircleBTPType::calc( const Args& targs, const KigDocument& ) const
{
  if ( targs.size() < 2 || targs.size() > 3 ) return new InvalidImp;
  Args args = margsparser.parse( targs );
  if ( ! args[0] || ! args[1] || ( targs.size() == 3 && !args[2] ) )
    return new InvalidImp;
  const Coordinate a = static_cast<const PointImp*>( args[0] )->coordinate();
  const Coordinate b = static_cast<const PointImp*>( args[1] )->coordinate();
  Coordinate c;
  if ( args[2] )
    c = static_cast<const PointImp*>( args[2] )->coordinate();
  else
  {
    // we pick the third point so that the three points form a
    // triangle with equal sides...

    // midpoint:
    Coordinate m = ( b + a ) / 2;
    if ( b.y != a.y )
    {
      // direction of the perpend:
      double d = -(b.x-a.x)/(b.y-a.y);

      // length:
      // sqrt( 3 ) == tan( 60� ) == sqrt( 2^2 - 1^2 )
      double l = 1.73205080756 * (a-b).length() / 2;

      double d2 = d*d;
      double l2 = l*l;
      double dx = sqrt( l2 / ( d2 + 1 ) );
      double dy = sqrt( l2 * d2 / ( d2 + 1 ) );
      if( d < 0 ) dy = -dy;

      c.x = m.x + dx;
      c.y = m.y + dy;
    }
    else
    {
      c.x = m.x;
      c.y = m.y + ( a.x - b.x );
    };
  };

  const Coordinate center = calcCenter( a, b, c );
  if ( center.valid() )
    return new CircleImp( center, (center - a ).length() );
  else return new InvalidImp;
}

int CircleBCPType::resultId() const
{
  return ObjectImp::ID_CircleImp;
}

int CircleBTPType::resultId() const
{
  return ObjectImp::ID_CircleImp;
}

static const ArgParser::spec argsspecCircleBPR[] =
{
  { ObjectImp::ID_PointImp, I18N_NOOP( "Construct a circle with this center" ) },
  { ObjectImp::ID_DoubleImp, "UNUSED" }
};

CircleBPRType::CircleBPRType()
  : ArgparserObjectType( "CircleBPR", argsspecCircleBPR, 2 )
{
}

CircleBPRType::~CircleBPRType()
{
}

const CircleBPRType* CircleBPRType::instance()
{
  static const CircleBPRType t;
  return &t;
}

ObjectImp* CircleBPRType::calc( const Args& args, const KigDocument& ) const
{
  const Args a = margsparser.parse( args );
  if ( ! a[0] || ! a[1] ) return new InvalidImp;
  assert( a[0]->inherits( ObjectImp::ID_PointImp ) );
  const Coordinate c = static_cast<const PointImp*>( a[0] )->coordinate();
  assert( a[1]->inherits( ObjectImp::ID_DoubleImp ) );
  double r = static_cast<const DoubleImp*>( a[1] )->data();
  return new CircleImp( c, r );
}

int CircleBPRType::resultId() const
{
  return ObjectImp::ID_CircleImp;
}
