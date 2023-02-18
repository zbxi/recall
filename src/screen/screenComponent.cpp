#include "screen/screenComponent.hpp"

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
}
