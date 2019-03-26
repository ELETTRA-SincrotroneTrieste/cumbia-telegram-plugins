#ifndef CUTELEGRAM_BOOKMARKS_PLUGIN_H
#define CUTELEGRAM_BOOKMARKS_PLUGIN_H

#include <QStringList>
#include <cubotplugininterface.h>

class CuTgBookmarksPluginPrivate;

class CuTgBookmarksPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CuBotPluginInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTgBookmarksPlugin" FILE "cumbia-telegram-bookmarks.json")
#endif // QT_VERSION >= 0x050000

public:
    enum Type { Bot_BookmarksPlugin = 95 };

    enum OperationType { Undefined, Bookmarks, AddBookmark, DelBookmark };

    CuTgBookmarksPlugin(QObject *parent = nullptr);

    virtual ~CuTgBookmarksPlugin();

    void reset();


    // CuBotModule interface
public:
    int type() const;
    QString name() const;
    QString description() const;
    QString help() const;
    int decode(const TBotMsg &msg);
    bool process();
    bool error() const;
    QString message() const;

    // CuBotPluginInterface interface
public:
    void init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf);

private:
    QString bookmarkAdded_msg(const HistoryEntry& b) const;
    QString bookmarkRemoved_msg(bool ok) const;
    QString bookmark_list_msg(const QList<HistoryEntry> &hel, int ttl) const;

    CuTgBookmarksPluginPrivate *d;

};

#endif // CuTgBookmarksPLUGIN_H
