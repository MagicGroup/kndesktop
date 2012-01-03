#include "iconview.h"
#include "iconviewadaptor.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDir>
#include <QItemSelectionModel>
#include <QTimer>
#include <QPainter>

#include <KAction>
#include <KBookmarkManager>
#include <KDirModel>
#include <KDirLister>
#include <KDirSortFilterProxyModel>
#include <KFileDialog>
#include <KFileItemDelegate>
#include <KIcon>
#include <KUrl>
#include <KMenu>
#include <KStandardAction>
#include <knewmenu.h>
#include <knewfilemenu.h>
#include <kio/fileundomanager.h>
#include <kio/paste.h>
#include <konqmimedata.h>
#include <konq_operations.h>
#include <konq_popupmenu.h>

#define DEFAULT_WALLPAPER "/opt/kde4/share/wallpapers/stripes.png"
#include <KFileDialog>

#include "rootpixmapserver.h"

IconView::IconView( QWidget* parent ) : QListView(parent),m_pixmap(0),m_actionCollection(this)
{
    setViewMode( QListView::IconMode );
    setFlow( QListView::TopToBottom );
    setSelectionRectVisible( true );
    setGridSize( QSize( 64, 64 ) );
    setSpacing( 2 );
    setUniformItemSizes( true );
    setEditTriggers( QAbstractItemView::EditKeyPressed );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setFrameShape( QFrame::NoFrame );
    setAutoFillBackground( true );
//     setAttribute(Qt::WA_PaintOutsidePaintEvent);
//     setStyleSheet( "background-color: transparent" );
    setObjectName( "iconview" );

    new IconViewAdaptor( this );
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject( "/IconView", this );
    dbus.registerService( "org.kde.kndesktop" );

    /// lazy initialization
    QTimer::singleShot( 0, this, SLOT(init()) );
}

IconView::~IconView()
{
    delete m_pixmap;
    delete m_rootPixmapServer;
}

void IconView::init()
{
    KConfig config;
    KConfigGroup generalGroup( &config, "General" );
    m_wallpaper = generalGroup.readEntry( "Wallpaper", DEFAULT_WALLPAPER );

    delete m_pixmap;
    m_pixmap = new QPixmap( m_wallpaper );
    QPalette p;
    p.setBrush( QPalette::Base, QBrush( *m_pixmap ) );
    setPalette( p );

    emit wallpaperChanged();

    m_rootPixmapServer = new RootPixmapServer;
    m_rootPixmapServer->setPixmap( m_pixmap );

    m_model = new KDirModel( this );
    KDirLister* lister = new KDirLister( this );
    lister->openUrl( KUrl( QDir::homePath() ) );
    m_model->setDirLister( lister );

    m_delegate = new KFileItemDelegate( this );
    m_delegate->setMaximumSize( QSize( 64, 64 ) );
    m_delegate->setShadowOffset( QPointF( 1, 1 ) );
    m_delegate->setShadowColor( QColor( 0xff, 0xff, 0xff ) );
    m_delegate->setShadowBlur( 1 );
    m_delegate->setWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );

    m_proxyModel = new KDirSortFilterProxyModel( this );
    m_proxyModel->setSourceModel( m_model );

    setModel( m_proxyModel );
    setItemDelegate( m_delegate );

    m_selectionModel = new QItemSelectionModel( m_proxyModel );
    setSelectionModel( m_selectionModel );

    /// create actions
    KAction* cut = KStandardAction::cut( this, SLOT(cut()), this );
    KAction* copy = KStandardAction::copy( this, SLOT(copy()), this );

    KIO::FileUndoManager* manager = KIO::FileUndoManager::self();
    KAction* undo = KStandardAction::undo( manager, SLOT(undo()), this );
    connect( manager, SIGNAL(undoAvailable(bool)), undo, SLOT(setEnabled(bool)) );
    connect( manager, SIGNAL(undoTextChanged(const QString&)), SLOT(undoTextChanged(const QString&)) );
    undo->setEnabled( manager->undoAvailable() );

    KAction* paste = KStandardAction::paste( this, SLOT(paste()), this );
    KAction* pasteTo = KStandardAction::paste( this, SLOT(pasteTo()), this );
    pasteTo->setEnabled( false ); // Only enabled during popupMenu()

    QString actionText = KIO::pasteActionText();
    if ( !actionText.isEmpty() )
        paste->setText( actionText );
    else
        paste->setEnabled( false );

    KAction* refresh = new KAction(KIcon("user-desktop"), i18n("&Refresh Desktop"), this);
    connect( refresh, SIGNAL(triggered()), this, SLOT(refresh()) );

    KAction* wallpaper = new KAction(KIcon("tools-wizard"), i18n("&Change Wallpaper..."), this);
    connect( wallpaper, SIGNAL(triggered()), this, SLOT(changeWallpaper()) );

    KAction* rename = new KAction(KIcon("edit-rename"), i18n("&Rename"), this);
    rename->setShortcut( Qt::Key_F2 );
    connect( rename, SIGNAL(triggered()), SLOT(rename()) );

    KAction* trash = new KAction(KIcon("user-trash"), i18n("&Move to Trash"), this);
    trash->setShortcut( Qt::Key_Delete );
    connect( trash, SIGNAL(triggered(Qt::MouseButtons, Qt::KeyboardModifiers)),
             this, SLOT(moveToTrash(Qt::MouseButtons, Qt::KeyboardModifiers)) );

    KAction* del = new KAction(KIcon("edit-delete"), i18n("&Delete"), this);
    del->setShortcut( Qt::SHIFT + Qt::Key_Delete );
    connect( del, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()) );

    KAction* emptyTrash = new KAction(KIcon("trash-empty"), i18n("&Empty Trash Bin"), this);
    KConfig trashConfig( "trashrc", KConfig::SimpleConfig );
    emptyTrash->setEnabled( !trashConfig.group( "Status" ).readEntry( "Empty", true ) );
    connect( emptyTrash, SIGNAL(triggered()), this, SLOT(emptyTrashBin()) );

    // Create the new menu
    m_newMenu = new KNewFileMenu(&m_actionCollection, "new_menu", this);
    connect( m_newMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(aboutToShowCreateNew()) );

    m_actionCollection.addAction( "undo", undo );
    m_actionCollection.addAction( "cut", cut );
    m_actionCollection.addAction( "copy", copy );
    m_actionCollection.addAction( "paste", paste );
    m_actionCollection.addAction( "pasteto", pasteTo );
    m_actionCollection.addAction( "refresh", refresh );
    m_actionCollection.addAction( "wallpaper", wallpaper );
    m_actionCollection.addAction( "rename", rename );
    m_actionCollection.addAction( "trash", trash );
    m_actionCollection.addAction( "del", del );
    m_actionCollection.addAction( "empty_trash", emptyTrash );
}

