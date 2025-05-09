#include <iostream>
#include "sqlite3.h"
#include "db.h"
#include "auth.h"
#include "game.h"

int main() {
    sqlite3* db;
    if (sqlite3_open("test.db", &db) != SQLITE_OK) {
        std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    initDatabase(db);

    // 1) Registration
    std::string user, pass;
    std::cout << "REGISTER\nUsername: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pass;
    registerUser(db, user, pass);

    // 2) Login
    std::cout << "\nLOGIN\nUsername: ";
    std::cin >> user;
    std::cout << "Password: ";
    std::cin >> pass;

    int userId = -1;
    if (loginUser(db, user, pass, userId)) {
        std::cout << "Login successful! Your user ID is " << userId << "\n";

        // 3) Menu after login
        while (true) {
            std::cout << "\nSelect action:\n"
                      << "1) Save a dummy game\n"
                      << "2) View game history\n"
                      << "0) Exit\n"
                      << "Choice: ";
            int choice;
            std::cin >> choice;

            if (choice == 0) break;

            if (choice == 1) {
                std::string dummyMoves = R"json(["X:0,0","O:1,1","X:0,1","O:2,2","X:0,2"])json";
                bool success = saveGame(db, userId, "AI", "AI", "Win", dummyMoves);
                std::cout << (success ? "Game saved successfully.\n" : "Failed to save game.\n");
            }
            else if (choice == 2) {
                auto history = loadGameHistory(db, userId, 10);
                if (history.empty()) {
                    std::cout << "No game history found.\n";
                } else {
                    std::cout << "Your last games:\n";
                    for (const auto& g : history) {
                        std::cout << "Game ID: " << g.id << " | " << g.timestamp
                                  << "\nMode: " << g.mode << " vs " << g.opponent
                                  << " | Result: " << g.result
                                  << "\nMoves: " << g.moves << "\n\n";
                    }
                }
            }
            else {
                std::cout << "Invalid choice.\n";
            }
        }
    } else {
        std::cout << "Login attempt failed.\n";
    }

    sqlite3_close(db);
    return 0;
}
