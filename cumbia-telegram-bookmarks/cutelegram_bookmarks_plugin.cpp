#include "cutelegram_bookmarks_plugin.h"
#include <botconfig.h>
#include <botdb.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <generic_msgformatter.h>
#include <formulahelper.h>

class CuTgBookmarksPluginPrivate {
public:
    bool err;
    QString msg, type_as_str;
    TBotMsg tbotmsg;
    CuTgBookmarksPlugin::OperationType optyp;
    int bookmark_idx;
};

CuTgBookmarksPlugin::CuTgBookmarksPlugin(QObject *parent) : QObject (parent)
{
    d = new CuTgBookmarksPluginPrivate;
    reset();
}

void CuTgBookmarksPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

QString CuTgBookmarksPlugin::bookmarkAdded_msg(const HistoryEntry &b) const
{
    QString s;
    FormulaHelper fh;
    if(b.isValid()) {
        s += "👍   successfully added bookmark:\n";
        s += QString("<i>%1</i>\ntype: <i>%2</i>  [host: <i>%3</i>]").arg(fh.escape(b.command))
                .arg(b.type).arg(fh.escape(b.host));
    }
    else {
        s = "👎   could not add the requested bookmark";
    }
    return s;
}

QString CuTgBookmarksPlugin::bookmarkRemoved_msg(bool ok) const
{
    QString s;
    ok ? s =  "👍   successfully removed bookmark" : s = "👎   failed to remove bookmark";
    return s;
}

CuTgBookmarksPlugin::~CuTgBookmarksPlugin()
{
    delete d;
}

void CuTgBookmarksPlugin::reset()
{
    d->err = false;
    d->optyp = Undefined;
    d->tbotmsg = TBotMsg();
    d->bookmark_idx = -1;
    d->msg.clear();
}

int CuTgBookmarksPlugin::type() const {
    return Bot_BookmarksPlugin;
}

QString CuTgBookmarksPlugin::name() const
{
    return "bookmarks";
}

QString CuTgBookmarksPlugin::description() const {
    return "manage bookmarks";
}

QString CuTgBookmarksPlugin::help() const
{
    return QString();
}

int CuTgBookmarksPlugin::decode(const TBotMsg &msg)
{
    reset();
    QString txt = msg.text();
    d->tbotmsg = msg;
    if(txt == "bookmarks" || txt == "/bookmarks") d->optyp = Bookmarks;
    else if(txt == "bookmark" || txt== "/bookmark") d->optyp = AddBookmark;
    else {
        QRegularExpression re;
        QRegularExpressionMatch match;
        re.setPattern("/XB(\\d{1,2})\\b");
        match = re.match(txt);
        if(match.hasMatch()) {
            d->bookmark_idx = match.captured(1).toInt();
            d->optyp = DelBookmark;
        }
    }
    if(d->optyp == Undefined)
        return -1;
    return type();
}

bool CuTgBookmarksPlugin::process()
{
    CuBotModuleListener *lis = getModuleListener();
    if(d->optyp == AddBookmark) {
        HistoryEntry he = getDb()->bookmarkLast(d->tbotmsg.user_id);
        lis->onSendMessageRequest(d->tbotmsg.chat_id, bookmarkAdded_msg(he));
    }
    else if(d->optyp == DelBookmark && d->bookmark_idx > 0) {
        bool success;
        if((success = getDb()->removeBookmark(d->tbotmsg.user_id, d->bookmark_idx))) {
            lis->onSendMessageRequest(d->tbotmsg.chat_id, bookmarkRemoved_msg(success));
        }
    }
    else if(d->optyp == Bookmarks) {
       QList<HistoryEntry> hel = getDb()->history(d->tbotmsg.user_id, "bookmarks");
        lis->onSendMessageRequest(d->tbotmsg.chat_id, bookmark_list_msg(hel, getBotConfig()->ttl()));
    }
    return true;
}

bool CuTgBookmarksPlugin::error() const {
    return d->err;
}

QString CuTgBookmarksPlugin::message() const {
    return d->msg;
}

QString CuTgBookmarksPlugin::bookmark_list_msg(const QList<HistoryEntry> &hel, int ttl) const
{
    QString msg;
    if(hel.size() == 0)
        msg = "bookmarks list is empty";
    else {
        // heading
        msg += "<b>BOOKMARKS</b>\n\n";
        for(int i = 0; i < hel.size(); i++) {
            msg += hel[i].formatEntry_msg(i, ttl, true); // true: is bookmark
        }
    }
    return msg;
}


