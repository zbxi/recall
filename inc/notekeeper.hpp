#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <memory>
#include <span>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <vector>
//
#include "folder.hpp"
#include "note.hpp"

namespace zbxi::recall
{
  // Manages Vault-specific Data
  class Notekeeper
  {
  public:
    Notekeeper(std::filesystem::path vaultPath);
    ~Notekeeper();

    auto vaultPath() const -> std::string const& { return m_vaultPath; }
    auto notes() -> std::span<Note> { return m_notes; }

  private:
    void connectToDatabase(std::filesystem::path databasePath);
    void initDatabase();
    void parseNotes();
    void readNote(std::filesystem::path path);
    void saveToDatabase();

    void checkSqlite(int result, int expected = SQLITE_OK);

    // zbxi::recall::Note Callback
    bool newTag(std::string_view tag, std::span<std::string>* updatedTags);

  private:
    sqlite3* m_connection{nullptr};
    std::vector<Note> m_notes{};
    std::vector<std::string> m_tags{};
    std::string m_vaultPath{};
    std::string m_tableName{"Notes"};
    std::unique_ptr<Folder> m_vaultFolder{};
  };
}
