/**
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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


#include "typesdialog.h"
#include "typesdialog.moc"

#include "../kig/kig_part.h"
#include "../misc/lists.h"
#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"
#include "edittype.h"

#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qpixmap.h>

#include <vector>
#include <algorithm>

TypesDialog::TypesDialog( QWidget* parent, const KigDocument& doc )
  : TypesDialogBase( parent, "types_dialog", true ), mdoc( doc )
{
  // improving GUI look'n'feel...
  il = doc.instance()->iconLoader();
  buttonHelp->setIconSet( QIconSet( il->loadIcon( "help", KIcon::Small ) ) );
  buttonOk->setIconSet( QIconSet( il->loadIcon( "button_ok", KIcon::Small ) ) );
  buttonRemove->setIconSet( QIconSet( il->loadIcon( "editdelete", KIcon::Small ) ) );

  typeList->setColumnWidth( 0, 22 );
  typeList->setColumnWidth( 1, 140 );
  typeList->setColumnWidth( 2, 240 );

  // loading macros...
  typedef MacroList::vectype vec;
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_reverse_iterator i = macros.rbegin(); i != macros.rend(); ++i )
  {
    typeList->insertItem( newListItem( *i ) );
  };
}

QListViewItem* TypesDialog::newListItem( Macro* m )
{
  MacroListElement* e = new MacroListElement( typeList, m );
  QCString ifn = m->action->iconFileName();
  if ( !ifn.isNull() )
  {
    QPixmap p = il->loadIcon( ifn, KIcon::Small );
    e->setPixmap( 0, p );
  }
  return e;
}

TypesDialog::~TypesDialog()
{
}

void TypesDialog::helpSlot()
{
  kapp->invokeHelp( QString::fromLatin1( "working-with-types" ),
                    QString::fromLatin1( "kig" ) );
}

void TypesDialog::okSlot()
{
  accept();
}

void TypesDialog::deleteType()
{
  std::vector<QListViewItem*> items;
  std::vector<Macro*> selectedTypes;
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      items.push_back( it.current() );
      selectedTypes.push_back( static_cast<MacroListElement*>( it.current() )->getMacro() );
    }
    ++it;
  }
  if (selectedTypes.empty()) return;
  if (selectedTypes.size()==1)
  {
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete the type named \"%1\"?").arg(selectedTypes.front()->action->descriptiveName()),
	 i18n("Are you sure?"),
	 KStdGuiItem::cont(),
	 "deleteTypeWarning") ==KMessageBox::Cancel ) return;
  }
  else
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete these %1 types?").arg(selectedTypes.size()),
	 i18n("Are you sure?"),
	 KStdGuiItem::cont(),
	 "deleteTypeWarning") == KMessageBox::Cancel ) return;
  for ( std::vector<QListViewItem*>::iterator i = items.begin(); i != items.end(); ++i)
  {
    int appel = typeList->itemIndex(*i);
    assert (appel != -1);
    delete *i;
  };
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      types.push_back( static_cast<MacroListElement*>( it.current() )->getMacro() );
    }
    ++it;
  }
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Export Types" ) );
  if ( file_name.isNull() )
    return;
  QFile fi( file_name );
  if ( fi.exists() )
    if ( KMessageBox::questionYesNo( 0, i18n( "The file \"%1\" already exists. "
                                       "Do you wish to overwrite it?" ).arg( fi.name() ),
                                       i18n( "Overwrite file?" ) ) == KMessageBox::No )
       return;
  MacroList::instance()->save( types, file_name );
}

void TypesDialog::importTypes()
{
  QStringList file_names =
    KFileDialog::getOpenFileNames(":importTypes", i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Import Types" ));

  std::vector<Macro*> macros;

  for ( QStringList::Iterator i = file_names.begin();
        i != file_names.end(); ++i)
  {
    std::vector<Macro*> nmacros;
    bool ok = MacroList::instance()->load( *i, nmacros, mdoc );
    if ( ! ok )
      continue;
    std::copy( nmacros.begin(), nmacros.end(), std::back_inserter( macros ) );
  };
  MacroList::instance()->add( macros );

  for ( uint i = 0; i < macros.size(); ++i )
    typeList->insertItem( newListItem( macros[i] ) );
}

QString TypesDialog::fetchIconFromListItem( QListViewItem* i )
{
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      Macro* ai = static_cast<MacroListElement*>( i )->getMacro();
      Macro* ait = static_cast<MacroListElement*>( it.current() )->getMacro();
      if ( ai == ait )
      {
        return ai->ctor->iconFileName( true );
      }
    }
    ++it;
  }
  return "gear";
}

void TypesDialog::executed( QListViewItem* i )
{
  EditType* d = new EditType( this, i->text( 1 ), i->text( 2 ), fetchIconFromListItem( i ) );
  if ( d->exec() )
  {
/*
    QString newname = d->name();
    QString newdesc = d->description();
    QString newicon = d->icon();

    index = typeList->itemIndex( i );
    typedef MacroList::vectype vec;
    const vec& macros = MacroList::instance()->macros();
    ObjectHierarchy* hierarchy = macros[index]->ctor->hierarchy();
    char* name = macros[index]->action->actionName();
    MacroConstructor* ctor =
        new MacroConstructor( *hierarchy, name.latin1(), newdesc.latin1(), newicon );
    GUIAction* act = new ConstructibleAction( ctor, newname );
    Macro* macro = new Macro( act, ctor );

    // TODO: implement insert for MacroList and, of course, for ObjectConstructorList
    // and GUIActionList...
    MacroList::instance()->insert( macros, index );

    delete typeList->itemAtIndex( index );
    typeList->insertItem( newListItem( macros[i] ), index );
*/
  }
  delete d;
}

MacroListElement::MacroListElement( KListView* lv, Macro* m )
  : QListViewItem( lv, QString::null, m->action->descriptiveName(), m->action->description() ),
    macro( m )
{
}
