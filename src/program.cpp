#include "program.hpp"

namespace zbxi::recall
{
  Program::Program()
  {
    m_notekeeper = std::make_unique<Notekeeper>("/tmp/database.db");
    m_presenter = std::make_unique<Presenter>(m_notekeeper.get());
    m_controller = std::make_unique<Controller>(m_presenter.get());
    m_interface = std::make_unique<Tui>(m_controller.get(), m_presenter.get());
  }

  void Program::run()
  {
    m_interface->run();
    return;
    std::filesystem::path path{"/tmp/Projects.md"};
    m_notekeeper->readNote(path);

    // printHeaders(headers);
    printTime(m_notekeeper->notes().front());
    printTags(m_notekeeper->notes().front());
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

  void Program::printTags(Note& note)
  {
    std::cout << "Tags: [ ";
    for(auto& e : note.tags()) {
      std::cout << e << " ";
    }
    std::cout << "]" << std::endl;
  }
}
