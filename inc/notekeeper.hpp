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
#include <list>
#include <memory>
#include <set>
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
    template<typename T>
    using Timepoint = std::chrono::time_point<std::chrono::system_clock, T>;
    using Duration = std::chrono::seconds;

    struct DataRow
    {
      std::string name{};
      std::string label{};
      Timepoint<std::chrono::milliseconds> modificationDate{};
      Timepoint<std::chrono::seconds> recallDate{};
      Duration intervalDuration{};
      double easeMod{};
      std::vector<std::string> tags{};
    };

  public:
    Notekeeper(std::filesystem::path vaultPath);
    ~Notekeeper();

    auto vaultName() const -> std::string { return m_vaultFolder->path().stem().string(); }
    auto vaultFolder() const -> Folder const& { return *m_vaultFolder; }
    auto notes() const -> std::vector<Note> { return m_notes; }

    auto noteByPath(std::filesystem::path path) const -> Note const&;
    auto noteByPath(std::filesystem::path path) -> Note&;

    static auto modificationDate(std::filesystem::path path) -> std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
    bool openNote(std::string);

  private:
    void connectToDatabase(std::filesystem::path databasePath);
    void initDatabase();
    void queryDatabase();
    void updateQuery();
    void readVault();
    void queryNote(std::string name, std::filesystem::path path);
    void readNote(std::filesystem::path path, DataRow const& row);
    auto parseTags(std::string tags) -> std::vector<std::string>;
    auto compressTags(std::vector<std::string> const& tags) -> std::string;
    void saveToDatabase();

    void checkSqlite(int result, int expected = SQLITE_OK);

  private:
    sqlite3* m_connection{nullptr};
    std::vector<Note> m_notes{};
    std::string m_tableName{"Notes"};
    std::unique_ptr<Folder> m_vaultFolder{};
    std::unordered_map<std::string, DataRow> m_databaseTable{};
  };
}
