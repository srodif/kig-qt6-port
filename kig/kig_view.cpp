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
 Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "kig_view.h"
#include "kig_view.moc"

#include "kig_part.h"
#include "kig_document.h"
#include "kig_commands.h"
#include "../misc/coordinate_system.h"
#include "../misc/kiginputdialog.h"
#include "../misc/kigpainter.h"
#include "../modes/mode.h"
#include "../modes/dragrectmode.h"

#include <qdialog.h>
#include <qevent.h>
#include <q3whatsthis.h>
#include <qlayout.h>
#include <qscrollbar.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGridLayout>
#include <QResizeEvent>
#include <QPaintEvent>

#include <kdebug.h>
#include <kcursor.h>
#include <klocale.h>
#include <kapplication.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kiconloader.h>

#include <cmath>
#include <algorithm>

kdbgstream& operator<< ( kdbgstream& s, const QPoint& t )
{
  s << "x: " << t.x() << " y: " << t.y();
  return s;
}

KigWidget::KigWidget( KigPart* part,
                      KigView* view,
                      QWidget* parent,
                      const char* name,
                      bool fullscreen )
  : QWidget( parent, name,
             fullscreen ? Qt::WStyle_Customize | Qt::WStyle_NoBorder : 0 ),
    mpart( part ),
    mview( view ),
    stillPix(size()),
    curPix(size()),
    msi( Rect(), rect() ),
    misfullscreen( fullscreen )
{
  part->addWidget(this);

  setFocusPolicy(ClickFocus);
  setBackgroundMode( Qt::NoBackground );
  setMouseTracking(true);

  curPix.resize( size() );
  stillPix.resize( size() );
}

KigWidget::~KigWidget()
{
  mpart->delWidget( this );
}

void KigWidget::paintEvent(QPaintEvent*)
{
  updateEntireWidget();
}

void KigWidget::mousePressEvent (QMouseEvent* e)
{
  if( e->button() & Qt::LeftButton )
    return mpart->mode()->leftClicked( e, this );
  if ( e->button() & Qt::MidButton )
    return mpart->mode()->midClicked( e, this );
  if ( e->button() & Qt::RightButton )
    return mpart->mode()->rightClicked( e, this );
}

void KigWidget::mouseMoveEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return mpart->mode()->leftMouseMoved( e, this );
  if ( e->state() & Qt::MidButton )
    return mpart->mode()->midMouseMoved( e, this );
  if ( e->state() & Qt::RightButton )
    return mpart->mode()->rightMouseMoved( e, this );
  return mpart->mode()->mouseMoved( e, this );
}

void KigWidget::mouseReleaseEvent (QMouseEvent* e)
{
  if( e->state() & Qt::LeftButton )
    return mpart->mode()->leftReleased( e, this );
  if ( e->state() & Qt::MidButton )
    return mpart->mode()->midReleased( e, this );
  if ( e->state() & Qt::RightButton )
    return mpart->mode()->rightReleased( e, this );
}

