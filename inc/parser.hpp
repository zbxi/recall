#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
//
#include <ctime>
#include <iostream>
//
#include "note.hpp"

namespace zbxi::recall
{
  class Parser
  {
  public:
    auto readNote(std::filesystem::path path) -> Note;

  private:
  };
}
