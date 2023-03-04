#pragma once

#include "categorySelector.hpp"
#include "screen/screenComponent.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <optional>
#include <set>

namespace zbxi::recall::component
{
  class Recaller : public ScreenComponent
  {
    template<typename T>
    using Timepoint = std::chrono::time_point<std::chrono::system_clock, T>;
    using Seconds = std::chrono::seconds;
    enum class Result
    {
      None = -1,
      Again = 0,
      Hard = 1,
      Good = 2,
      Easy = 3,
    };

  public:
    Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks, std::string tag);
    ~Recaller() = default;

  private:
    void buildComponent() override;
    bool navigation(ftxui::Event event);

    void buildQueues();
    bool queryNote();
    auto intervalIncrement(Result result, Seconds currentInterval, double, double) -> std::pair<Seconds, double>;
    auto currentNote() -> Note const& { return m_presenter.notekeeper().noteByPath(m_currentNotePath); }
    void updateIncrement(Result);
    void applySelection(Result);
    auto incrementText() -> ftxui::Element;


    std::filesystem::path m_currentNotePath;
    std::deque<std::reference_wrapper<Note const>> m_queue{};
    std::string m_tag{};

    std::chrono::seconds m_recallInterval{};

    ftxui::Component m_menu{};
    ftxui::MenuOption m_menuOption{};
    int m_menuEntry{};
    std::vector<std::string> m_menuEntries = {
      "  [0]  Again ",
      "  [1]  Hard  ",
      "  [2]  Good  ",
      "  [3]  Easy  ",
    };

    std::int64_t m_minimalInterval{600}; // In seconds
    double m_hardModifier{1.2};          // Hard only interval modifier
    double m_intervalModifier{1.0};      // Global interval multiplier
    double m_easeBonus{1.2};             // Interval increment for easy
    double m_currentEase{};
    bool m_invalid{true};
  };
}
