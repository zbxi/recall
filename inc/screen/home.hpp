#pragma once

#include "screen/categorySelector.hpp"
#include "screen/fileExplorer.hpp"
#include "screen/recaller.hpp"
#include "screen/screenComponent.hpp"

namespace zbxi::recall::component
{
  class Home : public ScreenComponent
  {
  public:
    Home(Presenter& presenter, Controller& controller, Callbacks callbacks);
    ~Home() = default;

  private:
    void buildEntries();
    void buildComponent() override;

    int m_menuEntry{};
    std::unordered_map<std::string, std::size_t> m_entryIds{};
    std::vector<std::string> m_entries{};
    std::size_t m_invalidId = std::numeric_limits<std::size_t>::max();
  };
}
