#include "cutelegramtangodbsearchplugin.h"
#include "botsearchtangoatt.h"
#include "botsearchtangodev.h"
#include <cubotvolatileoperations.h>
#include <cudata.h>
#include <botdb.h>

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtDebug>

class CuTelegramTangoDbSearchPluginPrivate {
public:
    CuTelegramTangoDbSearchPlugin::SearchMode m_mode;
    QString m_devnam, m_msg, m_host;
    int m_index;
    TBotMsg m_tbotmsg;
    CuBotVolatileOperations volatile_ops;
    bool m_err;
};

CuTelegramTangoDbSearchPlugin::CuTelegramTangoDbSearchPlugin()
{
    d = new CuTelegramTangoDbSearchPluginPrivate;
    reset();
}

CuTelegramTangoDbSearchPlugin::~CuTelegramTangoDbSearchPlugin()
{
    printf("\e[1;31m~CuTelegramTangoDbSearchPlugin %p\e[0m\n", this);
    delete d;
}

void CuTelegramTangoDbSearchPlugin::reset()
{
    d->m_mode = Invalid;
    d->m_index =  -1;
    d->m_tbotmsg = TBotMsg();
    d->m_err = false;
    d->m_msg = d->m_devnam = QString();
}

int CuTelegramTangoDbSearchPlugin::decode(const TBotMsg &msg)
{
    reset();
    d->m_tbotmsg = msg;

    const QString& text = msg.text();
    QRegularExpression re;
    QRegularExpressionMatch match;
    const char *tg_section_match = "[A-Za-z0-9_\\.\\*]";
    re.setPattern(QString("search\\s+(.+)")/*.arg(tg_section_match))*/);
    match = re.match(text);
    if(match.hasMatch()) {
        d->m_mode = DevSearch;
        d->m_devnam = match.captured(1);
    }

    if(d->m_mode == Invalid) {
        // search for /attlistX commands
        re.setPattern("/attlist(\\d{1,2})\\b");
        match = re.match(text);
        if(match.hasMatch()) {
            d->m_index = match.captured(1).toInt();
            d->m_mode = AttSearch;
        }
    }
    if(d->m_mode == Invalid) {
        // search for /aXX_read, from attribute list (/attlistXX)
        re.setPattern("/a(\\d{1,2})_read\\b");
        match = re.match(text);
        if(match.hasMatch()) {
            d->m_index = match.captured(1).toInt();
            d->m_mode = ReadFromAttList;
        }
    }

    int ret;
    d->m_mode == Invalid ? ret = -1 : ret = type();
    return ret;
}

bool CuTelegramTangoDbSearchPlugin::process()
{
    d->m_err = false;
    d->m_msg = QString();
    QString host = getDb()->getSelectedHost(d->m_tbotmsg.chat_id);
    if(d->m_mode == DevSearch) {
        BotSearchTangoDev * devSearch = new BotSearchTangoDev(this,  d->m_tbotmsg.chat_id);
        connect(devSearch, SIGNAL(devListReady(int, QStringList, QString)),
                this, SLOT(m_onTgDevListSearchReady(int, QStringList, QString)));
        connect(devSearch, SIGNAL(volatileOperationExpired(int,QString,QString)),
                this, SLOT(onVolatileOperationExpired(int,QString,QString)));
        devSearch->find(d->m_devnam, host);
        d->volatile_ops.replaceOperation(d->m_tbotmsg.chat_id, devSearch);
    }
    else if(d->m_mode == AttSearch) {
        QString devname;
        if(d->m_index < 0) { // attlist  tango/dev/name
            devname =d->m_devnam; // will contain tango device name
        }
        else {
            BotSearchTangoDev *sd =
                    static_cast<BotSearchTangoDev *>(d->volatile_ops.get(d->m_tbotmsg.chat_id, BotSearchTangoDev::Bot_SearchTangoDev));
            if(sd) {
                devname = sd->getDevByIdx(d->m_index);
            } else {
                QStringList sequence = QStringList() << "search PATTERN" << QString("/attlist%1").arg(d->m_index);
                getModuleListener()->onSendMessageRequest(d->m_tbotmsg.chat_id, m_errorVolatileSequence_msg(sequence));
            }
        }
        if(!devname.isEmpty()) {
            BotSearchTangoAtt *sta = new BotSearchTangoAtt(this, d->m_tbotmsg.chat_id);
            connect(sta, SIGNAL(attListReady(int, QString, QStringList,QString)),
                    this, SLOT(m_onTgAttListSearchReady(int, QString, QStringList,QString)));
            connect(sta, SIGNAL(volatileOperationExpired(int, QString,QString)),
                    this, SLOT(onVolatileOperationExpired(int, QString,QString)));
            sta->find(devname, host);
            d->volatile_ops.replaceOperation(d->m_tbotmsg.chat_id, sta);
        }
        else {
            QStringList sequence = QStringList() << "search PATTERN" << QString("/attlist%1" ).arg(d->m_index);
            getModuleListener()->onSendMessageRequest(d->m_tbotmsg.chat_id, m_errorVolatileSequence_msg(sequence));
        }
    }
    else if(d->m_mode == ReadFromAttList) {
        QString src;
        BotSearchTangoAtt* sta = static_cast<BotSearchTangoAtt *>(d->volatile_ops.get(d->m_tbotmsg.chat_id, BotSearchTangoAtt::Bot_SearchTangoAtt));
        if(!sta) {
            QStringList sequence = QStringList() << "search PATTERN" << "/attlist{IDX}"
                                                 << QString("/a%1_read" ).arg(d->m_index);
            getModuleListener()->onSendMessageRequest(d->m_tbotmsg.chat_id, m_errorVolatileSequence_msg(sequence));
        }
        else if(d->m_index > 0 && sta && (src = sta->getSourceByIdx(d->m_index) ) != QString()) {
            d->m_tbotmsg.setText(src);
            getModuleListener()->onReinjectMessage(d->m_tbotmsg);
        }
    }

    return !d->m_err;
}

