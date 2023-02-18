#include "program.hpp"

int main()
{
  std::filesystem::path path = "/tmp/notes/Game Loop.md";
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

  zbxi::recall::Note note{buffer, {}, {}};

  return 0;
  try {
    zbxi::recall::Program{}.run();
  }
  catch(std::runtime_error const& e) {
    std::cout << "[RuntimeError] " << e.what() << std::endl;
  }
}
