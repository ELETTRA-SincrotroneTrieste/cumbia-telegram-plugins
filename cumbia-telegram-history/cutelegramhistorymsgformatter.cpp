#include "cutelegramhistorymsgformatter.h"
#include <formulahelper.h>
#include <generic_msgformatter.h>

CuTelegramHistoryMsgFormatter::CuTelegramHistoryMsgFormatter()
{

}


QString CuTelegramHistoryMsgFormatter::history_msg(const QList<HistoryEntry> &hel, int ttl, const QString& type) const
{
    QString msg;
    if(hel.size() == 0)
        msg = type + " list is empty";
    else {
        // heading
        msg += "<b>" + type.toUpper();
        if(type != "bookmarks")
            msg += " HISTORY";
        msg += "</b>\n\n";

        for(int i = 0; i < hel.size(); i++) {
            msg += hel[i].formatEntry_msg(i, ttl, false); // false: is_bookmark
        }
    }
    return msg;
}
