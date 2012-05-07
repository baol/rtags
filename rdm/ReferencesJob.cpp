#include "ReferencesJob.h"
#include "Server.h"
#include "Rdm.h"
#include "CursorInfo.h"

ReferencesJob::ReferencesJob(int i, const Location &loc, unsigned flags)
    : Job(i), symbolName(QByteArray()), keyFlags(flags)
{
    locations.insert(loc);
}

ReferencesJob::ReferencesJob(int i, const QByteArray &sym, unsigned flags)
    : Job(i), symbolName(sym), keyFlags(flags)
{
}

void ReferencesJob::execute()
{
    if (!symbolName.isEmpty()) {
        Database *db = Server::instance()->db(Server::SymbolName);
        locations = db->value<QSet<Location> >(symbolName);
        if (locations.isEmpty()) {
            return;
        }
    }
    Database *db = Server::instance()->db(Server::Symbol);
    foreach(const Location &location, locations) {
        if (isAborted())
            return;
        CursorInfo cursorInfo = Rdm::findCursorInfo(db, location);
        QSet<Location> refs = cursorInfo.references;
        if (refs.isEmpty() && !cursorInfo.target.isNull()) {
            cursorInfo = Rdm::findCursorInfo(db, cursorInfo.target);
            refs = cursorInfo.references;
        }
        foreach (const Location &loc, refs) {
            write(loc.key(keyFlags));
        }
    }
}
