#ifndef KNDESKTOP_H
#define KNDESKTOP_H

#include <KUniqueApplication>

class Container;

class KNDesktop : public KUniqueApplication
{
    Q_OBJECT
    public:
        explicit KNDesktop();
        virtual ~KNDesktop();
    private:
        Container* m_desktop;
};

#endif // KNDESKTOP_H
