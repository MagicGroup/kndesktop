#include "container.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QX11Info>

#include <KWindowSystem>

#include <netwm.h>

#include "iconview.h"

Container::Container( QWidget* parent ) : QWidget(parent)
{
    setAttribute( Qt::WA_X11NetWmWindowTypeDesktop );
    setFocusPolicy( Qt::NoFocus );
    setContextMenuPolicy( Qt::NoContextMenu );
    setGeometry( QApplication::desktop()->geometry() );
    //     lower();

    Display* display = QX11Info::display();
    Window root = RootWindow( display, 0 );
    XDefineCursor( display, root, cursor().handle() );

    KWindowSystem::setType( winId(), NET::Desktop );
    KWindowSystem::setState( winId(), NET::SkipPager );
    KWindowSystem::setOnAllDesktops( winId(), true );

    /// view
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    setLayout( layout );

    m_view = new IconView( this );
    layout->addWidget( m_view );
//     layout->addSpacing( 48 );

    connect( QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(slotScreenResized()) );
}

Container::~Container()
{
}

void Container::slotScreenResized()
{
    setGeometry( QApplication::desktop()->geometry() );
}
