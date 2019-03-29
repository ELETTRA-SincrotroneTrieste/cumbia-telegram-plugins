#include "cutelegramhistoryplugin.h"
#include "cutelegramhistorymsgformatter.h"
#include <botconfig.h>
#include <botdb.h>

QString CuTgHistoryMsgDecoder::toHistoryTableType(CuTgHistoryMsgDecoder::Type t) const
{
    QString type;
    if(t == CuTgHistoryMsgDecoder::MonitorHistory)
        type = "monitor";
    else if(t == CuTgHistoryMsgDecoder::AlertHistory)
        type = "alert";
    else if(t == CuTgHistoryMsgDecoder::ReadHistory)
        type = "read";
    return type;
}

CuTgHistoryMsgDecoder::Type CuTgHistoryMsgDecoder::decode(const TBotMsg &msg)
{
    m_type = Undefined;
    QString m_text = msg.text();
    if(m_text == "/monitors" || m_text == "monitors") {
        m_type = MonitorHistory;
    }
    else if(m_text == "/alerts" || m_text == "alerts") {
        m_type = AlertHistory;
    }
    else if(m_text == "/reads" || m_text == "reads")
        m_type = ReadHistory;
    return  m_type;
}

class CuTelegramHistoryPluginPrivate {
public:
    CuTgHistoryMsgDecoder::Type msg_type;
    int chat_id, user_id;
    bool err;
    QString msg;
};

CuTelegramHistoryPlugin::CuTelegramHistoryPlugin(QObject *parent) : QObject (parent)
{
    d = new CuTelegramHistoryPluginPrivate;
    reset();
}

void CuTelegramHistoryPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

QList<HistoryEntry> CuTelegramHistoryPlugin::m_prepareHistory(int uid, CuTgHistoryMsgDecoder::Type t)
{
    QString type = CuTgHistoryMsgDecoder().toHistoryTableType(t);
    QList<HistoryEntry> out = getDb()->history(uid, type);
    return out;
}

CuTelegramHistoryPlugin::~CuTelegramHistoryPlugin()
{
    delete d;
}

void CuTelegramHistoryPlugin::reset()
{
    d->chat_id = d->user_id = -1;
    d->err = false;
    d->msg.clear();
    d->msg_type = CuTgHistoryMsgDecoder::Undefined;
}

int CuTelegramHistoryPlugin::type() const {
    return Bot_HistoryPlugin;
}

QString CuTelegramHistoryPlugin::name() const
{
    return "history";
}

QString CuTelegramHistoryPlugin::description() const {
    return "Search for readers and active/inactive monitors and alerts";
}

QString CuTelegramHistoryPlugin::help() const
{
    return QString();
}

int CuTelegramHistoryPlugin::decode(const TBotMsg &msg)
{
    reset();
    d->user_id = msg.user_id;
    d->chat_id = msg.chat_id;
    CuTgHistoryMsgDecoder dec;
    d->msg_type = dec.decode(msg);
    if(d->msg_type != CuTgHistoryMsgDecoder::Undefined)
        return type();
    return -1;
}

bool CuTelegramHistoryPlugin::process()
{

    if(d->msg_type != CuTgHistoryMsgDecoder::Undefined) {
        QList<HistoryEntry> hel = m_prepareHistory(d->user_id, d->msg_type);
        getModuleListener()->onSendMessageRequest(d->chat_id, CuTelegramHistoryMsgFormatter().history_msg(hel, getBotConfig()->ttl(), CuTgHistoryMsgDecoder().toHistoryTableType(d->msg_type)));
    }
    return true;
}

bool CuTelegramHistoryPlugin::error() const {
    return d->err;
}

QString CuTelegramHistoryPlugin::message() const {
    return d->msg;
}



