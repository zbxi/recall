#pragma once

#include "controller.hpp"
#include "presenter.hpp"
#include "screen/screenComponent.hpp"
#include "screen/vaultSelector.hpp"

#include <list>
#include <stack>
#include <type_traits>
#include <unordered_map>

namespace zbxi::recall
{
  class Tui
  {
  public:
    Tui(Presenter& presenter, Controller& controller);
    ~Tui();

    void run();

  private:
    void add(std::size_t id, std::unique_ptr<ScreenComponent> screenComponent);
    void open(ftxui::Component component);
    void open(std::size_t id);
    void minimize();

    void insert(std::size_t id, std::unique_ptr<ScreenComponent>);

    template<typename T>
    auto typeHash()
    {
      return typeid(T).hash_code();
    }

    std::stack<ftxui::Closure> m_exitClosures;
    std::unordered_map<std::size_t, std::unique_ptr<ScreenComponent>> m_screenComponents{};

    Presenter& m_presenter;
    Controller& m_controller;
  };
}
