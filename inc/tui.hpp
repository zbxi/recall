#pragma once

#include "ftxui/component/captured_mouse.hpp"    // ftxui
#include "ftxui/component/component.hpp"         // Menu
#include "ftxui/component/component_options.hpp" // MenuOption
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp" // ScreenInteractive
#include "ftxui/dom/elements.hpp"

#include <stack>
#include <unordered_map>
//
#include "controller.hpp"
#include "presenter.hpp"

namespace zbxi::recall
{
  class Tui
  {
  public:
    Tui(Controller* controller, Presenter* presenter);
    ~Tui();

    void run();

  private:
    void buildScreens();
    void loop(ftxui::Component screenComponent);
    void exit();

    std::unordered_map<std::string, std::function<ftxui::Component()>> m_screenComponents{};
    // std::unordered_map<std::string, ftxui::Component> m_components{};

    Controller* m_controller{nullptr};
    Presenter* m_presenter{nullptr};
    std::stack<ftxui::Closure> m_exitClosures;
  };
}
