#include "cutelegram_exec_history_links_plugin.h"
#include <botconfig.h>
#include <botdb.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <generic_msgformatter.h>
#include <formulahelper.h>

int CuTgExHistLnkMsgDecoder::decode(const TBotMsg &msg)
{
    int index = -1;
    QString text = msg.text();
    QRegularExpression re;
    QRegularExpressionMatch match;
    re.setPattern("/(?:read|monitor|alert)(\\d{1,2})\\b");
    match = re.match(text);
    if(match.hasMatch()) {
        index = match.captured(1).toInt();
        type = text.remove("/").remove(match.captured(1));
    }
    return  index;
}


class CuTgExHistLnkPluginPrivate {
public:
    int index;
    bool err;
    QString msg, type_as_str;
    TBotMsg tbotmsg;
};

CuTgExHistLnkPlugin::CuTgExHistLnkPlugin(QObject *parent) : QObject (parent)
{
    d = new CuTgExHistLnkPluginPrivate;
    reset();
}

void CuTgExHistLnkPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

QString CuTgExHistLnkPlugin::m_history_op_msg(const QDateTime &dt, const QString &name) const
{
    if(name.isEmpty())
        return "You haven't performed any operation yet!";
    QString msg = "<i>" + GenMsgFormatter().timeRepr(dt) + "</i>\n";
    msg += "operation: <b>" + FormulaHelper().escape(name) + "</b>";
    return msg;
}

CuTgExHistLnkPlugin::~CuTgExHistLnkPlugin()
{
    delete d;
}

void CuTgExHistLnkPlugin::reset()
{
    d->index = -1;
    d->err = false;
    d->tbotmsg = TBotMsg();
    d->msg.clear();
}

int CuTgExHistLnkPlugin::type() const {
    return Bot_ExecHistoryLinkPlugin;
}

QString CuTgExHistLnkPlugin::name() const
{
    return "cmdshortcuts";
}

QString CuTgExHistLnkPlugin::description() const {
    return "execute commands from history, like /monitorN /readX /alertY";
}

QString CuTgExHistLnkPlugin::help() const
{
    return QString();
}

int CuTgExHistLnkPlugin::decode(const TBotMsg &msg)
{
    reset();
    CuTgExHistLnkMsgDecoder dec;
    d->index = dec.decode(msg);
    d->type_as_str = dec.type;
    d->tbotmsg = msg;
    if(d->index > -1)
        return type();
    return -1;
}

bool CuTgExHistLnkPlugin::process()
{
    CuBotModuleListener *lis = getModuleListener();
    HistoryEntry he = getDb()->commandFromIndex(d->tbotmsg.user_id, d->type_as_str, d->index);
    if(he.isValid() && d->index > -1) {
        // 1. remind the user what was the command linked to /commandN
        QString operation = he.toCommand();
        QDateTime dt = he.datetime;
        lis->onSendMessageRequest(d->tbotmsg.chat_id, this->m_history_op_msg(dt, operation));
        // 2. reinject command from history
        d->tbotmsg.setText(operation);
        d->tbotmsg.setHost(he.host);
        lis->onReinjectMessage(d->tbotmsg);
    }
    return true;
}

bool CuTgExHistLnkPlugin::error() const {
    return d->err;
}

QString CuTgExHistLnkPlugin::message() const {
    return d->msg;
}



