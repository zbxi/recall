#include "program.hpp"

namespace zbxi::recall
{
  Program::Program()
  {
  }

  void Program::run()
  {
    using namespace zbxi;
    std::filesystem::path path{"/tmp/Projects.md"};
    recall::Parser p;
    recall::Note note = p.readNote(path);

    printTime(note);
    // printHeaders(headers);
  }

  void Program::printHeaders(std::span<zbxi::recall::Header> headers)
  {
    auto printChildren = [](std::span<zbxi::recall::Header> headers) {
      auto constexpr separator = "----------------------------->";
      for(auto& h : headers) {
        std::cout << "<--- level " << (int)h.level << " " << separator << std::endl;
        std::cout << h.text;
        std::cout << "<----------------------------------------->" << std::endl;
      }
    };

    printChildren(headers);
    for(auto& h : headers) {
      printChildren(h.children);
    }
  }

  void Program::printTime(zbxi::recall::Note& note)
  {
    auto time = std::chrono::system_clock::to_time_t(note.modificationDate());
    std::cout << std::asctime(std::localtime(&time));
  }
}
