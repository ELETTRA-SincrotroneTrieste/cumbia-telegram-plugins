#include "cutelegramtangodbsearchplugin.h"
#include "botsearchtangoatt.h"
#include "botsearchtangodev.h"

#include <cudata.h>

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtDebug>

CuTelegramTangoDbSearchPlugin::CuTelegramTangoDbSearchPlugin()
{
    reset();
}

CuTelegramTangoDbSearchPlugin::~CuTelegramTangoDbSearchPlugin()
{
    printf("\e[1;31m~CuTelegramTangoDbSearchPlugin %p\e[0m\n", this);
}

void CuTelegramTangoDbSearchPlugin::reset()
{
    d_life_cnt = 1;
    m_mode = Invalid;
    m_index =  -1;
    m_tbotmsg = TBotMsg();
    m_err = false;
    m_msg = QString();
}


/**
 * @brief CuTelegramTangoDbSearchPlugin::consume consume the life of this object if the input type is not AttSearch
 *
 * @param t the type of message that causes this VolatileOperation to consume itself or not
 */
void CuTelegramTangoDbSearchPlugin::consume(int moduletyp)
{
    if(moduletyp != type())
        d_life_cnt--;
    if(d_life_cnt < 0) {
        BotSearchTangoDev *devSearch = findChild<BotSearchTangoDev *>();
        BotSearchTangoAtt *sta = findChild<BotSearchTangoAtt *>();
        if(devSearch) delete devSearch;
        if(sta) delete sta;
    }
}

int CuTelegramTangoDbSearchPlugin::type() const
{
    return TangoDbSearch;
}

QString CuTelegramTangoDbSearchPlugin::name() const
{
    return "TangoDbSearch";
}

void CuTelegramTangoDbSearchPlugin::signalTtlExpired()
{
    emit volatileOperationExpired(m_tbotmsg.chat_id, name(), "attlist " + m_devnam);
}

/**
 * @brief CuTelegramTangoDbSearchPlugin::deleteWhenOver returns true to prevent CuBotVolatileOperations
 *        from deleting this object when consumed
 * @return  true
 *
 * Prevents CuBotVolatileOperations from deleting this when life count is < 0
 */
bool CuTelegramTangoDbSearchPlugin::disposeWhenOver() const
{
    return false;
}

int CuTelegramTangoDbSearchPlugin::decode(const TBotMsg &msg)
{
    reset();
    m_tbotmsg = msg;

    const QString& text = msg.text();
    QRegularExpression re;
    QRegularExpressionMatch match;
    const char *tg_section_match = "[A-Za-z0-9_\\.\\*]";
    re.setPattern(QString("search\\s+(%1*/%1*/%1)").arg(tg_section_match));
    match = re.match(text);
    if(match.hasMatch()) {
        m_mode = DevSearch;
        m_devnam = match.captured(1);
    }

    if(m_mode == Invalid) {
        // search for /attlistX commands
        re.setPattern("/attlist(\\d{1,2})\\b");
        match = re.match(text);
        if(match.hasMatch()) {
            m_index = match.captured(1).toInt();
            m_mode = AttSearch;
        }
    }
    if(m_mode == Invalid) {
        // search for /aXX_read, from attribute list (/attlistXX)
        re.setPattern("/a(\\d{1,2})_read\\b");
        match = re.match(text);
        if(match.hasMatch()) {
            m_index = match.captured(1).toInt();
            m_mode = ReadFromAttList;
        }
    }

    int ret;
    m_mode == Invalid ? ret = -1 : ret = type();
    return ret;
}

