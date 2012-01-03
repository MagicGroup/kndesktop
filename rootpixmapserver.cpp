#include "rootpixmapserver.h"

#include <QDebug>
#include <QLabel>
#include <QList>
#include <QPixmap>
#include <QX11Info>
#include <X11/Xlib.h>

class RootPixmapServerPrivate
{
public:
    Atom pixmap;
//     QList<Atom> actives;
    QPixmap* qpixmap;
};

RootPixmapServer::RootPixmapServer()
: d(new RootPixmapServerPrivate)
{
    d->pixmap = XInternAtom( QX11Info::display(), "PIXMAP", false );
    d->qpixmap = 0;//new QPixmap( "/usr/share/wallpapers/NEDA/contents/images/800x600.jpg" );
}

RootPixmapServer::~RootPixmapServer()
{
    delete d;
}

void RootPixmapServer::setPixmap( QPixmap* qpixmap )
{
    d->qpixmap = qpixmap;

    Atom selection = XInternAtom( QX11Info::display(), "KDESHPIXMAP", false );
    XSetSelectionOwner( QX11Info::display(), selection, winId(), CurrentTime );
}

bool RootPixmapServer::x11Event( XEvent* event )
{
    // Handle SelectionRequest events by which a X client can request a
    // shared pixmap.

    if (event->type == SelectionRequest)
    {
        qWarning() << "SelectionRequest";
    XSelectionRequestEvent *ev = &event->xselectionrequest;

    // Build negative reply
    XEvent reply;
    reply.type = SelectionNotify;
    reply.xselection.display = QX11Info::display();
    reply.xselection.requestor = ev->requestor;
    reply.xselection.selection = ev->selection;
    reply.xselection.target = d->pixmap;
    reply.xselection.property = None;
    reply.xselection.time = ev->time;

    // Check if we know about this selection
//     Atom sel = ev->selection;
//     SelectionIterator it = m_Selections.find(sel);
//     if (it == m_Selections.end())
//         return false;
//     KSelectionInode si = it.data();

    // Only convert to pixmap
    if ( ev->target != d->pixmap ) {
        qWarning() << "illegal target\n";
        XSendEvent( QX11Info::display(), ev->requestor, false, 0, &reply );
        return true;
    }

    // Check if there is no transaction in progress to the same property
//     if ( d->actives.contains( ev->property ) ) {
//         qWarning() << "selection is busy.\n";
//         XSendEvent( QX11Info::display(), ev->requestor, false, 0, &reply );
//         return true;
//     }

    // Check if the selection was not deleted
//     DataIterator it2 = m_Data.find(si.handle);
//     if (it2 == m_Data.end())
//     {
//         qWarning() << ID << "selection has been deleted.\n";
//         XSendEvent(QX11Info::display(), ev->requestor, false, 0, &reply);
//         return true;
//     }

//     qWarning() << ID << "request for " << si.name << "\n";

    // All OK: pass the pixmap handle.
    Qt::HANDLE p = d->qpixmap->handle();
    XChangeProperty(QX11Info::display(), ev->requestor, ev->property, d->pixmap,
        32, PropModeReplace, (unsigned char*)(&p), 1);
//     it2.data().usecount++;
//     d->actives << ev->property;

    // Request PropertyNotify events for the target window
    // XXX: The target window better not be handled by us!
    XSelectInput(QX11Info::display(), ev->requestor, PropertyChangeMask);

    // Acknowledge to the client and return
    reply.xselection.property = ev->property;
    XSendEvent(QX11Info::display(), ev->requestor, false, 0, &reply);
    return true;
    }

    // ICCCM says that the target property is to be deleted by the
    // requestor. We are notified of this by a PropertyNotify. Only then, we
    // can actually delete the pixmap if it was removed.

    if (event->type == PropertyNotify)
    {
        qWarning() << "PropertyNotify";
//     XPropertyEvent *ev = &event->xproperty;

//     AtomIterator it = m_Active.find(ev->atom);
//     if (it == m_Active.end())
//         return false;
//     HANDLE handle = it.data();
//     d->actives.remove( it );

//     DataIterator it2 = m_Data.find(handle);
//     assert(it2 != m_Data.end());
//     it2.data().usecount--;
//     if (!it2.data().usecount && !it2.data().refcount)
//     {
//         delete it2.data().pixmap;
//         m_Data.remove(it2);
//     }
    return true;
    }

    // Handle SelectionClear events.

    if (event->type == SelectionClear)
    {
        qWarning() << "SelectionClear";
//     XSelectionClearEvent *ev = &event->xselectionclear;

//     SelectionIterator it = m_Selections.find(ev->selection);
//     if (it == m_Selections.end())
//         return false;

//     emit selectionCleared(it.data().name);
    return  true;
    }

    // Process further
    return false;
}
