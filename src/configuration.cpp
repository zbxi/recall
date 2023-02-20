#include "configuration.hpp"

namespace zbxi::recall
{
  Configuration::Configuration(std::filesystem::path configPath)
  {
    openDatabase(configPath);
  }

  Configuration::~Configuration()
  {
    saveToDatabase();
    sqlite3_close_v2(m_connection);
  }

  auto Configuration::vaultHistory() -> std::vector<std::string>&
  {
    if(!m_queried) {
      getVaultHistory();
      m_queried = true;
    }
    return m_vaultHistory;
  }

  auto Configuration::vaultHistory() const -> std::vector<std::string> const&
  {
    return const_cast<Configuration*>(this)->vaultHistory();
  }

  void Configuration::openDatabase(std::filesystem::path configPath)
  {
    // Create config directory
    if(!std::filesystem::exists(configPath)) {
      if(!std::filesystem::create_directory(configPath)) {
        throw std::runtime_error("Failed to create configuration directory");
      }
    }

    std::filesystem::path databasePath = configPath.string() + '/' + m_databaseName;
    bool novel{};
    if(!std::filesystem::exists(databasePath)) {
      novel = true;
    }

    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    checkSqlite(sqlite3_open_v2(databasePath.c_str(), &m_connection, flags, nullptr));

    if(novel) {
      initDatabase();
    }
  }

  void Configuration::initDatabase()
  {
    std::string statement{
      "CREATE TABLE " + m_tableName + "(" +
        "path VARCHAR(255) PRIMARY KEY"
        ");",
    };

    checkSqlite(sqlite3_exec(m_connection, statement.c_str(), nullptr, nullptr, nullptr));
  }

  void Configuration::getVaultHistory()
  {
    sqlite3_stmt* stmt{};
    std::string statement = "SELECT path FROM " + m_tableName;
    checkSqlite(sqlite3_prepare_v2(m_connection, statement.c_str(), statement.length(), &stmt, nullptr));
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::filesystem::path entry = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 0));
      m_vaultHistory.push_back(entry);
    }
    checkSqlite(sqlite3_finalize(stmt));
  }

  void Configuration::checkSqlite(int result, int expected)
  {
    if(result != expected) {
      throw std::runtime_error(sqlite3_errmsg(m_connection));
    }
  }

  void Configuration::saveToDatabase()
  {
    std::string statement = "INSERT OR IGNORE INTO " + m_tableName + " (path) VALUES(?)";
    sqlite3_stmt* stmt{};
    checkSqlite(sqlite3_prepare_v2(m_connection, statement.c_str(), statement.length(), &stmt, nullptr));

    for(auto& e : m_vaultHistory) {
      sqlite3_bind_text(stmt, 1, e.c_str(), e.length(), SQLITE_STATIC);
      checkSqlite(sqlite3_step(stmt), SQLITE_DONE);
      checkSqlite(sqlite3_reset(stmt));
    }

    checkSqlite(sqlite3_finalize(stmt));
  }
}
