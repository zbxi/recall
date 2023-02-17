#pragma once

#include <filesystem>
#include <span>
#include <sqlite3.h>
//
#include "locator.hpp"
#include "tui.hpp"

namespace zbxi::recall
{
  class Program
  {
  public:
    Program();
    ~Program();
    void run();

  private:
    void printHeaders(std::span<zbxi::recall::Header> headers);
    void printTime(zbxi::recall::Note& note);
    void printTags(Note& note);

  private:
    Locator m_locator;
    Presenter m_presenter;
    Controller m_controller;
    std::unique_ptr<Tui> m_interface;
  };
}
