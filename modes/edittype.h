/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2004  Dominique Devriese <devriese@kde.org>
 Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KIG_MODES_EDITTYPE_H
#define KIG_MODES_EDITTYPE_H

#include "edittypebase.h"

#include <kiconloader.h>

class EditType : public EditTypeBase
{
  Q_OBJECT

  QString mname;
  QString mdesc;
  QString micon;
  const KIconLoader* il;
public:
  EditType( QWidget* parent, QString name = QString::null, QString desc = QString::null, QString icon = QString::null );
  ~EditType();
  const QString name() const;
  const QString description() const;
  const QString icon() const;

public slots:
  void helpSlot();
  void okSlot();
  void cancelSlot();
};

#endif