void CuTelegramTangoDbSearchPlugin::m_onTgDevListSearchReady(int chat_id, const QStringList &devs, const QString &message)
{
    qDebug() << __PRETTY_FUNCTION__ << chat_id << devs;
    getModuleListener()->onSendMessageRequest(chat_id, tg_devSearchList_msg(devs, message));
    getModuleListener()->onStatsUpdateRequest(chat_id, CuData("err", false));// CuData is passed for error stats
}

void CuTelegramTangoDbSearchPlugin::m_onTgAttListSearchReady(int chat_id, const QString& devname,
                                                             const QStringList &atts, const QString& msg)
{
    getModuleListener()->onSendMessageRequest(chat_id, tg_attSearchList_msg(devname, atts, msg));
    getModuleListener()->onStatsUpdateRequest(chat_id, CuData("err", false)); // CuData is passed for error stats
}

void CuTelegramTangoDbSearchPlugin::onVolatileOperationExpired(int i, const QString &s, const QString &t)
{
    printf("\e[1;35mCuTelegramTangoDbSearchPlugin.onVolatileOperationExpired: %d %s %s\e[0m\n",
           i, qstoc(s), qstoc(t));
}

bool CuTelegramTangoDbSearchPlugin::error() const
{
    return d->m_err;
}

QString CuTelegramTangoDbSearchPlugin::message() const
{
    return d->m_msg;
}

bool CuTelegramTangoDbSearchPlugin::isVolatileOperation() const
{
    return false;
}

QString CuTelegramTangoDbSearchPlugin::description() const
{
    return "cumbia-telegram plugin to search devices on the tango database";
}

int CuTelegramTangoDbSearchPlugin::type() const
{
    return TangoDbSearch;
}

QString CuTelegramTangoDbSearchPlugin::name() const
{
    return "search";
}

QString CuTelegramTangoDbSearchPlugin::help() const
{
    return "search";
}

void CuTelegramTangoDbSearchPlugin::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}

QString CuTelegramTangoDbSearchPlugin::m_errorVolatileSequence_msg(const QStringList &seq) const
{
    QString s = "😞   The commands <i> ";
    for(int i = 0; i < seq.size() -1; i++) {
        s += seq[i] + ", ";
    }
    if(seq.size() > 0)
        s += seq.last();

    s +=  " </i>\nmust be executed in sequence";
    return s;
}

QString CuTelegramTangoDbSearchPlugin::tg_devSearchList_msg(const QStringList &devs, const QString& message) const
{
    QString s;
    if(devs.isEmpty())
        s = "😞   No device found matching the given pattern: " + message;
    else {
        s = QString("<b>%1 DEVICES</b>\n\n").arg(devs.size());
        for(int i = 0; i < devs.size(); i++) {
            s += QString("%1: " + devs[i] + "   [/attlist%1]\n").arg(i+1);
        }
    }
    return s;
}

QString CuTelegramTangoDbSearchPlugin::tg_attSearchList_msg(const QString &devname, const QStringList &atts, const QString &message) const
{
    QString s;
    if(atts.isEmpty())
        s = "😞   No attributes found in device <i>" + devname + "</i>: " + message + "\n";
    else {
        s = QString("<b>%1 ATTRIBUTES</b> from <i>%2</i>\n\n").arg(atts.size()).arg(devname);
        for(int i = 0; i < atts.size(); i++) {
            s += QString("%1: <i>" + atts[i] + "</i>   [/a%1_read]\n").arg(i+1);
        }
    }
    return s;
}

