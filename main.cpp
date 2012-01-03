#include <KAboutData>
#include <KCmdLineArgs>
#include <KDebug>
#include <kdeversion.h>
#include <KLocale>

#include "kndesktop.h"

int main( int argc, char** argv )
{
    KAboutData aboutData( "kndesktop", 0, ki18n("KDE Desktop"),
                          KDE_VERSION_STRING, ki18n("The KDE desktop"), KAboutData::License_GPL,
                          ki18n("(c) 2010, Ni Hui") );
    aboutData.addAuthor( ki18n("Ni Hui"), ki18n("Current maintainer"), "shuizhuyuanluo@126.com" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    if ( !KNDesktop::start() )
    {
        kError() << "kndesktop is already running!" << endl;
        return 0;
    }

    KNDesktop kndesktop;
    return kndesktop.exec();
}
