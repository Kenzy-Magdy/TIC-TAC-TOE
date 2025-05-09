#include "auth.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "sha256.h"


std::string sha256(const std::string& str) {
    uint8_t hash[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t*)str.c_str(), str.size());
    sha256_final(&ctx, hash);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

bool registerUser(sqlite3* db, const std::string& username, const std::string& password) {
    std::string hashed = sha256(password); // Hash the password
    std::cout << "[DEBUG] Hashed password (register): " << hashed << std::endl;

    const char* sql = "INSERT INTO Users (username, password_hash) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Registration failed: username might already exist.\n";
        return false;
    }

    std::cout << "User registered successfully!\n";
    return true;
}
bool loginUser(sqlite3* db,
               const std::string& username,
               const std::string& password,
               int& outUserId)
{
    const char* sql =
        "SELECT id, password_hash FROM Users WHERE username = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare login query: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // Bind the username
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    // Try to fetch a row
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cerr << "Login failed: user not found.\n";
        sqlite3_finalize(stmt);
        return false;
    }

    // Extract database values
    int dbUserId = sqlite3_column_int(stmt, 0);
    const unsigned char* dbHashText = sqlite3_column_text(stmt, 1);
    std::string storedHash = reinterpret_cast<const char*>(dbHashText);

    // Hash the provided password
    std::string hashedInput = sha256(password);

    // Debug prints
    std::cout << "[DEBUG] stored hash: " << storedHash << "\n";
    std::cout << "[DEBUG] input  hash: " << hashedInput << "\n";

    sqlite3_finalize(stmt);

    // Compare
    if (hashedInput == storedHash) {
        outUserId = dbUserId;
        return true;
    } else {
        std::cerr << "Login failed: wrong password\n";
        return false;
    }
}
