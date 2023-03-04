#include "notekeeper.hpp"

namespace zbxi::recall
{
  Notekeeper::Notekeeper(std::filesystem::path vaultPath) :
    m_vaultFolder{std::make_unique<Folder>(vaultPath)}
  {
    connectToDatabase(std::filesystem::path{vaultPath}.append("recaller.db"));
    queryDatabase();
    updateQuery();
    readVault();
  }

  Notekeeper::~Notekeeper()
  {
    saveToDatabase();
    sqlite3_close_v2(m_connection);
  }

  void Notekeeper::connectToDatabase(std::filesystem::path databasePath)
  {
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

  void Notekeeper::initDatabase()
  {
    std::string statement{
      "CREATE TABLE " + m_tableName + "(" +
        "name VARCHAR(255) UNIQUE,"
        "label VARCHAR(24),"
        "modificationDate INTEGER,"
        "recallDate INTEGER,"
        "intervalDuration INTEGER,"
        "easeModifier REAL,"
        "tags VARCHAR"
        ");",
    };

    checkSqlite(sqlite3_exec(m_connection, statement.c_str(), nullptr, nullptr, nullptr));
  }

  auto Notekeeper::modificationDate(std::filesystem::path path) -> std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
  {
    std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
    std::chrono::system_clock::time_point systemTime = std::chrono::file_clock::to_sys(fileTime);
    // return std::chrono::duration_cast<std::chrono::milliseconds>(systemTime.time_since_epoch()).count();
    auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(systemTime);
    return timePoint;
  }

  bool Notekeeper::openNote(std::string name)
  {
    std::filesystem::path path{};
    assert(m_vaultFolder->fullPathOf(name, &path));
    if(!std::filesystem::is_regular_file(path)) {
      return false;
    }

    std::string escapedName{};
    for(auto& e : name) {
      if(std::isspace(e)) {
        escapedName.append("%20");
        continue;
      }
      if(e == '%') {
        escapedName.append("%25");
        continue;
      }
      escapedName.push_back(e);
    }

    std::string vaultName = this->vaultName();
    std::string link = "obsidian://open?vault=" + vaultName + "&file=" + escapedName;
    std::string command = "xdg-open '" + link + "' 2>/dev/null 1>&2";
    std::system(command.c_str());
    return true;
  }

  void Notekeeper::queryDatabase()
  {
    sqlite3_stmt* stmt{};
    std::string statement = "SELECT * FROM " + m_tableName;
    checkSqlite(sqlite3_prepare_v2(m_connection, statement.c_str(), statement.length(), &stmt, nullptr));
    DataRow row{};
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      namespace chrono = std::chrono;

      auto& now = chrono::system_clock::now;
      Timepoint<chrono::milliseconds> modificationDate{};
      {
        auto dur = chrono::milliseconds{sqlite3_column_int64(stmt, 2)};
        chrono::system_clock::time_point point = chrono::system_clock::time_point(dur);
        modificationDate = chrono::time_point_cast<chrono::milliseconds>(point);
      }

      Timepoint<chrono::seconds> recallDate{};
      {
        auto dur = chrono::seconds(sqlite3_column_int64(stmt, 3));
        chrono::system_clock::time_point point = chrono::time_point<chrono::system_clock>(dur);
        recallDate = chrono::time_point_cast<chrono::seconds>(point);

        if(recallDate.time_since_epoch().count() == 0) {
          recallDate = chrono::time_point_cast<chrono::seconds>(now());
        }
      }

      Duration intervalDuration = chrono::seconds{sqlite3_column_int64(stmt, 4)};

      row = {
        .name = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 0)),
        .label = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1)),
        .modificationDate = modificationDate,
        .recallDate = recallDate,
        .intervalDuration = intervalDuration,
        .easeMod = sqlite3_column_double(stmt, 5),
        .tags = parseTags(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 6))),
      };

      std::filesystem::path path{};
      assert(m_vaultFolder->fullPathOf(row.name, &path));
      m_databaseTable.insert({path, std::move(row)});
    }
    sqlite3_finalize(stmt);
  }

  void Notekeeper::updateQuery()
  {
    // Delete deprecated entries
    for(auto it = m_databaseTable.begin(); it != m_databaseTable.end(); ++it) {
      auto& [path, row] = *it;

      if(!std::filesystem::exists(path)) {
        m_databaseTable.erase(path);
      } else if(row.modificationDate != modificationDate(path)) {
        m_databaseTable.at(path).label = Note::getLabelText(Note::Label::none);
        row.modificationDate = modificationDate(path);
      }
    }

    // Query filesystem vault for new entries
    for(auto& path : std::filesystem::recursive_directory_iterator(m_vaultFolder->path())) {
      if(!path.is_regular_file() ||
         path.path().extension() != ".md" ||
         m_databaseTable.contains(path.path())) {
        continue;
      }

      std::filesystem::path name{};
      if(!m_vaultFolder->relativePathOf(path, &name)) {
        throw std::runtime_error("Failed to convert relative path");
      }
      queryNote(name, path);
    }
  }

  void Notekeeper::queryNote(std::string name, std::filesystem::path path)
  {
    DataRow row{
      .name = name,
      .label = Note::getLabelText(Note::Label::none),
      .modificationDate = modificationDate(path),
      .recallDate = {},
      .intervalDuration = {},
      .easeMod = Note::maximalEase(),
      .tags = {},
    };

    m_databaseTable.insert({path, std::move(row)});
  }

  void Notekeeper::readVault()
  {
    for(auto& [path, row] : m_databaseTable) {
      readNote(path, row);
    }
  }

  void Notekeeper::readNote(std::filesystem::path path, DataRow const& row)
  {
    std::fstream file{path, file.binary | file.in | file.ate};

    if(!file.is_open()) {
      throw std::runtime_error("Failed to open note: \"" + row.name + "\"");
    }

    //// File size
    std::fstream::pos_type fileSize = file.tellg();
    file.seekg(file.beg);

    //// Read
    std::string buffer{};
    buffer.resize(fileSize);
    file.read(&buffer[0], fileSize);

    using namespace std::chrono;
    m_notes.push_back(Note{
      path,
      std::move(buffer),
      row.name,
      Note::getLabel(row.label),                // label Note::Label
      row.modificationDate,                     // modification
      row.recallDate,                           // recall
      row.intervalDuration,                     //
      row.easeMod,                              // ease modifier
      std::move(m_databaseTable.at(path).tags), // tags  std::vector<string>
    });
  }

  auto Notekeeper::parseTags(std::string tags) -> std::vector<std::string>
  {
    std::vector<std::string> result{};

    for(std::size_t i{}; i < tags.length(); ++i) {
      result.push_back("");
      while(i < tags.length() && tags.at(i) != '|') {
        result.back().push_back(tags.at(i));
        ++i;
      }
    };

    return result;
  }

  auto Notekeeper::compressTags(std::vector<std::string> const& tags) -> std::string
  {
    std::string result{};
    for(auto& e : tags) {
      for(auto& c : e) {
        if(c == '|') { throw std::runtime_error("Invalid tag"); }
      }

      if(!result.empty()) { result.push_back('|'); }
      result.append(e);
    };
    return result;
  }

  auto Notekeeper::noteByPath(std::filesystem::path path) -> Note&
  {
    for(auto& e : m_notes) {
      if(e.path() == path) {
        return e;
      }
    }
    throw std::runtime_error("Failed to find note by path");
  }

  auto Notekeeper::noteByPath(std::filesystem::path path) const -> Note const&
  {
    return const_cast<Notekeeper*>(this)->noteByPath(path);
  }

  void Notekeeper::saveToDatabase()
  {
    for(auto& e : m_notes) {
      auto& row = m_databaseTable.at(e.path());
      row.name = e.name(),
      row.label = Note::getLabelText(e.label());
      row.modificationDate = e.modificationDate();
      row.recallDate = e.recallDate(),
      row.intervalDuration = e.intervalDuration(),
      row.easeMod = e.easeModifier();
      row.tags = {e.tags().begin(), e.tags().end()};
    };

    std::string statement = "INSERT OR REPLACE INTO " + m_tableName + "(name, label, modificationDate, recallDate, intervalDuration, easeModifier, tags) VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt{};
    checkSqlite(sqlite3_prepare_v2(m_connection, statement.c_str(), statement.length(), &stmt, nullptr));

    for(auto& [path, row] : m_databaseTable) {
      using namespace std::chrono;
      std::int64_t modificationDate = row.modificationDate.time_since_epoch().count();
      std::int64_t recallDate = row.recallDate.time_since_epoch().count();
      std::int64_t intervalDuration = row.intervalDuration.count();

      std::string tags = compressTags(row.tags);
      sqlite3_bind_text(stmt, 1, row.name.c_str(), row.name.length(), SQLITE_STATIC);
      sqlite3_bind_text(stmt, 2, row.label.c_str(), row.label.length(), SQLITE_STATIC);
      sqlite3_bind_int64(stmt, 3, modificationDate);
      sqlite3_bind_int64(stmt, 4, recallDate);
      sqlite3_bind_int64(stmt, 5, intervalDuration);
      sqlite3_bind_double(stmt, 6, row.easeMod);
      sqlite3_bind_text(stmt, 7, tags.c_str(), tags.length(), SQLITE_STATIC);

      checkSqlite(sqlite3_step(stmt), SQLITE_DONE);
      checkSqlite(sqlite3_reset(stmt));
    }

    checkSqlite(sqlite3_finalize(stmt));
  }

  void Notekeeper::checkSqlite(int result, int expected)
  {
    if(result != expected) {
      throw std::runtime_error(sqlite3_errmsg(m_connection));
    }
  }
}
