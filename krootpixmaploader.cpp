#include "krootpixmaploader.h"

#include <QDebug>
#include <QRect>
#include <QX11Info>

#include "krootpixmaploader_p.h"

#include <X11/Xlib.h>

KRootPixmapLoader::KRootPixmapLoader( QWidget* parent )
: d(new KRootPixmapLoaderPrivate)
{
    d->q = this;
    d->window = new KRootPixmapLoaderWindow( d, parent );
    d->pixmap = XInternAtom( QX11Info::display(), "PIXMAP", false );
    d->target = XInternAtom( QX11Info::display(), "KSHAREDPIXMAPTARGET", false );
    d->selection = None;

    connect( d->window, SIGNAL(done(bool,const QPixmap&)), this, SIGNAL(done(bool,const QPixmap&)) );
}

KRootPixmapLoader::~KRootPixmapLoader()
{
    delete d->window;
    delete d;
}

bool KRootPixmapLoader::load( const QRect& rect )
{
    d->rect = rect;

    if ( d->selection != None ) {
        /// already active
        return false;
    }

    /// invalidate
    d->qpixmap = QPixmap();

    d->selection = XInternAtom( QX11Info::display(), "KDESHPIXMAP", true );
    if ( d->selection == None ) {
        /// no such pixmap
        return false;
    }

    int owner = XGetSelectionOwner( QX11Info::display(), d->selection );
    if ( owner == None ) {
        /// no window owns the pixmap
        d->selection = None;
        return false;
    }

    XConvertSelection( QX11Info::display(), d->selection, d->pixmap, d->target,
                       d->window->winId(), CurrentTime );
    return true;
}

bool KRootPixmapLoaderWindow::x11Event( XEvent* event )
{
    if ( event->type != SelectionNotify )
        return false;

    XSelectionEvent* ev = &(event->xselection);
    if ( ev->selection != d->selection )
        return false;

    if ( ev->target != d->pixmap || ev->property == None ) {
        qWarning() << "illegal selection notify event.";
        d->selection = None;
        emit done( false, d->qpixmap );
        return true;
    }

    /// read pixmap handle from ev->property
    Atom type;
    int format;
    unsigned long nitems;
    unsigned long bytesAfter;
    unsigned char* pixmapProperty = 0;
    XGetWindowProperty( QX11Info::display(), winId(), ev->property, 0, 1, false, d->pixmap,
                        &type, &format, &nitems, &bytesAfter, &pixmapProperty );
    if ( nitems != 1 || !pixmapProperty ) {
        qWarning() << "could not read property, nitems = " << nitems;
        emit done( false, d->qpixmap );
        return true;
    }

    /// get geometry
    Window root;
    int x, y;
    unsigned int w, h;
    unsigned int borderWidth;
    unsigned int depth;
    Drawable pixmap = *(Drawable*)pixmapProperty;
    Status status = XGetGeometry( QX11Info::display(), pixmap,
                                  &root, &x, &y, &w, &h, &borderWidth, &depth );
    if ( status == BadDrawable ) {
        return false;
    }

    int sx = d->rect.x();
    int sy = d->rect.y();
    int tx = 0;
    int ty = 0;
    if ( sx < 0 ) {
        tx = -sx;
        sx = 0;
    }
    if ( sy < 0 ) {
        ty = -sy;
        sy = 0;
    }

    int tw = x + w - sx;
    int th = y + h - sy;
    if ( tw <= 0 || th <= 0 ) {
        qWarning() << "no intersection";
        d->selection = None;
        emit done( false, d->qpixmap );
        return true;
    }

// qWarning() << sx << sy << tw << th << tx << ty;

    /// set proper pixmap size
    resize( tw, th );
    d->qpixmap = QPixmap( tw, th );

    /// copy pixmap area to target rect
    XCopyArea( QX11Info::display(), pixmap, d->qpixmap.handle(), DefaultGC( QX11Info::display(), 0 ),
                sx, sy, tw, th, tx, ty );

    XFree( pixmapProperty );
    XDeleteProperty( QX11Info::display(), winId(), ev->property );
    d->selection = None;
    emit done( true, d->qpixmap );
    return true;
}
