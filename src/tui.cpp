#include "tui.hpp"

namespace zbxi::recall
{
  Tui::Tui(Presenter& presenter, Controller& controller) :
    m_presenter{presenter},
    m_controller{controller}
  {
  }

  Tui::~Tui()
  {
  }

  void Tui::run()
  {
    using namespace ftxui;
    ScreenComponent::Callbacks callbacks{
      .open = [this](std::size_t id) { open(id); },
      .close = std::bind(&Tui::close, this),
      .add = std::bind(&Tui::add, this, std::placeholders::_1, std::placeholders::_2),
    };

    std::unique_ptr<ScreenComponent> vaultSelector = std::make_unique<component::VaultSelector>(m_presenter, m_controller, callbacks);

    m_screenComponents.insert({typeHash<component::VaultSelector>(), std::move(vaultSelector)});
    open(m_screenComponents.at(typeHash<component::VaultSelector>())->component());
  }

  void Tui::open(ftxui::Component screenComponent)
  {
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
    m_exitClosures.push(screen.ExitLoopClosure());
    screen.Loop(screenComponent);
  }

  void Tui::open(std::size_t id)
  {
    open(m_screenComponents.at(id)->component());
  }

  void Tui::add(std::size_t id, std::unique_ptr<ScreenComponent> screenComponent)
  {
    assert(!m_screenComponents.contains(id) && "non-unique insertion");
    m_screenComponents.insert({id, std::move(screenComponent)});
  }

  void Tui::close()
  {
    auto exit = m_exitClosures.top();
    m_exitClosures.pop();
    exit();
  }
}
