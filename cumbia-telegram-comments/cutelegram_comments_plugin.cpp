#include "cutelegram_comments_plugin.h"
#include <botconfig.h>
#include <botdb.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <generic_msgformatter.h>
#include <formulahelper.h>

class CuTgCommentsPluginPrivate {
public:
    bool err;
    QString msg, type_as_str;
    TBotMsg tbotmsg;
};

CuTgCommentsPlugin::CuTgCommentsPlugin(QObject *parent) : QObject (parent)
{
    d = new CuTgCommentsPluginPrivate;
    reset();
}

void CuTgCommentsPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

CuTgCommentsPlugin::~CuTgCommentsPlugin()
{
    delete d;
}

void CuTgCommentsPlugin::reset()
{
    d->err = false;
    d->tbotmsg = TBotMsg();
    d->msg.clear();
}

int CuTgCommentsPlugin::type() const {
    return Bot_CommentsPlugin;
}

QString CuTgCommentsPlugin::name() const
{
    return "comments";
}

QString CuTgCommentsPlugin::description() const {
    return "messages starting with \",,\" or \"..\" are comments and can be used to chat in groups";
}

QString CuTgCommentsPlugin::help() const
{
    return "start a message with double comma or double dot to send a comment"
           " to the bot chat or group";
}

int CuTgCommentsPlugin::decode(const TBotMsg &msg)
{
    reset();
    d->tbotmsg = msg;
    if(msg.text().startsWith("..") || msg.text().startsWith(",,") || msg.text().startsWith("  "))
        return type();
    return -1;
}

bool CuTgCommentsPlugin::process()
{
//    CuBotModuleListener *lis = getModuleListener();
//    QString msg = d->tbotmsg.text();
//    QString s = QString("<b>%1</b> says:\n<i>%2</i>").arg(d->tbotmsg.username)
//            .arg(msg.remove(0,2).trimmed());
//    lis->onSendMessageRequest(d->tbotmsg.chat_id, s);
    return true;
}

bool CuTgCommentsPlugin::error() const {
    return d->err;
}

QString CuTgCommentsPlugin::message() const {
    return d->msg;
}



