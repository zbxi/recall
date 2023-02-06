#include "parser.hpp"

namespace zbxi::recall
{
  auto Parser::readNote(std::filesystem::path path) -> Note
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

    return Note{
      std::move(buffer),
      systemTime,
    };
  }
}
