#pragma once

#include "categorySelector.hpp"
#include "screen/screenComponent.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <set>

namespace zbxi::recall::component
{
  class Recaller : public ScreenComponent
  {
    using Days = std::chrono::duration<double, std::ratio<86400>>;
    enum class Result
    {
      Easy,
      Good,
      Hard,
      Again,
    };

  public:
    Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks, std::string tag);
    ~Recaller() = default;

  private:
    void buildComponent() override;
    bool navigation(ftxui::Event event);

    void buildQueue();
    auto interval(Result result, Days currentInterval) -> Days;

    double easeMod{2.5};
    double hardMod{1.2};
    double intervalMod{1.0};
    double easyBonus{1.3};

    std::deque<std::reference_wrapper<Note const>> m_queue{};
    std::string m_tag{};
  };
}
