#pragma once
#include <string>
#include <sqlite3.h>

bool registerUser(sqlite3* db, const std::string& username, const std::string& password);

bool loginUser(sqlite3* db,
               const std::string& username,
               const std::string& password,
               int& outUserId);
