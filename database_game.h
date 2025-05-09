#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

// Represents one saved game
struct Game {
    int    id;
    int    user_id;
    std::string opponent;
    std::string mode;    // "AI" or "PVP"
    std::string result;  // "Win", "Lose", "Draw"
    std::string moves;   // JSON or CSV of moves
    std::string timestamp;
};

// Inserts a new game record; returns true on success
bool saveGame(sqlite3* db,
              int userId,
              const std::string& opponent,
              const std::string& mode,
              const std::string& result,
              const std::string& movesJson);

// Loads up to `limit` past games for `userId`, most recent first
std::vector<Game> loadGameHistory(sqlite3* db,
                                  int userId,
                                  int limit = 10);

