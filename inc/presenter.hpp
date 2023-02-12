#pragma once

#include "notekeeper.hpp"

#include <string>
#include <vector>

namespace zbxi::recall
{
  // Used to retrieve, store and present data to the View.
  // The TUI class does not store its UI data, relying on Presenter for that.
  // Can potentially handle things like caching.
  class Presenter
  {
  public:
    Presenter(Notekeeper* notekeeper);
    ~Presenter();

    auto vaultHistory() -> std::vector<std::string>&;

    auto inputStrings() -> std::vector<std::string>& { return m_userInputStrings; }
    auto menuEntry() -> int& { return m_menuEntry; }
    auto errorMessage() -> std::string& { return m_errorMessage; }
    auto homeMenuEntries() -> std::vector<std::string> const& { return m_homeMenuEntries; }
    auto notes() -> Notekeeper& { return *m_notekeeper; }

  private:
    Notekeeper* m_notekeeper{};

    std::string m_vaultPath{};
    std::string m_errorMessage{};
    std::vector<std::string> m_userInputStrings{};
    int m_menuEntry{};

    std::vector<std::string> m_vaultHistory{};
    std::vector<std::string> m_homeMenuEntries{
      "Recaller",
      "File Explorer",
      "Configuration",
      "Statistics",
    };
  };
}
