#include "game.h"
#include <iostream>

bool saveGame(sqlite3* db,
              int userId,
              const std::string& opponent,
              const std::string& mode,
              const std::string& result,
              const std::string& movesJson)
{
    const char* sql =
      "INSERT INTO Games (user_id, opponent, mode, result, moves) "
      "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "saveGame prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int   (stmt, 1, userId);
    sqlite3_bind_text  (stmt, 2, opponent.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3, mode.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 4, result.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 5, movesJson.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

std::vector<Game> loadGameHistory(sqlite3* db,
                                  int userId,
                                  int limit)
{
    const char* sql =
      "SELECT id, user_id, opponent, mode, result, moves, timestamp "
      "FROM Games "
      "WHERE user_id = ? "
      "ORDER BY timestamp DESC "
      "LIMIT ?;";

    sqlite3_stmt* stmt = nullptr;
    std::vector<Game> history;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "loadGameHistory prepare failed: " << sqlite3_errmsg(db) << "\n";
        return history;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Game g;
        g.id        = sqlite3_column_int   (stmt, 0);
        g.user_id   = sqlite3_column_int   (stmt, 1);
        g.opponent  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        g.mode      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        g.result    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        g.moves     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        g.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        history.push_back(g);
    }

    sqlite3_finalize(stmt);
    return history;
}