bool IconView::setWallpaper( const QString& wallpaper )
{
    if ( wallpaper.isEmpty() || !QImage().load( wallpaper ) )
        return false;

    m_wallpaper = wallpaper;
    delete m_pixmap;
    m_pixmap = new QPixmap( m_wallpaper );

    m_rootPixmapServer->setPixmap( m_pixmap );

    QPalette p;
    p.setBrush( QPalette::Base, QBrush( *m_pixmap ) );
    setPalette( p );

    KConfig config;
    KConfigGroup generalGroup( &config, "General" );
    generalGroup.writeEntry( "Wallpaper", m_wallpaper );
    generalGroup.config()->sync();

    emit wallpaperChanged();

    return true;
}

QString IconView::wallpaper() const
{
    return m_wallpaper;
}

void IconView::changeWallpaper()
{
    QString file = KFileDialog::getOpenFileName();
    setWallpaper( file );
}

void IconView::contextMenuEvent( QContextMenuEvent* event )
{
    QModelIndex index = indexAt( event->pos() );
    if ( !index.isValid() || m_selectionModel->selectedIndexes().isEmpty() ) {
        QMenu* menu = new QMenu;
        menu->addAction( m_actionCollection.action( "new_menu" ) );
        menu->addSeparator();
        menu->addAction( m_actionCollection.action( "undo" ) );
        menu->addAction( m_actionCollection.action( "paste" ) );
        menu->addSeparator();
        menu->addAction( m_actionCollection.action( "refresh" ) );
        menu->addSeparator();
        menu->addAction( m_actionCollection.action( "wallpaper" ) );
        if ( event->reason() == QContextMenuEvent::Keyboard )
            menu->exec( QPoint( 0, 0 ) );
        else
            menu->exec( event->pos() );
        delete menu;
        return;
    }

    KFileItemList items;
    foreach ( const QModelIndex &index, m_selectionModel->selectedIndexes() ) {
        KFileItem item = m_model->itemForIndex( m_proxyModel->mapToSource( index ) );
        if ( !item.isNull() )
            items.append( item );
    }

    QAction* pasteTo = m_actionCollection.action( "pasteto" );
    if ( pasteTo ) {
        pasteTo->setEnabled( m_actionCollection.action( "paste" )->isEnabled() );
        pasteTo->setText( m_actionCollection.action( "paste" )->text() );
    }

    QList<QAction*> editActions;
    editActions.append( m_actionCollection.action( "rename" ) );
    editActions.append( m_actionCollection.action( "trash" ) );
    KConfigGroup configGroup( KGlobal::config(), "KDE" );
    bool showDeleteCommand = configGroup.readEntry( "ShowDeleteCommand", false );
    if ( showDeleteCommand )
        editActions.append( m_actionCollection.action( "del" ) );
    KParts::BrowserExtension::ActionGroupMap actionGroups;
    actionGroups.insert( "editactions", editActions );
    KParts::BrowserExtension::PopupFlags flags = KParts::BrowserExtension::ShowProperties;
    flags |= KParts::BrowserExtension::ShowUrlOperations;
    KonqPopupMenu* contextMenu = new KonqPopupMenu( items, KUrl(QDir::homePath()), m_actionCollection, m_newMenu,
                                                    KonqPopupMenu::ShowNewWindow, flags,
                                                    QApplication::desktop(),
                                                    KBookmarkManager::userBookmarksManager(),
                                                    actionGroups );
    contextMenu->exec( event->pos() );
    delete contextMenu;
}