bool CuTelegramTangoDbSearchPlugin::process()
{
    m_err = false;
    m_msg = QString();
    if(m_mode == DevSearch) {
        BotSearchTangoDev *devSearch = findChild<BotSearchTangoDev *>();
        if(!devSearch) {
            devSearch = new BotSearchTangoDev(this,  m_tbotmsg.chat_id);
            connect(devSearch, SIGNAL(devListReady(int, QStringList)), this, SLOT(m_onTgDevListSearchReady(int, QStringList)));
            connect(devSearch, SIGNAL(volatileOperationExpired(int,QString,QString)),
                    this, SLOT(onVolatileOperationExpired(int,QString,QString)));
        }
        devSearch->find(m_devnam);
    }
    else if(m_mode == AttSearch) {
        QString devname;
        if(m_index < 0) { // attlist  tango/dev/name
            devname = m_devnam; // will contain tango device name
        }
        else {
            BotSearchTangoDev *sd = findChild<BotSearchTangoDev *>();
            if(sd) {
                devname = sd->getDevByIdx(m_index);
            }
        }
        if(!devname.isEmpty()) {
            BotSearchTangoAtt *sta = findChild<BotSearchTangoAtt *>();
            if(!sta) {
                sta = new BotSearchTangoAtt(this, m_tbotmsg.chat_id);
                connect(sta, SIGNAL(attListReady(int, QString, QStringList)),
                        this, SLOT(m_onTgAttListSearchReady(int, QString, QStringList)));
                connect(sta, SIGNAL(volatileOperationExpired(int, QString,QString)),
                        this, SLOT(onVolatileOperationExpired(int, QString,QString)));
            }
            sta->find(devname);
            getModuleListener()->onAddVolatileOperationRequest(m_tbotmsg.chat_id, this);
        }
        else {
            QStringList sequence = QStringList() << "search PATTERN" << QString("/attlist%1" ).arg(m_index);
            getModuleListener()->onSendMessageRequest(m_tbotmsg.chat_id, m_errorVolatileSequence_msg(sequence));
        }
    }
    else if(m_mode == ReadFromAttList) {
        QString src;
        BotSearchTangoAtt* sta = findChild<BotSearchTangoAtt *>();
        if(!sta) {
            QStringList sequence = QStringList() << "search PATTERN" << "/attlist{IDX}"
                                                 << QString("/a%1_read" ).arg(m_index);
            getModuleListener()->onSendMessageRequest(m_tbotmsg.chat_id, m_errorVolatileSequence_msg(sequence));
        }
        else if(m_index > 0 && sta && (src = sta->getSourceByIdx(m_index) ) != QString()) {
            m_tbotmsg.setText(src);
            getModuleListener()->onReinjectMessage(m_tbotmsg);
        }
    }
    if(!m_err && (m_mode == AttSearch || m_mode == DevSearch))
        getModuleListener()->onAddVolatileOperationRequest(m_tbotmsg.chat_id, this);

    return !m_err;
}


void CuTelegramTangoDbSearchPlugin::m_onTgDevListSearchReady(int chat_id, const QStringList &devs)
{
    qDebug() << __PRETTY_FUNCTION__ << chat_id << devs;
    getModuleListener()->onSendMessageRequest(chat_id, tg_devSearchList_msg(devs));
    getModuleListener()->onStatsUpdateRequest(chat_id, CuData("err", false));// CuData is passed for error stats
}

void CuTelegramTangoDbSearchPlugin::m_onTgAttListSearchReady(int chat_id, const QString& devname, const QStringList &atts)
{
    getModuleListener()->onSendMessageRequest(chat_id, tg_attSearchList_msg(devname, atts));
    getModuleListener()->onStatsUpdateRequest(chat_id, CuData("err", false)); // CuData is passed for error stats
}

bool CuTelegramTangoDbSearchPlugin::error() const
{
    return m_err;
}

QString CuTelegramTangoDbSearchPlugin::message() const
{
    return m_msg;
}

bool CuTelegramTangoDbSearchPlugin::isVolatileOperation() const
{
    return true;
}

QString CuTelegramTangoDbSearchPlugin::description() const
{
    return "cumbia-telegram plugin to search devices on the tango database";
}

QString CuTelegramTangoDbSearchPlugin::help() const
{
    return "cumbia-telegram bot plugin to find tango devices from the database";
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

QString CuTelegramTangoDbSearchPlugin::tg_devSearchList_msg(const QStringList &devs) const
{
    QString s;
    if(devs.isEmpty())
        s = "😞   No device found matching the given pattern";
    else {
        s = QString("<b>%1 DEVICES</b>\n\n").arg(devs.size());
        for(int i = 0; i < devs.size(); i++) {
            s += QString("%1: <i>" + devs[i] + "</i>   [/attlist%1]\n").arg(i+1);
        }
    }
    return s;
}

QString CuTelegramTangoDbSearchPlugin::tg_attSearchList_msg(const QString &devname, const QStringList &atts) const
{
    QString s;
    if(atts.isEmpty())
        s = "😞   No attributes found within the device <i>" + devname + "</i>\n";
    else {
        s = QString("<b>%1 ATTRIBUTES</b> from <i>%2</i>\n\n").arg(atts.size()).arg(devname);
        for(int i = 0; i < atts.size(); i++) {
            s += QString("%1: <i>" + atts[i] + "</i>   [/a%1_read]\n").arg(i+1);
        }
    }
    return s;
}
