#ifndef ROOTPIXMAPSERVER_H
#define ROOTPIXMAPSERVER_H

#include <QWidget>
class QPixmap;
class RootPixmapServerPrivate;
class RootPixmapServer : public QWidget
{
    Q_OBJECT
    public:
        explicit RootPixmapServer();
        virtual ~RootPixmapServer();
        void setPixmap( QPixmap* qpixmap );
    protected:
        bool x11Event( XEvent* event );
    private:
        RootPixmapServerPrivate* const d;
};

#endif // ROOTPIXMAPSERVER_H
