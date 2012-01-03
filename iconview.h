#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <QListView>
#include <KActionCollection>

class QItemSelectionModel;
class KDirModel;
class KDirSortFilterProxyModel;
class KFileItemDelegate;
class KNewFileMenu;

class RootPixmapServer;
class IconView : public QListView
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kndesktop.IconView")
    public:
        explicit IconView( QWidget* parent = 0 );
        virtual ~IconView();
    public Q_SLOTS:
        bool setWallpaper( const QString& wallpaper );
        QString wallpaper() const;
    Q_SIGNALS:
        void wallpaperChanged() const;
    private Q_SLOTS:
        void init();
        void changeWallpaper();
    protected:
        virtual void contextMenuEvent( QContextMenuEvent* event );
        virtual void mouseDoubleClickEvent( QMouseEvent* event );
    private Q_SLOTS:
        void cut();
        void copy();
        void paste();
        void pasteTo();
        void rename();
        void moveToTrash( Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers );
        void deleteSelectedItems();
        void emptyTrashBin();
        void refresh();
        void undoTextChanged( const QString& text );
        void aboutToShowCreateNew();
    private:
        QString m_wallpaper;
        QPixmap* m_pixmap;
        RootPixmapServer* m_rootPixmapServer;
        KDirModel* m_model;
        KFileItemDelegate* m_delegate;
        QItemSelectionModel* m_selectionModel;
        KDirSortFilterProxyModel* m_proxyModel;
        KActionCollection m_actionCollection;
        KNewFileMenu* m_newMenu;
};

#endif // ICONVIEW_H
