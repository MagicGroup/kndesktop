#ifndef KROOTPIXMAPLOADER_P_H
#define KROOTPIXMAPLOADER_P_H

#include <QWidget>
#include <X11/Xdefs.h>

class KRootPixmapLoaderPrivate;
class KRootPixmapLoaderWindow : public QWidget
{
    Q_OBJECT
    public:
        explicit KRootPixmapLoaderWindow( KRootPixmapLoaderPrivate* data, QWidget* parent )
        : QWidget(parent),d(data) {}
    Q_SIGNALS:
        void done( bool success, const QPixmap& pixmap );
    protected:
        bool x11Event( XEvent* event );
    private:
        KRootPixmapLoaderPrivate* d;
};

class KRootPixmapLoader;
class KRootPixmapLoaderPrivate
{
    public:
        KRootPixmapLoader* q;
        KRootPixmapLoaderWindow* window;
        QPixmap qpixmap;
        Atom pixmap;
        Atom target;
        Atom selection;
        QRect rect;
};

#endif // KROOTPIXMAPLOADER_P_H
