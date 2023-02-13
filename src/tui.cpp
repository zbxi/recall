#include "tui.hpp"

namespace zbxi::recall
{
  Tui::Tui(Controller* controller, Presenter* presenter) :
    m_controller{controller},
    m_presenter{presenter}
  {
  }

  Tui::~Tui()
  {
  }

  void Tui::run()
  {
    using namespace ftxui;
    buildScreens();
    loop(m_screenComponents.at("Vault Selector")());
  }

  void Tui::loop(ftxui::Component screenComponent)
  {
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
    m_exitClosures.push(screen.ExitLoopClosure());
    ftxui::Loop loop{&screen, screenComponent};
    while(!loop.HasQuitted()) {
      loop.RunOnceBlocking();
    }
  }

  void Tui::exit()
  {
  }

  void Tui::buildScreens()
  {
    using namespace ftxui;
    auto quitHandler = [this](Event event) {
      if(event == Event::Character('q')) {
        auto exit = m_exitClosures.top();
        m_exitClosures.pop();
        exit();
        return true;
      }
      return false;
    };

    auto vaultSelector = [&, this] {
      //-----< Components >-----//

      // history menu
      MenuOption menuOption{
        .on_enter = [this] {
          int& entry = m_presenter->menuEntrySelector();
          auto path = m_presenter->vaultHistory().at(entry);
          if(this->m_controller->openVault(path)) {
            loop(m_screenComponents.at("Home")());
          }
        }};
      Component menu = Menu(&m_presenter->vaultHistory(), &m_presenter->menuEntrySelector(), menuOption);

      // path input
      InputOption inputOption{
        .on_enter = {[this] {
          auto path = m_presenter->inputStrings().front();
          if(this->m_controller->openVault(path)) {
            loop(m_screenComponents.at("Home")());
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

      //-----< Windows >-----//
      auto shouldShowPreview = [this]() { return !m_presenter->vaultHistory().empty(); };
      Component selectorComponent = Container::Horizontal({
        left | Maybe(shouldShowPreview),
        Renderer([] { return separator(); }) | Maybe(shouldShowPreview),
        right,
      });

      Component window = Renderer(selectorComponent,
        [this, selectorComponent] {
          auto errorComponent = Renderer([this] { return text(m_presenter->errorMessage()); });
          auto shouldShowError = [this]() -> bool { return !m_presenter->errorMessage().empty(); };
          return center(vbox({
            selectorComponent->Render() | borderRounded | center,
            filler() | size(HEIGHT, EQUAL, 1 - shouldShowError()),
            (errorComponent | center | Maybe(shouldShowError))->Render(),
          }));
        });

      Component screenComponent = window | center | CatchEvent(quitHandler);
      m_screenComponents.insert({"Vault Selector", [screenComponent] { return screenComponent; }});
    };

    auto home = [&, this] {
      MenuOption menuOption{
        .on_enter = [this] {
          int& entry = m_presenter->menuEntryHome();
          auto option = m_presenter->homeMenuEntries().at(entry);
          if(this->m_screenComponents.contains(option)) {
            loop(m_screenComponents.at(option)());
          }
        },
      };

      Component menu = Menu(&m_presenter->homeMenuEntries(), &m_presenter->menuEntryHome(), menuOption);
      Component screenComponent = menu | borderRounded | center | CatchEvent(quitHandler);
      m_screenComponents.insert({"Home", [screenComponent] { return screenComponent; }});
    };

    auto fileExplorer = [&, this] {
      MenuOption menuOption{
        .on_enter = {},
      };
      Component menu = Menu(&m_presenter->explorerEntries(), &m_presenter->menuEntryExplorer(), menuOption);

      auto shouldPreview = []() -> bool {
        return true;
      };
      Component preview = Renderer([] {
        return hbox({
          separator(),
          text("preview"),
        });
      }) | Maybe(shouldPreview);

      Component window = Renderer(menu, [menu, preview] {
        return hbox({
          frame(menu->Render()),
          (preview)->Render(),
        });
      });

      Component screenComponent = window | borderRounded | center | CatchEvent(quitHandler);
      m_screenComponents.insert({"File Explorer",
        [this, screenComponent] {
          static_cast<void>(m_presenter->explorerEntries());
          return screenComponent;
        }});
    };

    vaultSelector();
    home();
    fileExplorer();
  }
}
