#include "screen/vaultSelector.hpp"

namespace zbxi::recall::component
{
  VaultSelector::VaultSelector(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    ScreenComponent{presenter, controller, callbacks}
  {
    buildComponent();
  }

  void VaultSelector::openVault(std::filesystem::path path)
  {
    if(m_controller.openVault(path, &m_errorMessage)) {
      spawnChild<Home>();
    }
  }

  void VaultSelector::buildComponent()
  {
    using namespace ftxui;
    //-----< Components >-----//

    // history menu
    MenuOption menuOption{
      .on_enter = [this] {
        auto path = m_presenter.configuration().vaultHistory().at(m_menuEntry);
        openVault(path);
      },
    };

    Component menu = Menu(&m_presenter.configuration().vaultHistory(), &m_menuEntry, menuOption);

    // path input
    InputOption inputOption{
      .on_enter = std::bind(&VaultSelector::openVault, this, std::cref(m_input)),
    };
    Component input = Input(&m_input, "type here...", inputOption);

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
    auto shouldShowPreview = [this]() { return !m_presenter.configuration().vaultHistory().empty(); };
    Component selectorComponent = Container::Horizontal({
      left | Maybe(shouldShowPreview),
      Renderer([] { return separator(); }) | Maybe(shouldShowPreview),
      right,
    });

    Component window = Renderer(selectorComponent,
      [this, selectorComponent] {
        auto errorComponent = Renderer([this] { return text(m_errorMessage); });
        auto shouldShowError = [this]() -> bool { return m_errorMessage.empty(); };
        return center(vbox({
          selectorComponent->Render() | borderRounded | center,
          filler() | size(HEIGHT, EQUAL, 1 - shouldShowError()),
          (errorComponent | center | Maybe(shouldShowError))->Render(),
        }));
      });

    m_component = window | center | CatchEvent([this](ftxui::Event event) -> bool { return basicQuitHandler(event); });
  }
}
