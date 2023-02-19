#pragma once

#include <cassert>
#include <deque>
#include <filesystem>
#include <string>
#include <vector>

namespace zbxi::recall
{
  class Locator;
  class Notekeeper;
  class Configuration;

  class Presenter
  {
  public:
    Presenter(Locator& m_locator);
    ~Presenter();

    auto notekeeper() const -> Notekeeper const&;
    auto configuration() const -> Configuration const&;

  private:
    Locator& m_locator;
  };
}
