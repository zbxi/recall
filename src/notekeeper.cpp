#include "notekeeper.hpp"

namespace zbxi::recall
{
  Notekeeper::Notekeeper(std::filesystem::path databasePath)
  {
    connectToDatabase(databasePath);
    sqlite3_stmt* stmt{};

    auto prepare = [this, &stmt]() {
      std::string statement = "";
      char const* out_tail{};

      checkSqlite(sqlite3_prepare_v2(m_sqliteConnection, statement.c_str(), statement.length(), &stmt, &out_tail));
    };

    auto evaluate = [this, &stmt]() {

    };
  }

  Notekeeper::~Notekeeper()
  {
    sqlite3_close_v2(m_sqliteConnection);
  }

  void Notekeeper::connectToDatabase(std::filesystem::path databasePath)
  {
    bool novel{};
    if(!std::filesystem::exists(databasePath)) {
      novel = true;
    }

    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    checkSqlite(sqlite3_open_v2(databasePath.c_str(), &m_sqliteConnection, flags, nullptr));

    if(novel) {
      initDatabase();
    }
  }

  void Notekeeper::initDatabase()
  {
    std::string stmt = "CREATE TABLE Notes("
                       "  path VARCHAR(255) PRIMARY KEY,"
                       "  modificationDate INTEGER,"
                       "  label VARCHAR(24),"
                       "  tags VARCHAR)";
    checkSqlite(sqlite3_exec(m_sqliteConnection, stmt.c_str(), nullptr, nullptr, nullptr));
  }

  void Notekeeper::readNote(std::filesystem::path path)
  {
    std::fstream file{path, file.binary | file.in | file.ate};

    if(!file.is_open()) {
      throw std::runtime_error("Failed to open file: \"" + path.string() + "\"");
    }

    //// File size
    std::fstream::pos_type fileSize = file.tellg();
    file.seekg(file.beg);

    //// Read
    std::string buffer{};
    buffer.resize(fileSize);
    file.read(&buffer[0], fileSize);

    //// Unix Time in milliseconds
    std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
    std::chrono::system_clock::time_point systemTime = std::chrono::file_clock::to_sys(fileTime);
    // std::int64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(systemTime.time_since_epoch()).count();

    m_notes.push_back(Note{
      std::move(buffer),
      systemTime,
      std::bind(&Notekeeper::newTag, this, std::placeholders::_1, std::placeholders::_2),
    });
  }

  bool Notekeeper::newTag(std::string_view tag, std::span<std::string>* updatedTags)
  {
    bool novel{true};
    std::size_t index{};
    while(index < m_tags.size()) {
      if(tag == m_tags[index]) {
        novel = false;
        break;
      }
      ++index;
    }

    if(novel) {
      m_tags.push_back(std::string(tag));
      *updatedTags = m_tags;
      return true;
    }

    return false;
  }

  void Notekeeper::checkSqlite(int result)
  {
    if(result != SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(m_sqliteConnection));
    }
  }
}