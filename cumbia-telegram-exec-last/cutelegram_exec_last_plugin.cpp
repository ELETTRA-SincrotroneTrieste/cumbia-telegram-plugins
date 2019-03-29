#include "cutelegram_exec_last_plugin.h"
#include <botconfig.h>
#include <botdb.h>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <generic_msgformatter.h>
#include <formulahelper.h>

class CuTgExecLastPluginPrivate {
public:
    bool err;
    QString msg, type_as_str;
    TBotMsg tbotmsg;
};

CuTgExecLastPlugin::CuTgExecLastPlugin(QObject *parent) : QObject (parent)
{
    d = new CuTgExecLastPluginPrivate;
    reset();
}

void CuTgExecLastPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

QString CuTgExecLastPlugin::m_history_op_msg(const QDateTime &dt, const QString &name) const
{
    if(name.isEmpty())
        return "You haven't performed any operation yet!";
    QString msg = "<i>" + GenMsgFormatter().timeRepr(dt) + "</i>\n";
    msg += "operation: <b>" + FormulaHelper().escape(name) + "</b>";
    return msg;
}

CuTgExecLastPlugin::~CuTgExecLastPlugin()
{
    delete d;
}

void CuTgExecLastPlugin::reset()
{
    d->err = false;
    d->tbotmsg = TBotMsg();
    d->msg.clear();
}

int CuTgExecLastPlugin::type() const {
    return Bot_ExecLastPlugin;
}

QString CuTgExecLastPlugin::name() const
{
    return "last";
}

QString CuTgExecLastPlugin::description() const {
    return "execute last successful command";
}

QString CuTgExecLastPlugin::help() const
{
    return QString();
}

int CuTgExecLastPlugin::decode(const TBotMsg &msg)
{
    reset();
    d->tbotmsg = msg;
    if(msg.text() == "/last" || msg.text() == "last")
        return type();
    return -1;
}

bool CuTgExecLastPlugin::process()
{
    CuBotModuleListener *lis = getModuleListener();
    HistoryEntry he = getDb()->lastOperation(d->tbotmsg.user_id);
    if(he.isValid()) {
        // 1. remind the user what was the command linked to /commandN
        QString operation = he.toCommand();
        QDateTime dt = he.datetime;
        lis->onSendMessageRequest(d->tbotmsg.chat_id, this->m_history_op_msg(dt, operation));
        // 2. reinject command from history
        d->tbotmsg.setText(operation);
        d->tbotmsg.setHost(he.host);
        lis->onReinjectMessage(d->tbotmsg);
        return true;
    }
    return false;
}

bool CuTgExecLastPlugin::error() const {
    return d->err;
}

QString CuTgExecLastPlugin::message() const {
    return d->msg;
}



