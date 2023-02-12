#pragma once

#include "configuration.hpp"
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
    Presenter(Notekeeper* notekeeper, Configuration* Configuration);
    ~Presenter();

    auto menuEntrySelector() -> int& { return m_selectorMenuEntry; }
    auto vaultHistory() -> std::vector<std::string>& { return m_configuration->vaultHistory(); }
    auto menuEntryHome() -> int& { return m_homeMenuEntry; }
    auto inputStrings() -> std::vector<std::string>& { return m_userInputStrings; }
    auto errorMessage() -> std::string& { return m_errorMessage; }
    auto homeMenuEntries() -> std::vector<std::string> const& { return m_homeMenuEntries; }
    auto explorerEntries() -> std::vector<std::string>&;
    auto menuEntryExplorer() -> int& { return m_explorerMenuEntry; }

    void setCurrentPath(std::filesystem::path);

  private:
    void queryFolderFiles(std::filesystem::path path);

    Notekeeper* m_notekeeper{};
    Configuration* m_configuration{};

    std::string m_errorMessage{};
    std::vector<std::string> m_userInputStrings{};
    int m_selectorMenuEntry{};
    int m_homeMenuEntry{};
    int m_explorerMenuEntry{};

    std::vector<std::string> m_homeMenuEntries{
      "Recaller",
      "File Explorer",
      "Configuration",
      "Statistics",
    };

    bool m_explorerQueried;
    std::vector<std::string> m_explorerEntries{"1", "2", "3"};
    std::filesystem::path m_currentPath{};
  };
}
