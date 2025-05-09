#include "db.h"
#include <iostream>

bool execNonQuery(sqlite3* db, const std::string& sql) {
  char* errMsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return false;
  }
  return true;
}
const char* CREATE_USERS = R"sql(
  CREATE TABLE IF NOT EXISTS Users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL
  );
)sql";

const char* CREATE_GAMES = R"sql(
  CREATE TABLE IF NOT EXISTS Games (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    opponent TEXT NOT NULL,
    mode TEXT CHECK(mode IN ('AI','PVP')) NOT NULL,
    result TEXT CHECK(result IN ('Win','Lose','Draw')) NOT NULL,
    moves TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(user_id) REFERENCES Users(id)
  );
)sql";
void initDatabase(sqlite3* db) {
  if (!execNonQuery(db, CREATE_USERS)) {
    std::cerr << "Failed to create Users table\n";
    exit(1);
  }
  if (!execNonQuery(db, CREATE_GAMES)) {
    std::cerr << "Failed to create Games table\n";
    exit(1);
  }
}

