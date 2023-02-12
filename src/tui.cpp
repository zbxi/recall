#include "tui.hpp"

namespace zbxi::recall
{
  Tui::Tui(Controller* controller, Presenter* presenter) :
    m_controller{controller},
    m_presenter{presenter},
    m_screen{ftxui::ScreenInteractive::Fullscreen()}
  {
    using namespace ftxui;
    buildWindows();
    auto vaultSelectorWindow = m_windows.at("VaultSelector")();
    auto errorMessageWindow = m_windows.at("ErrorMessage")();

    auto screen = Renderer(vaultSelectorWindow, [this, vaultSelectorWindow, errorMessageWindow]() {
      std::function<bool()> condition = [this] { return !m_presenter->errorMessage().empty(); };
      return vbox({
               vaultSelectorWindow->Render() | center,
               filler() | size(HEIGHT, EQUAL, 1 - condition()),
               (errorMessageWindow | center | Maybe(condition))->Render(),
             }) |
             center;
    });

    m_screen.Loop(screen);
  }

  Tui::~Tui()
  {
    m_screen.ExitLoopClosure()();
  }

  void Tui::run()
  {
  }

  void Tui::buildWindows()
  {
    using namespace ftxui;
    auto vaultSelector = [this] {
      //-----< Components >-----//

      // history menu
      MenuOption menuOption{
        .on_enter = [this] {
          int entry = m_presenter->menuEntry();
          auto path = m_presenter->vaultHistory().at(entry);
          this->m_controller->openVault(path);
        },
      };
      Component menu = Menu(&m_presenter->vaultHistory(), &m_presenter->menuEntry(), menuOption);

      // path input
      InputOption inputOption{
        .on_enter = {[this] {
          auto path = m_presenter->inputStrings().front();
          if(this->m_controller->openVault(path)) {
            m_screen.ExitLoopClosure()();
            m_screen.Loop(m_windows.at("Home")());
          }
        }},
      };
      Component input = Input(&m_presenter->inputStrings().front(), "type here...", inputOption);

      //-----< Renderers >-----//
      Component left = Renderer(menu, [menu]() {
        return vbox({
          text(" Previous vaults ") | center,
          separator(),
          menu->Render(),
        });
      });

      Component right = Renderer(input, [input]() {
        return vbox(
          {
            text(" Open folder as vault ") | center,
            separator(),
            hbox({
              filler() | size(WIDTH, EQUAL, 1),
              frame(input->Render()) | size(WIDTH, LESS_THAN, 24),
              filler() | size(WIDTH, EQUAL, 1),
            }),
          });
      });

      //-----< Window >-----//
      auto showPreview = [this]() { return !m_presenter->vaultHistory().empty(); };
      Component window = Container::Horizontal({
        left | Maybe(showPreview),
        Renderer([] { return separator(); }) | Maybe(showPreview),
        right,
      });
      m_windows.insert({"VaultSelector", [window] { return window | borderRounded; }});
    };

    auto errorMessage = [this] {
      auto errorMessageWindow = Renderer([this] { return text(m_presenter->errorMessage()); });
      m_windows.insert({"ErrorMessage", [errorMessageWindow] { return errorMessageWindow; }});
    };

    auto home = [this] {
      MenuOption menuOption{
        .on_enter = [this] { m_screen.ExitLoopClosure()(); },
      };
      Component menu = Menu(&m_presenter->homeMenuEntries(), &m_presenter->menuEntry(), menuOption);
      // Component preview = paragraph(m_presenter->notekeeper().notes());

      m_windows.insert({"Home", [menu] { return menu; }});
    };

    errorMessage();
    vaultSelector();
    home();
  }
}
