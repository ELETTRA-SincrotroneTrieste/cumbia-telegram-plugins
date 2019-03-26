#include "botsearchtangodev.h"
#include "cutelegramtangodbsearchplugin.h" // for enum SearchMode { Invalid = 0, DevSearch, AttSearch, ReadFromAttList };
#include <tango.h>
#include <cumacros.h>
#include <QtDebug>

BotSearchTangoDev::BotSearchTangoDev(QObject *parent, int chat_id) : QObject(parent)
{
    m_chat_id = chat_id;
}

BotSearchTangoDev::~BotSearchTangoDev()
{
    TgDevSearchThread *th = findChild<TgDevSearchThread *>();
    if(th)
        th->wait();
    printf("\e[1;31mx BotSearchTangoDev %p (as CuBotVolatileOperation: %p) %s\e[0m\n",
           this, static_cast<CuBotVolatileOperation *>(this), qstoc(m_pattern));
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

void BotSearchTangoDev::onSearchFinished()
{
    m_devlist = qobject_cast<TgDevSearchThread *>(sender())->devices;
    sender()->deleteLater();
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


void BotSearchTangoDev::consume(int typ)
{
    if(typ != CuTelegramTangoDbSearchPlugin::AttSearch)
        d_life_cnt--;
}

bool BotSearchTangoDev::disposeWhenOver() const
{
    return true;
}

int BotSearchTangoDev::type() const
{
    return Bot_SearchTangoDev;
}

QString BotSearchTangoDev::name() const
{
    return "BotSearchTangoDev";
}

void BotSearchTangoDev::signalTtlExpired()
{
    emit volatileOperationExpired(m_chat_id, name(),  "search " + m_pattern);
}
