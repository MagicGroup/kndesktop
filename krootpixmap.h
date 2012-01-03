#ifndef KROOTPIXMAP_H
#define KROOTPIXMAP_H

#include <QObject>
#include "krootpixmap_export.h"

class QPixmap;
class QWidget;

class KRootPixmapPrivate;
class KROOTPIXMAP_EXPORT KRootPixmap : public QObject
{
    Q_OBJECT
    public:
        explicit KRootPixmap( QWidget* target, QObject* parent = 0 );
        virtual ~KRootPixmap();
    protected:
        virtual bool eventFilter( QObject* object, QEvent* event );
    private Q_SLOTS:
        void slotDone( bool success, const QPixmap& pixmap );
        void repaint();
        void slotWallpaperChanged();
    private:
        KRootPixmapPrivate* const d;
};

#endif // KROOTPIXMAP_H
