// guiaction.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_GUIACTION_H
#define KIG_MISC_GUIACTION_H

#include <config.h>

#include <qstring.h>
#include <qcstring.h>
#include <kaction.h>

class GUIAction;
class KigDocument;

class KigGUIAction
  : public KAction
{
  Q_OBJECT
  GUIAction* mact;
  KigDocument& mdoc;
public:
  KigGUIAction( GUIAction* act,
                KigDocument& doc,
                QObject* parent );
  void slotActivated();

  GUIAction* guiAction();

  void plug( KigDocument* doc );
};

class GUIAction
{
public:
  virtual ~GUIAction();

  virtual QString description() const = 0;
  virtual QCString iconFileName() const = 0;
  virtual QString descriptiveName() const = 0;
  virtual const char* actionName() const = 0;
  virtual int shortcut() const = 0;
  virtual void act( KigDocument& ) = 0;

  virtual void plug( KigDocument* doc, KigGUIAction* kact );
};

class ObjectConstructor;

class ConstructibleAction
  : public GUIAction
{
  ObjectConstructor* mctor;
  QCString mactionname;
  int mshortcut;
public:
  ConstructibleAction( ObjectConstructor* ctor, const QCString& actionname,
                       int shortcut = 0 );
  ~ConstructibleAction();
  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  int shortcut() const;
  void act( KigDocument& );
  void plug( KigDocument* doc, KigGUIAction* kact );
};

class ConstructPointAction
  : public GUIAction
{
  const char* mactionname;
public:
  ConstructPointAction( const char* actionname );
  ~ConstructPointAction();

  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  int shortcut() const;
  void act( KigDocument& );
};

class ConstructTextLabelAction
  : public GUIAction
{
  const char* mactionname;
public:
  ConstructTextLabelAction( const char* actionname );

  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  int shortcut() const;
  void act( KigDocument& );
};

class AddFixedPointAction
  : public GUIAction
{
  const char* mactionname;
public:
  AddFixedPointAction( const char* actionname );
  ~AddFixedPointAction();
  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  int shortcut() const;
  void act( KigDocument& );
};

#if 0
class TestAction
  : public GUIAction
{
  const char* mactionname;
public:
  TestAction( const char* actionname );
  ~TestAction();
  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  void act( KigDocument& );
};
#endif

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

class NewScriptAction
  : public GUIAction
{
  const char* mactionname;
  const char* mdescname;
  const char* mdescription;
  const char* micon;
public:
  NewScriptAction( const char* actionname, const char* descname, const char* description, const char* icon );
  ~NewScriptAction();
  QString description() const;
  QCString iconFileName() const;
  QString descriptiveName() const;
  const char* actionName() const;
  void act( KigDocument& );
  int shortcut() const;
};

#endif // KIG_ENABLE_PYTHON_SCRIPTING

#endif
