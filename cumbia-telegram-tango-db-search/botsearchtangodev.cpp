#include "botsearchtangodev.h"
#include <tango.h>
#include <QtDebug>

BotSearchTangoDev::BotSearchTangoDev(QObject *parent) : QObject(parent)
{
}

BotSearchTangoDev::~BotSearchTangoDev()
{
    TgDevSearchThread *th = findChild<TgDevSearchThread *>();
    if(th)
        th->wait();
    printf("\e[1;31mx BotSearchTangoDev %p\e[0m\n", this);
}

void BotSearchTangoDev::find(const QString &pattern)
{
    m_pattern = pattern;
    TgDevSearchThread *th = new TgDevSearchThread(this, pattern);
    connect(th, SIGNAL(finished()), this, SLOT(onSearchFinished()));
    th->start();
}

QString BotSearchTangoDev::getDevByIdx(int idx)
{
    if(idx > 0 && idx - 1 < m_devlist.size())
        return m_devlist.at(idx - 1);
    d_error = true;
    d_msg = QString("BotSearchTangoDev.getDevByIdx: index %1 out of range").arg(idx);
    return QString();
}

QStringList BotSearchTangoDev::devices() const
{
    return m_devlist;
}

void BotSearchTangoDev::signalTtlExpired()
{
    printf("\e[1;31mBotSearchTangoDev::signalTtlExpired chat_id NOT INITIALIZED!!!\e[0m\n");
    emit volatileOperationExpired(m_chat_id, name(), "search " + m_pattern);
}

void BotSearchTangoDev::onSearchFinished()
{
    m_devlist = qobject_cast<TgDevSearchThread *>(sender())->devices;
    sender()->deleteLater();
    printf("\e[1;31mBotSearchTangoDev::onSearchFinished chat_id NOT INITIALIZED!!!\e[0m\n");
    emit devListReady(m_chat_id, m_devlist);
}

TgDevSearchThread::TgDevSearchThread(QObject *parent, const QString &pattern) : QThread(parent)
{
    m_pattern = pattern;
}

void TgDevSearchThread::run()
{
    Tango::Database db;
    devices.clear();
    m_pattern.replace("//", "/*/");
    std::string pattern = m_pattern.toStdString();
    Tango::DbDatum dbd  = db.get_device_exported(pattern);
    std::vector<std::string> devs;
    dbd >> devs;
    for(size_t i = 0; i < devs.size(); i++)
        devices << QString::fromStdString(devs[i]);
}

/**
 * @brief BotSearchTangoDev::consume consume the life of this object if the input type is not AttSearch
 *
 * @param t the type of message that causes this VolatileOperation to consume itself or not
 */
void BotSearchTangoDev::consume(int moduletyp)
{
    QVariant att_search_type = CuBotModule::getOption("att_search_type");
    if(!att_search_type.isValid() || moduletyp != att_search_type.toInt())
        d_life_cnt--;
}

int BotSearchTangoDev::type() const
{
    return DevSearch;
}

QString BotSearchTangoDev::name() const
{
    return "Tango device search";
}

int BotSearchTangoDev::decode(const TBotMsg &msg)
{
}

bool BotSearchTangoDev::process()
{
}

bool BotSearchTangoDev::error() const
{
}

QString BotSearchTangoDev::message() const
{
}

bool BotSearchTangoDev::isVolatileOperation() const
{
    return true;
}

QString BotSearchTangoDev::description() const
{
    return "cumbia-telegram plugin to search devices on the tango database";
}

QString BotSearchTangoDev::help() const
{
    return "cumbia-telegram bot plugin to find tango devices from the database";
}

void BotSearchTangoDev::init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf)
{
    setBotmoduleListener(listener);
    setDb(db);
    setConf(bot_conf);
}