void IconView::mouseDoubleClickEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton ) {
        const QModelIndex index = indexAt( event->pos() );
        if ( !index.isValid() )
            return;
        const KFileItem item = m_model->itemForIndex( m_proxyModel->mapToSource( index ) );
        item.run();
        m_selectionModel->clearSelection();
    }
}

void IconView::cut()
{
    QMimeData* mimeData = m_proxyModel->mimeData( m_selectionModel->selectedIndexes() );
    KonqMimeData::addIsCutSelection( mimeData, true );
    QApplication::clipboard()->setMimeData( mimeData );
}

void IconView::copy()
{
    QMimeData* mimeData = m_proxyModel->mimeData( m_selectionModel->selectedIndexes() );
    QApplication::clipboard()->setMimeData( mimeData );
}

void IconView::paste()
{
    KonqOperations::doPaste( QApplication::desktop(), KUrl(QDir::homePath()) );
}
void IconView::pasteTo()

{
    KUrl::List urls;
    foreach (const QModelIndex &index, m_selectionModel->selectedIndexes()) {
        KFileItem item = m_model->itemForIndex( m_proxyModel->mapToSource( index ) );
        urls.append( item.url() );
    }
    Q_ASSERT(urls.count() == 1);
    KonqOperations::doPaste( QApplication::desktop(), urls.first() );
}

void IconView::rename()
{
    /// simulate F2 key press
    QApplication::sendEvent( this, new QKeyEvent( QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier ) );
}

void IconView::moveToTrash( Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers )
{
    if ( modifiers & Qt::ShiftModifier ) {
        deleteSelectedItems();
        return;
    }

    KUrl::List urls;
    foreach (const QModelIndex &index, m_selectionModel->selectedIndexes()) {
        KFileItem item = m_model->itemForIndex( m_proxyModel->mapToSource( index ) );
        urls.append( item.url() );
    }
    KonqOperations::del( QApplication::desktop(), KonqOperations::TRASH, urls );
}

void IconView::deleteSelectedItems()
{
    KUrl::List urls;
    foreach (const QModelIndex &index, m_selectionModel->selectedIndexes()) {
        KFileItem item = m_model->itemForIndex( m_proxyModel->mapToSource( index ) );
        urls.append( item.url() );
    }
    KonqOperations::del( QApplication::desktop(), KonqOperations::DEL, urls );
}

void IconView::emptyTrashBin()
{
    KonqOperations::emptyTrash( QApplication::desktop() );
}

void IconView::refresh()
{
    m_model->dirLister()->updateDirectory( KUrl(QDir::homePath()) );
}

void IconView::undoTextChanged( const QString& text )
{
    if ( QAction* action = m_actionCollection.action( "undo" ) )
        action->setText( text );
}

void IconView::aboutToShowCreateNew()
{
    if ( m_newMenu ) {
        m_newMenu->checkUpToDate();
        m_newMenu->setPopupFiles( KUrl(QDir::homePath()) );
    }
}

