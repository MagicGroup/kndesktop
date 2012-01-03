#ifndef KROOTPIXMAPLOADER_H
#define KROOTPIXMAPLOADER_H

#include <QObject>
class QPixmap;
class QRect;
class QWidget;

class KRootPixmapLoaderPrivate;
class KRootPixmapLoader : public QObject
{
    Q_OBJECT
    public:
        explicit KRootPixmapLoader( QWidget* parent );
        virtual ~KRootPixmapLoader();
        bool load( const QRect& rect );
    Q_SIGNALS:
        void done( bool success, const QPixmap& pixmap );
    private:
        KRootPixmapLoaderPrivate* const d;
};

#endif // KROOTPIXMAPLOADER_H
