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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_FILTERS_KSEG_FILTER_H
#define KIG_FILTERS_KSEG_FILTER_H

#include "filter.h"

class ObjectCalcer;

class KigFilterKSeg
  : public KigFilter
{
  KigFilterKSeg();
  ~KigFilterKSeg();

  ObjectCalcer* transformObject( KigDocument& kigdoc,
                                 std::vector<ObjectCalcer*>& parents,
                                 int subtype, bool& ok );

public:
  static KigFilterKSeg* instance();

  bool supportMime ( const QString& mime ) Q_DECL_OVERRIDE;
  KigDocument* load ( const QString& fromfile ) Q_DECL_OVERRIDE;
};

#endif
