#include "program.hpp"

namespace zbxi::recall
{
  Program::Program()
  {
    m_configuration = std::make_unique<Configuration>();
    m_notekeeper = std::make_unique<Notekeeper>();
    m_presenter = std::make_unique<Presenter>(m_notekeeper.get(), m_configuration.get());
    m_controller = std::make_unique<Controller>(m_notekeeper.get(), m_presenter.get());
    m_interface = std::make_unique<Tui>(m_controller.get(), m_presenter.get());
  }

  Program::~Program()
  {
  }

  void Program::run()
  {
    m_interface->run();
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
