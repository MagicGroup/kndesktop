#ifndef KROOTPIXMAP_P_H
#define KROOTPIXMAP_P_H

#include <QRect>

class QTimer;
class QWidget;
class KRootPixmapLoader;

class KRootPixmapPrivate
{
    public:
        QWidget* target;
        QWidget* toplevel;
        KRootPixmapLoader* loader;
        QTimer* timer;
        QRect rect;
};

#endif // KROOTPIXMAP_P_H
