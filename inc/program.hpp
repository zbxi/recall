#pragma once

#include <filesystem>
#include <span>
#include <sqlite3.h>
//
#include "controller.hpp"
#include "note.hpp"
#include "notekeeper.hpp"
#include "presenter.hpp"
#include "tui.hpp"


namespace zbxi::recall
{
  class Program
  {
  public:
    Program();
    void run();

  private:
    void printHeaders(std::span<zbxi::recall::Header> headers);
    void printTime(zbxi::recall::Note& note);
    void printTags(Note& note);

  private:
    std::unique_ptr<Notekeeper> m_notekeeper{};
    std::unique_ptr<Presenter> m_presenter{};
    std::unique_ptr<Tui> m_interface{};
    std::unique_ptr<Controller> m_controller{};
  };
}
