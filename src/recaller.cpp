#include "recaller.hpp"

namespace zbxi::recall::component
{
  Recaller::Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    ScreenComponent{presenter, controller, callbacks}
  {
  }

  bool Recaller::navigation(ftxui::Event event)
  {
    using namespace ftxui;
    if(event == Event::Character('1')) {
      return true;
    }
    if(event == Event::Character('2')) {
      return true;
    }
    if(event == Event::Character('3')) {
      return true;
    }
    if(event == Event::Character('4')) {
      return true;
    }
    if(event == Event::Character('`')) {
      m_callbacks.minimize();
      return false;
    }
    return false;
  }

  auto Recaller::interval(Result result, Days currentInterval) -> Days
  {
    double newInterval{};
    using enum Result;
    switch(result) {
      case Easy: {
        newInterval = currentInterval.count() * easeMod * intervalMod * easyBonus;
        easeMod += 0.15;
      }
      case Good: {
        newInterval = currentInterval.count() * easeMod * intervalMod;
      }
      case Hard: {
        newInterval = currentInterval.count() * hardMod * intervalMod;
        easeMod -= 0.15;
      }
      case Again: {
        newInterval /= 2;
        easeMod -= 0.2;
      }
    }

    easeMod = std::min(easeMod, 2.5);
    easeMod = std::max(easeMod, 1.0);

    return Days{newInterval};
  }

  void Recaller::buildComponent()
  {
    using namespace ftxui;
    Component window;
    std::string noteName{};
    auto box = vbox({
      text(noteName),
      separator(),
      text(" [1]  Again "),
      text(" [2]  Hard  "),
      text(" [3]  Good  "),
      text(" [4]  Easy  "),
    });
    m_component = window | borderRounded | center | CatchEvent(std::bind(&Recaller::navigation, this, std::placeholders::_1));
  }
}
