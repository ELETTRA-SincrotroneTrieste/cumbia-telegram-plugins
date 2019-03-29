#include "botsearchtangodev.h"
#include "cutelegramtangodbsearchplugin.h" // for enum SearchMode { Invalid = 0, DevSearch, AttSearch, ReadFromAttList };
#include <tango.h>
#include <cumacros.h>
#include <cutango-world.h>
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

void BotSearchTangoDev::find(const QString &pattern, const QString& host)
{
    m_pattern = pattern;
    TgDevSearchThread *th = new TgDevSearchThread(this, pattern, host);
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
    d_msg = qobject_cast<TgDevSearchThread *>(sender())->message();
    sender()->deleteLater();
    emit devListReady(m_chat_id, m_devlist, d_msg);
}

TgDevSearchThread::TgDevSearchThread(QObject *parent, const QString &pattern, const QString &host) : QThread(parent)
{
    m_pattern = pattern;
    m_host = host;
}

void TgDevSearchThread::run()
{
    m_errmsg.clear();
    Tango::Database *db;
    try {
        if(!m_host.isEmpty() && m_host.split(":").size() == 2) {
            std::string host(m_host.section(':', 0, 0).toStdString());
            db = new Tango::Database(host, m_host.section(":", -1).toInt());
        }
        else {
            db = new Tango::Database();
        }
        devices.clear();
        m_pattern.replace("//", "/*/");
        std::string pattern = m_pattern.toStdString();
        Tango::DbDatum dbd  = db->get_device_exported(pattern);
        std::vector<std::string> devs;
        dbd >> devs;
        for(size_t i = 0; i < devs.size(); i++)
            devices << QString::fromStdString(devs[i]);
        delete db;
    }
    catch(Tango::DevFailed &e) {
        CuTangoWorld tw;
        m_errmsg = QString::fromStdString(tw.strerror(e));
    }
}

bool TgDevSearchThread::error() const
{
    return m_errmsg.size() > 0;
}

QString TgDevSearchThread::message() const
{
    return m_errmsg;
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
