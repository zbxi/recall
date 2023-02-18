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

  template<typename T, typename... Args>
  requires(std::is_base_of<ScreenComponent, T>())
  void ScreenComponent::spawnChild(Args&&... args)
  {
    std::size_t id = typeHash<T>();
    auto pointer = std::make_unique<T>(m_presenter, m_controller, m_callbacks, args...);
    m_callbacks.add(id, std::move(pointer));
    m_callbacks.open(id);
  }
}
