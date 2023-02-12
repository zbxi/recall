#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>
//
#include <sqlite3.h>
//
#include <ctime>
#include <iostream>
//
#include "note.hpp"

namespace zbxi::recall
{
  class Notekeeper
  {
  public:
    Notekeeper();
    ~Notekeeper();

    void openVault(std::filesystem::path vaultPath);

    auto vaultPath() -> std::string const& { return m_vaultPath; }
    auto vaultHistory() -> std::vector<std::string>&;
    auto notes() -> std::span<Note> { return m_notes; }
    bool connected() { return m_connection != nullptr; }

  private:
    void connectToDatabase(std::filesystem::path databasePath);
    void initDatabase();
    void parseNotes();
    void readNote(std::filesystem::path path);
    void saveToDatabase();

    void checkSqlite(int result, int expected = SQLITE_OK);

    // Note Callback
    bool newTag(std::string_view tag, std::span<std::string>* updatedTags);

  private:
    std::vector<Note> m_notes{};
    std::vector<std::string> m_tags{};
    //
    sqlite3* m_connection{nullptr};

    std::string m_vaultPath;
    std::string m_tableName{"Notes"};
  };
}