void KigWidget::updateWidget( const std::vector<QRect>& overlay )
{
#undef SHOW_OVERLAY_RECTS
#ifdef SHOW_OVERLAY_RECTS
  QPainter debug (this, this);
  debug.setPen(Qt::yellow);
#endif // SHOW_OVERLAY_RECTS
  // we undo our old changes...
  for ( std::vector<QRect>::const_iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
    bitBlt( this, i->topLeft(), &curPix, *i );
  // we add our new changes...
  for ( std::vector<QRect>::const_iterator i = overlay.begin(); i != overlay.end(); ++i )
  {
    bitBlt( this, i->topLeft(), &curPix, *i );
#ifdef SHOW_OVERLAY_RECTS
    debug.drawRect(*i);
#endif
  };
  oldOverlay = overlay;
}

void KigWidget::updateEntireWidget()
{
  std::vector<QRect> overlay;
  overlay.push_back( QRect( QPoint( 0, 0 ), size() ) );
  updateWidget( overlay );
}

void KigWidget::resizeEvent( QResizeEvent* e )
{
  QSize osize = e->oldSize();
  QSize nsize = e->size();
  Rect orect = msi.shownRect();

  curPix.resize( nsize );
  stillPix.resize( nsize );
  msi.setViewRect( rect() );

  Rect nrect( 0., 0.,
              orect.width() * nsize.width() / osize.width(),
              orect.height() * nsize.height() / osize.height() );
  nrect = matchScreenShape( nrect );
  nrect.setCenter( orect.center() );
  msi.setShownRect( nrect );

  // horrible hack...  We need to somehow differentiate between the
  // resizeEvents we get on startup, and the ones generated by the
  // user.  The first require recentering the screen, the latter
  // don't..
  if ( nsize.width() / osize.width() > 4 ) recenterScreen();

  mpart->redrawScreen( this );
  updateScrollBars();
}

void KigWidget::updateCurPix( const std::vector<QRect>& ol )
{
  // we make curPix look like stillPix again...
  for ( std::vector<QRect>::const_iterator i = oldOverlay.begin(); i != oldOverlay.end(); ++i )
    bitBlt( &curPix, i->topLeft(), &stillPix, *i );
  for ( std::vector<QRect>::const_iterator i = ol.begin(); i != ol.end(); ++i )
    bitBlt( &curPix, i->topLeft(), &stillPix, *i );

  // we add ol to oldOverlay, so that part of the widget will be
  // updated too in updateWidget...
  std::copy( ol.begin(), ol.end(), std::back_inserter( oldOverlay ) );
}

void KigWidget::recenterScreen()
{
  msi.setShownRect( matchScreenShape( mpart->document().suggestedRect() ) );
}

Rect KigWidget::matchScreenShape( const Rect& r ) const
{
  return r.matchShape( Rect::fromQRect( rect() ) );
}

void KigWidget::slotZoomIn()
{
  Rect nr = msi.shownRect();
  Coordinate c = nr.center();
  nr /= 2;
  nr.setCenter( c );
  KigCommand* cd =
    new KigCommand( *mpart,
                    i18n( "Zoom In" ) );
  cd->addTask( new KigViewShownRectChangeTask( *this, nr ) );
  mpart->history()->addCommand( cd );
}

void KigWidget::slotZoomOut()
{
  Rect nr = msi.shownRect();
  Coordinate c = nr.center();
  nr *= 2;
  nr.setCenter( c );

  // zooming in is undoable..  I know this isn't really correct,
  // because the current view doesn't really belong to the document (
  // althought KGeo and KSeg both save them along, iirc ).  However,
  // undoing a zoom or another operation affecting the window seems a
  // bit too useful to not be available.  Please try to convince me if
  // you feel otherwise ;-)
  KigCommand* cd =
    new KigCommand( *mpart,
                    i18n( "Zoom Out" ) );
  cd->addTask( new KigViewShownRectChangeTask( *this, nr ) );
  mpart->history()->addCommand( cd );
}

void KigWidget::clearStillPix()
{
  stillPix.fill(Qt::white);
  oldOverlay.clear();
  oldOverlay.push_back ( QRect( QPoint(0,0), size() ) );
}

void KigWidget::redrawScreen( const std::vector<ObjectHolder*>& selection, bool dos )
{
  std::vector<ObjectHolder*> nonselection;
  std::set<ObjectHolder*> objs = mpart->document().objectsSet();
  std::set_difference( objs.begin(), objs.end(), selection.begin(), selection.end(),
                       std::back_inserter( nonselection ) );

  // update the screen...
  clearStillPix();
  KigPainter p( msi, &stillPix, mpart->document() );
  p.drawGrid( mpart->document().coordinateSystem(), mpart->document().grid(),
              mpart->document().axes() );
  p.drawObjects( selection, true );
  p.drawObjects( nonselection, false );
  updateCurPix( p.overlay() );
  if ( dos ) updateEntireWidget();
}

const ScreenInfo& KigWidget::screenInfo() const
{
  return msi;
}

const Rect KigWidget::showingRect() const
{
  return msi.shownRect();
}

const Coordinate KigWidget::fromScreen( const QPoint& p )
{
  return msi.fromScreen( p );
}

double KigWidget::pixelWidth() const
{
  return msi.pixelWidth();
}

const Rect KigWidget::fromScreen( const QRect& r )
{
  return msi.fromScreen( r );
}


void KigWidget::updateScrollBars()
{
  mview->updateScrollBars();
}

KigView::KigView( KigPart* part,
                  bool fullscreen,
                  QWidget* parent,
                  const char* name )
  : QWidget( parent, name ),
    mlayout( 0 ), mrightscroll( 0 ), mbottomscroll( 0 ),
    mupdatingscrollbars( false ),
    mrealwidget( 0 ), mpart( part )
{
  connect( part, SIGNAL( recenterScreen() ), this, SLOT( slotInternalRecenterScreen() ) );

  mlayout = new QGridLayout( this, 2, 2 );
  mrightscroll = new QScrollBar( Vertical, this, "Right Scrollbar" );
  // TODO: make this configurable...
  mrightscroll->setTracking( true );
  connect( mrightscroll, SIGNAL( valueChanged( int ) ),
           this, SLOT( slotRightScrollValueChanged( int ) ) );
  connect( mrightscroll, SIGNAL( sliderReleased() ),
           this, SLOT( updateScrollBars() ) );
  mbottomscroll = new QScrollBar( Horizontal, this, "Bottom Scrollbar" );
  connect( mbottomscroll, SIGNAL( valueChanged( int ) ),
           this, SLOT( slotBottomScrollValueChanged( int ) ) );
  connect( mbottomscroll, SIGNAL( sliderReleased() ),
           this, SLOT( updateScrollBars() ) );
  mrealwidget = new KigWidget( part, this, this, "Kig Widget", fullscreen );
  mlayout->addWidget( mbottomscroll, 1, 0 );
  mlayout->addWidget( mrealwidget, 0, 0 );
  mlayout->addWidget( mrightscroll, 0, 1 );

  resize( sizeHint() );
  mrealwidget->recenterScreen();
  part->redrawScreen( mrealwidget );
  updateScrollBars();
}

void KigView::updateScrollBars()
{
  // we update the scrollbars to reflect the new "total size" of the
  // document...  The total size is calced in entireDocumentRect().
  // ( it is calced as a rect that contains all the points in the
  // document, and then enlarged a bit, and scaled to match the screen
  // width/height ratio...
  // What we do here is tell the scroll bars what they should show as
  // their total size..

  // see the doc of this variable in the header for this...
  mupdatingscrollbars = true;

  Rect er = mrealwidget->entireDocumentRect();
  Rect sr = mrealwidget->screenInfo().shownRect();

  // we define the total rect to be the smallest rect that contains
  // both er and sr...
  er |= sr;

  // we need ints, not doubles, so since "pixelwidth == widgetcoord /
  // internalcoord", we use "widgetcoord/pixelwidth", which would then
  // equal "internalcoord", which has to be an int ( by definition.. )
  // i know, i'm a freak to think about these sorts of things... ;)
  double pw = mrealwidget->screenInfo().pixelWidth();

  // what the scrollbars reflect is the bottom resp. the left side of
  // the shown rect.  This is why the maximum value is not er.top()
  // (which would be the maximum value of the top of the shownRect),
  // but er.top() - sr.height(), which is the maximum value the bottom of
  // the shownRect can reach...

  int rightmin = static_cast<int>( er.bottom() / pw );
  int rightmax = static_cast<int>( ( er.top() - sr.height() ) / pw );

  mrightscroll->setMinValue( rightmin );
  mrightscroll->setMaxValue( rightmax );
  mrightscroll->setLineStep( (int)( sr.height() / pw / 10 ) );
  mrightscroll->setPageStep( (int)( sr.height() / pw / 1.2 ) );

  // note that since Qt has a coordinate system with the lowest y
  // values at the top, and we have it the other way around ( i know i
  // shouldn't have done this.. :( ), we invert the value that the
  // scrollbar shows.  This is inverted again in
  // slotRightScrollValueChanged()...
  mrightscroll->setValue( (int) ( rightmin + ( rightmax - ( sr.bottom() / pw ) ) ) );

  mbottomscroll->setMinValue( (int)( er.left() / pw ) );
  mbottomscroll->setMaxValue( (int)( ( er.right() - sr.width() ) / pw ) );
  mbottomscroll->setLineStep( (int)( sr.width() / pw / 10 ) );
  mbottomscroll->setPageStep( (int)( sr.width() / pw / 1.2 ) );
  mbottomscroll->setValue( (int)( sr.left() / pw ) );

  mupdatingscrollbars = false;
}

Rect KigWidget::entireDocumentRect() const
{
  return matchScreenShape( mpart->document().suggestedRect() );
}

void KigView::slotRightScrollValueChanged( int v )
{
  if ( ! mupdatingscrollbars )
  {
    // we invert the inversion that was done in updateScrollBars() (
    // check the documentation there..; )
    v = mrightscroll->minValue() + ( mrightscroll->maxValue() - v );
    double pw = mrealwidget->screenInfo().pixelWidth();
    double nb = double( v ) * pw;
    mrealwidget->scrollSetBottom( nb );
  };
}

void KigView::slotBottomScrollValueChanged( int v )
{
  if ( ! mupdatingscrollbars )
  {
    double pw = mrealwidget->screenInfo().pixelWidth();
    double nl = double( v ) * pw;
    mrealwidget->scrollSetLeft( nl );
  };
}

void KigWidget::scrollSetBottom( double rhs )
{
  Rect sr = msi.shownRect();
  Coordinate bl = sr.bottomLeft();
  bl.y = rhs;
  sr.setBottomLeft( bl );
  msi.setShownRect( sr );
  mpart->redrawScreen( this );
}

void KigWidget::scrollSetLeft( double rhs )
{
  Rect sr = msi.shownRect();
  Coordinate bl = sr.bottomLeft();
  bl.x = rhs;
  sr.setBottomLeft( bl );
  msi.setShownRect( sr );
  mpart->redrawScreen( this );
}

const ScreenInfo& KigView::screenInfo() const
{
  return mrealwidget->screenInfo();
}

KigView::~KigView()
{
}

KigWidget* KigView::realWidget() const
{
  return mrealwidget;
}

const KigDocument& KigWidget::document() const
{
  return mpart->document();
}

QSize KigWidget::sizeHint() const
{
  return QSize( 630, 450 );
}

void KigWidget::wheelEvent( QWheelEvent* e )
{
  int delta = e->delta();
  Qt::Orientation orient = e->orientation();
  if ( orient == Qt::Vertical )
    mview->scrollVertical( delta );
  else
    mview->scrollHorizontal( delta );
}

void KigView::scrollHorizontal( int delta )
{
  if ( delta >= 0 )
    for ( int i = 0; i < delta; i += 120 )
      mbottomscroll->subtractLine();
  else
    for ( int i = 0; i >= delta; i -= 120 )
      mbottomscroll->addLine();
}

void KigView::scrollVertical( int delta )
{
  if ( delta >= 0 )
    for ( int i = 0; i < delta; i += 120 )
      mrightscroll->subtractLine();
  else
    for ( int i = 0; i >= delta; i -= 120 )
      mrightscroll->addLine();
}

bool KigWidget::isFullScreen() const
{
  return misfullscreen;
}

void KigView::slotZoomIn()
{
  mrealwidget->slotZoomIn();
}

void KigView::slotZoomOut()
{
  mrealwidget->slotZoomOut();
}

void KigWidget::slotRecenterScreen()
{
  Rect nr = mpart->document().suggestedRect();
  KigCommand* cd =
    new KigCommand( *mpart,
                    i18n( "Recenter View" ) );

  cd->addTask( new KigViewShownRectChangeTask( *this, nr ) );
  mpart->history()->addCommand( cd );
}

void KigView::toggleFullScreen()
{
  mrealwidget->setFullScreen( ! mrealwidget->isFullScreen() );
  if ( mrealwidget->isFullScreen() )
    topLevelWidget()->showFullScreen();
  else
    topLevelWidget()->showNormal();
}

void KigWidget::setFullScreen( bool f )
{
  misfullscreen = f;
}

void KigWidget::zoomRect()
{
  mpart->emitStatusBarText( i18n( "Select the rectangle that should be shown." ) );
  DragRectMode d( *mpart, *this );
  mpart->runMode( &d );
  if ( ! d.cancelled() )
  {
    Rect nr = d.rect();
    KigCommand* cd =
      new KigCommand( *mpart,
                      i18n( "Change Shown Part of Screen" ) );

    cd->addTask( new KigViewShownRectChangeTask( *this, nr ) );
    mpart->history()->addCommand( cd );
  };

  mpart->redrawScreen( this );
  updateScrollBars();
}

void KigView::zoomRect()
{
  mrealwidget->zoomRect();
}

void KigWidget::setShowingRect( const Rect& r )
{
  msi.setShownRect( r.matchShape( Rect::fromQRect( rect() ) ) );
}

void KigView::slotRecenterScreen()
{
  mrealwidget->slotRecenterScreen();
}

void KigView::slotInternalRecenterScreen()
{
  mrealwidget->recenterScreen();
}

void KigWidget::zoomArea()
{
//  mpart->emitStatusBarText( i18n( "Select the area that should be shown." ) );
  Rect oldrect = showingRect();
  Coordinate tl = oldrect.topLeft();
  Coordinate br = oldrect.bottomRight();
  bool ok = true;
  KigInputDialog::getTwoCoordinates( i18n( "Select Zoom Area" ),
        i18n( "Select the zoom area by entering the coordinates of "
              "the upper left corner and the lower right corner." ) +
        QString::fromLatin1("<br>") +
        mpart->document().coordinateSystem().coordinateFormatNoticeMarkup(),
        this, &ok, mpart->document(), &tl, &br );
  if ( ok )
  {
    Coordinate nc1( tl.x, br.y );
    Coordinate nc2( br.x, tl.y );
    Rect nr( nc1, nc2 );
    KigCommand* cd = new KigCommand( *mpart, i18n( "Change Shown Part of Screen" ) );

    cd->addTask( new KigViewShownRectChangeTask( *this, nr ) );
    mpart->history()->addCommand( cd );
  }

  mpart->redrawScreen( this );
  updateScrollBars();
}

void KigView::zoomArea()
{
  mrealwidget->zoomArea();
}

