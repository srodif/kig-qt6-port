// macrowizard.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#include "macrowizard.h"
#include "macrowizard.moc"

#include "macro.h"

#include <kdebug.h>

MacroWizard::MacroWizard( QWidget* parent, DefineMacroMode* m )
  : MacroWizardBase( parent, "Define Macro Wizard", false ), mmode( m )
{
};

MacroWizard::~MacroWizard()
{
}

void MacroWizard::back()
{
  if ( currentPage() == mpfinal )
  {
    // currentPage() is not yet updated when we get here, so this
    // means that the page about to be shown is actually mpgiven...
    mmode->givenPageEntered();
  }
  else if ( currentPage() == mpname )
  {
    mmode->finalPageEntered();
  }
  MacroWizardBase::back();
}

void MacroWizard::next()
{
  if ( currentPage() == mpgiven )
  {
    // currentPage() is not yet updated when we get here, so this
    // means that the page about to be shown is actually mpgiven...
    kdDebug() << k_funcinfo << "mpgiven" << endl;
  }
  else if ( currentPage() == mpfinal )
  {
//    mmode->namePageEntered();
  }
  MacroWizardBase::next();
}

void MacroWizard::reject()
{
  MacroWizardBase::reject();
  mmode->cancelPressed();
}
