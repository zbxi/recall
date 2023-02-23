#pragma once

#include "screen/recaller.hpp"
#include "screen/screenComponent.hpp"

namespace zbxi::recall::component
{
  class CategorySelector : public ScreenComponent
  {
  public:
    CategorySelector(Presenter& presenter, Controller& controller, Callbacks callbacks);
    ~CategorySelector() = default;

  private:
    void buildComponent() override;
    bool navigation(ftxui::Event event);

    void buildTagList();

    std::set<std::string> m_tags{};
    std::vector<std::string> m_entries{};
    int m_menuEntry{};
  };
}
