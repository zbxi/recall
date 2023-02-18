#include "screen/home.hpp"

namespace zbxi::recall::component
{
  Home::Home(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    ScreenComponent{presenter, controller, callbacks}
  {
    buildEntries();
    buildComponent();
  }

  void Home::buildEntries()
  {
    std::vector menuEntries = {
      "Recaller",
      "File Explorer",
      "Configuration",
      "Statistics",
    };

    // File Explorer
    auto path = m_presenter.notekeeper().vaultFolder().path();
    auto id = typeHash<FileExplorer>();
    auto ptr = std::make_unique<FileExplorer>(m_presenter, m_controller, m_callbacks, path);
    m_callbacks.add(id, std::move(ptr));
    m_entryIds.insert({"File Explorer", id});

    for(auto& e : menuEntries) {
      m_entries.push_back(e);
    }
  }

  void Home::buildComponent()
  {
    using namespace ftxui;
    MenuOption menuOption{
      .on_enter = [this] {
        auto entry = m_entries.at(m_menuEntry);
        if(m_entryIds.contains(entry) && m_entryIds.at(entry) != m_invalidId) {
          m_callbacks.open(m_entryIds.at(entry));
        }
      },
    };

    Component menu = Menu(&m_entries, &m_menuEntry, menuOption);
    m_component = menu | borderRounded | center | CatchEvent([this](ftxui::Event event) -> bool { return basicQuitHandler(event); });
  }
}
