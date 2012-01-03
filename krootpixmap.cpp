#include "krootpixmap.h"
#include "krootpixmap_p.h"

#include <QBrush>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QEvent>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QTimer>
#include <QWidget>

#include "krootpixmaploader.h"

KRootPixmap::KRootPixmap( QWidget* target, QObject* parent )
: QObject(parent),d(new KRootPixmapPrivate)
{
    d->target = target;
    d->toplevel = target->window();
    d->loader = new KRootPixmapLoader( d->target );
    d->timer = new QTimer;
    d->timer->setSingleShot( true );

    /// FIXME
    d->target->setAutoFillBackground( true );

    connect( d->loader, SIGNAL(done(bool, const QPixmap&)),
             this, SLOT(slotDone(bool, const QPixmap&)) );
    connect( d->timer, SIGNAL(timeout()),
             this, SLOT(repaint()) );

    d->toplevel->installEventFilter( this );
    d->target->installEventFilter( this );

    QDBusInterface* interface = new QDBusInterface(
        "org.kde.kndesktop", "/IconView",
        "org.kde.kndesktop.IconView",
        QDBusConnection::sessionBus(), this
    );
    connect( interface, SIGNAL(wallpaperChanged()),
             this, SLOT(slotWallpaperChanged()) );
}

KRootPixmap::~KRootPixmap()
{
    delete d->loader;
    delete d->timer;
    delete d;
}

bool KRootPixmap::eventFilter( QObject* object, QEvent* event )
{
    Q_UNUSED(object)

    switch ( event->type() ) {
        case QEvent::Resize:
            d->timer->start( 100 );
            break;
        case QEvent::Move:
            d->timer->start( 100 );
            break;
        case QEvent::Paint:
            d->timer->start( 0 );
            break;
        case QEvent::ParentChange:
            d->toplevel->removeEventFilter( this );
            d->toplevel = d->target->window();
            d->toplevel->installEventFilter( this );
            break;
        default:
            break;
    }

    return false;
}

void KRootPixmap::slotDone( bool success, const QPixmap& pixmap )
{
    if ( !success ) {
        return;
    }

    QPalette p( d->target->palette() );
    p.setBrush( QPalette::Window, QBrush( pixmap ) );
    d->target->setPalette( p );
}

void KRootPixmap::repaint()
{
    QPoint p1 = d->target->mapToGlobal( d->target->rect().topLeft() );
    QPoint p2 = d->target->mapToGlobal( d->target->rect().bottomRight() );
    QRect rect( p1, p2 );

    if ( d->rect == rect )
        return;

    d->rect = rect;
//     qWarning() << d->rect;

    d->loader->load( d->rect );
}

void KRootPixmap::slotWallpaperChanged()
{
    d->loader->load( d->rect );
}
