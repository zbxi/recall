#pragma once

#include "ftxui/component/captured_mouse.hpp"     // ftxui
#include "ftxui/component/component.hpp"          // Menu
#include "ftxui/component/component_options.hpp"  // MenuOption
#include "ftxui/component/screen_interactive.hpp" // ScreenInteractive
#include "ftxui/dom/elements.hpp"
//
#include "configuration.hpp"
#include "controller.hpp"
#include "folder.hpp"
#include "notekeeper.hpp"
#include "presenter.hpp"
//
#include <algorithm>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace zbxi::recall
{
  class ScreenComponent
  {
  public:
    struct Callbacks
    {
      std::function<void(std::size_t id)> open{};
      std::function<void()> close{};
      std::function<void(std::size_t id, std::unique_ptr<ScreenComponent>)> add{};
    };

    ScreenComponent(Presenter& presenter, Controller& controller, Callbacks callbacks);
    virtual ~ScreenComponent() = default;
    virtual auto component() -> ftxui::Component;

  protected:
    virtual void buildComponent() = 0;
    bool basicQuitHandler(ftxui::Event event);

    template<typename T, typename... Args>
    requires(std::is_base_of<ScreenComponent, T>())
    void spawnChild(Args&&...);

    template<typename T>
    auto typeHash()
    {
      return typeid(T).hash_code();
    }

    ftxui::Component m_component{};
    Presenter& m_presenter;
    Controller& m_controller;
    Callbacks m_callbacks;
  };

  namespace component
  {
    class VaultSelector : public ScreenComponent
    {
    public:
      VaultSelector(Presenter& presenter, Controller& controller, Callbacks callbacks);
      ~VaultSelector() = default;

    private:
      void buildComponent() override;
      void openVault(std::filesystem::path path);

      std::string m_errorMessage{};
      std::string m_input{};
      int m_menuEntry{};
    };

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

    class Recaller : public ScreenComponent
    {
    public:

    private:
    };

    class FileExplorer : public ScreenComponent
    {
    public:
      FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path);
      ~FileExplorer() = default;

    private:
      void buildComponent();
      void queryFiles();

      int m_menuEntry{};
      Folder m_folder;
      std::vector<std::string> m_menuEntries{};
    };

    class Statistics : public ScreenComponent
    {
    public:

    private:
    };

    class Configuration : public ScreenComponent
    {
    public:

    private:
    };
  }
}
