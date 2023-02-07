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
    Notekeeper(std::filesystem::path databasePath);

    void readNote(std::filesystem::path path);
    auto notes() -> std::span<Note> { return m_notes; }

  private:
    bool newTag(std::string_view tag, std::span<std::string>* updatedTags);
    void connectToDatabase(std::filesystem::path databasePath);

  private:
    std::vector<Note> m_notes{};
    std::vector<std::string> m_tags{};
  };
}
