#pragma once

#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace zbxi::recall
{
  class Configuration
  {
  public:
    Configuration(std::filesystem::path configPath);
    ~Configuration();

    auto vaultHistory() -> std::vector<std::string>&;
    auto vaultHistory() const -> std::vector<std::string> const&;

  private:
    void openDatabase(std::filesystem::path configPath);
    void initDatabase();
    void getVaultHistory();
    void saveToDatabase();

    void checkSqlite(int result, int expected = SQLITE_OK);

    sqlite3* m_connection{};
    bool m_queried{};

    std::vector<std::string> m_vaultHistory{};

    std::string m_tableName{"VaultHistory"};
    std::string m_databaseName{"recall.db"};
  };
}
