#include "screen/recaller.hpp"

namespace zbxi::recall::component
{
  Recaller::Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks, std::string tag) :
    ScreenComponent{presenter, controller, callbacks},
    m_tag{tag}
  {
    buildQueues();
    if(queryNote()) {
      updateIncrement(static_cast<Result>(m_menuEntry));
    };
    buildComponent();
  }

  void Recaller::buildQueues()
  {
    using namespace std::chrono;
    m_queue.clear();
    auto const& notes = m_presenter.notekeeper().notes();
    auto now = system_clock::now();
    for(auto& note : notes) {
      if(note.tags().contains(m_tag)) {
        if(note.recallDate() <= now) {
          m_queue.push_back(std::ref(note));
        }
      }
    }
  }

  bool Recaller::queryNote()
  {
    if(!m_queue.empty()) {
      m_currentNotePath = m_queue.front().get().path();
      m_queue.pop_front();
      m_invalid = false;
    } else {
      m_invalid = true;
      return false;
    };

    return true;
  }

  void Recaller::buildComponent()
  {
    using namespace ftxui;

    m_menuOption = {
      .on_change = [this] { updateIncrement(static_cast<Result>(m_menuEntry)); },
      .on_enter = [this] { 
        m_menuEntry = 0;
        m_menuOption.focused_entry = m_menuEntry; 
        applySelection(static_cast<Result>(m_menuEntry)); },
    };

    m_menu = Menu(&m_menuEntries, &m_menuEntry, &m_menuOption);

    auto window = Renderer(m_menu, [this] {
      if(m_invalid) {
        return hbox({text("No available notes")});
      }

      Element selector = vbox({
        text(" [" + m_tag + "] ") | center,
        text(" " + currentNote().name() + " ") | center,
        m_menu->Render(),
      });

      auto noteInfo = vbox({
        text(" next recall ") | center,
        incrementText(),
      });

      return hbox({
        selector,
        separator(),
        noteInfo,
      });
    });

    m_component = window | borderRounded | center | CatchEvent(std::bind(&Recaller::navigation, this, std::placeholders::_1));
  }

  auto Recaller::incrementText() -> ftxui::Element
  {
    using namespace ftxui;
    using Hours = std::chrono::duration<double, std::chrono::hours::period>;
    using Days = std::chrono::duration<double, std::chrono::days::period>;

    Seconds durationSecs = m_recallInterval;
    Days durationDays = std::chrono::duration_cast<Days>(m_recallInterval);
    Hours durationHours = std::chrono::duration_cast<Hours>(m_recallInterval);

    std::stringstream stream{};
    stream << std::fixed << std::setprecision(2);
    Element txt{};
    if(durationDays.count() > 1.0) {
      stream << durationDays.count();
      txt = text(" " + stream.str() + " days ") | center;
    } else if(durationHours.count() > 1.0) {
      stream << durationHours.count();
      txt = text(" " + stream.str() + " hours ") | center;
    } else {
      txt = text(" " + std::to_string(durationSecs.count()) + " seconds ") | center;
    }

    return txt;
  }

  auto Recaller::intervalIncrement(Result result, Seconds currentInterval, double easeModifier, double easeBonus) -> std::pair<Seconds, double>
  {
    assert(easeModifier > 0 && "Invalid modifier");

    using enum Result;
    using namespace std::chrono;
    currentInterval = seconds{std::max(currentInterval.count(), m_minimalInterval)};

    double newInterval{};
    switch(result) {
      case Again: {
        newInterval = currentInterval.count() / 2.0;
        easeModifier -= 0.2;
        break;
      }
      case Hard: {
        newInterval = currentInterval.count() * m_hardModifier * m_intervalModifier;
        easeModifier -= 0.15;
        break;
      }
      case Good: {
        newInterval = currentInterval.count() * easeModifier * m_intervalModifier;
        break;
      }
      case Easy: {
        newInterval = currentInterval.count() * easeModifier * m_intervalModifier * easeBonus;
        easeModifier += 0.15;
        break;
      }
      case None: {
        newInterval = currentInterval.count();
        break;
      }
    }

    auto max = std::max(static_cast<std::int64_t>(newInterval), m_minimalInterval);
    seconds intervalSeconds = seconds{max};
    return {intervalSeconds, easeModifier};
  }

  void Recaller::updateIncrement(Result result)
  {
    m_recallInterval = {};

    // Recall increment
    Seconds currentInterval = currentNote().intervalDuration();
    double easeModifier = currentNote().easeModifier();

    auto [interval, ease] = intervalIncrement(result, currentInterval, easeModifier, m_easeBonus);
    m_currentEase = ease;
    m_recallInterval = interval;
  }

  void Recaller::applySelection(Result result)
  {
    m_controller.setNoteEaseModifier(m_currentNotePath, m_currentEase);
    m_controller.incrementNoteRecall(m_currentNotePath, m_recallInterval);
    // queryNote();
    updateIncrement(result);
  }

  bool Recaller::navigation(ftxui::Event event)
  {
    using namespace ftxui;
    if(event == Event::Character('`')) {
      m_callbacks.minimize();
      return true;
    }

    return false;
  }
}
