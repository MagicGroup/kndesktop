#ifndef CONTAINER_H
#define CONTAINER_H

#include <QWidget>

class IconView;

class Container : public QWidget
{
    Q_OBJECT
    public:
        explicit Container( QWidget* parent = 0 );
        virtual ~Container();
    private Q_SLOTS:
        void slotScreenResized();
    private:
        IconView* m_view;
};

#endif // CONTAINER_H
