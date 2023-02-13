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
    Configuration();
    ~Configuration();

    bool connected() { return m_connection != nullptr; }
    auto vaultHistory() -> std::vector<std::string>&;

  private:
    void initDatabase();
    void getVaultHistory();
    void connectToDatabase(std::string databaseName);
    void saveToDatabase();

    void checkSqlite(int result, int expected = SQLITE_OK);

    sqlite3* m_connection{};
    std::vector<std::string> m_vaultHistory{};
    std::string m_tableName{"VaultHistory"};

    bool m_queried{};
  };
}
