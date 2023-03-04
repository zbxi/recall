#include "screen/categorySelector.hpp"

namespace zbxi::recall::component
{
  CategorySelector::CategorySelector(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    ScreenComponent{presenter, controller, callbacks}
  {
    buildTagList();
    buildComponent();
  }

  void CategorySelector::buildComponent()
  {
    using namespace ftxui;
    MenuOption tagMenuOption{
      .on_enter = [this] {
        std::string tag = m_entries.at(m_menuEntry);
        spawnChild<Recaller>(tag);
      },
    };
    Component tagMenu = Menu(&m_entries, &m_menuEntry, tagMenuOption);

    Component window = Renderer(tagMenu, [tagMenu] {
      return vbox({
        text("Select your category:") | center,
        separator(),
        tagMenu->Render(),
      });
    });
    m_component = window | borderRounded | center | CatchEvent(std::bind(&CategorySelector::navigation, this, std::placeholders::_1));
  }

  bool CategorySelector::navigation(ftxui::Event event)
  {
    using namespace ftxui;
    if(event == Event::Character('`')) {
      m_callbacks.minimize();
      return true;
    }
    return false;
  }

  void CategorySelector::buildTagList()
  {
    auto const& notes = m_presenter.notekeeper().notes();
    for(auto& note : notes) {
      auto& tags = note.tags();
      m_tags.insert(tags.begin(), tags.end());
    }

    m_entries.assign(m_tags.begin(), m_tags.end());
  }
}
