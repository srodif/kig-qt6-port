// drgeo-filter-chooser.cc
// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#include "drgeo-filter-chooser.h"
#include "drgeo-filter-chooser.moc"

#include <kiconloader.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

KigFilterDrgeoChooser::KigFilterDrgeoChooser( const QStringList& l )
  : KigFilterDrgeoChooserBase( 0, "drgeo_filter", true )
{
  KIconLoader* li = KGlobal::iconLoader();
  OKButton->setIconSet( QIconSet( li->loadIcon( "button_ok", KIcon::Small ) ) );
  CancelButton->setIconSet( QIconSet( li->loadIcon( "button_cancel", KIcon::Small ) ) );

  FigureListBox->insertStringList( l );

  connect( OKButton, SIGNAL( clicked() ), SLOT( slotOKPressed() ) );
  connect( CancelButton, SIGNAL( clicked() ), SLOT( slotCancelPressed() ) );
  connect( FigureListBox, SIGNAL( executed( QListBoxItem* ) ), SLOT( slotExecuted( QListBoxItem* ) ) );
}

void KigFilterDrgeoChooser::slotOKPressed()
{
  const int r = FigureListBox->currentItem();
  if ( r == -1 )
  {
    KMessageBox::sorry( 0, i18n( "Please select a figure." ) );
    return;
  }
  done( r );
}

void KigFilterDrgeoChooser::slotCancelPressed()
{
  done( -1 );
}

KigFilterDrgeoChooser::~KigFilterDrgeoChooser()
{

}

void KigFilterDrgeoChooser::slotExecuted( QListBoxItem* i )
{
  done( FigureListBox->index( i ) );
}
