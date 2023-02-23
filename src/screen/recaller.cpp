#include "screen/recaller.hpp"

namespace zbxi::recall::component
{
  Recaller::Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks, std::string tag) :
    ScreenComponent{presenter, controller, callbacks},
    m_tag{tag}
  {
    buildQueue();
    buildComponent();
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
      return true;
    }
    return false;
  }

  void Recaller::buildQueue()
  {
    using namespace std::chrono;
    auto const& notes = m_presenter.notekeeper().notes();
    auto now = system_clock::now();
    for(auto& note : notes) {
      if(note.tags().contains(m_tag) &&
         note.recallDate() <= now) {
        m_queue.push_back(std::ref(note));
      }
    }
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
    std::string noteName{"my note name"};
    auto window = Renderer([this, noteName] {
      return vbox({
        text(" [" + m_tag + "] ") | center,
        text(noteName) | center,
        separator(),
        text(" [1]  Again "),
        text(" [2]  Hard  "),
        text(" [3]  Good  "),
        text(" [4]  Easy  "),
      });
    });
    m_component = window | borderRounded | center | CatchEvent(std::bind(&Recaller::navigation, this, std::placeholders::_1));
  }
}
