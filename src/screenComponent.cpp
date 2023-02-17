#include "screenComponent.hpp"

namespace zbxi::recall
{
  ScreenComponent::ScreenComponent(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    m_presenter{presenter},
    m_controller{controller},
    m_callbacks{callbacks}
  {
  }

  auto ScreenComponent::component() -> ftxui::Component
  {
    assert(m_component && "nullptr component");
    return m_component;
  }

  auto ScreenComponent::basicQuitHandler(ftxui::Event event) -> bool
  {
    if(event == ftxui::Event::Character('q')) {
      m_callbacks.close();
      return true;
    }
    return false;
  }

  template<typename T, typename... Args>
  requires(std::is_base_of<ScreenComponent, T>())
  void ScreenComponent::spawnChild(Args&&... args)
  {
    std::size_t id = typeHash<T>();
    auto pointer = std::make_unique<T>(m_presenter, m_controller, m_callbacks, args...);
    m_callbacks.add(id, std::move(pointer));
    m_callbacks.open(id);
  }

  namespace component
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
      auto path = m_presenter.notekeeper().vaultPath();
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

    FileExplorer::FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path) :
      ScreenComponent(presenter, controller, callbacks),
      m_folder{path}
    {
      queryFiles();
      buildComponent();
    }

    void FileExplorer::buildComponent()
    {
      using namespace ftxui;
      MenuOption menuOption{
        .on_enter = {},
      };
      Component menu = Menu(&m_menuEntries, &m_menuEntry, menuOption);

      auto shouldPreview = []() -> bool {
        return true;
      };

      Component preview = Renderer([] {
        return hbox({
          separator(),
          vbox({frame(text("preview"))}),
        });
      }) | Maybe(shouldPreview);

      Component window = Renderer(menu, [menu, preview] {
        return hbox({
          frame(menu->Render()),
          (preview)->Render(),
        });
      });

      m_component = window | borderRounded | center | CatchEvent([this](ftxui::Event event) -> bool { return basicQuitHandler(event); });
    }

    void FileExplorer::queryFiles()
    {
      namespace fs = std::filesystem;
      m_menuEntries.clear();

      // Folders
      for(auto& e : m_folder.folders()) {
        auto path = e.path();
        if(path.string().front() != '.') {
          m_menuEntries.push_back(path);
        }
      }

      // Files
      for(auto& e : m_folder.files()) {
        if(fs::is_regular_file(e) && e.extension() == ".md") {
          m_menuEntries.push_back(e);
        }
      }

      auto fileName = [this](std::filesystem::path e) -> std::string {
        std::size_t pathLength = m_presenter.notekeeper().vaultPath().length() + 1; // +1 for the '/'
        std::string entryStr = e.string();
        std::string path = std::string{entryStr.begin() + pathLength, entryStr.end()};

        if(fs::is_regular_file(e)) {
          assert(e.extension() == ".md");
          path.resize(path.length() - 3); // -3 for the '.md'
        } else if(fs::is_directory(e)) {
          path.append("/");
        }

        return path;
      };

      for(auto& e : m_menuEntries) {
        e = fileName(e);
      }
    }
  }
}
