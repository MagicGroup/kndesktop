#include "kndesktop.h"

#include <KGlobal>
#include <KLocale>

#include "container.h"

KNDesktop::KNDesktop() : KUniqueApplication()
{
    disableSessionManagement();

    KGlobal::locale()->insertCatalog( "libkonq" );

    m_desktop = new Container;
    m_desktop->show();
}

KNDesktop::~KNDesktop()
{
    delete m_desktop;
}
