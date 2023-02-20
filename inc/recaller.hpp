#pragma once

#include "screen/screenComponent.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>

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
    Recaller(Presenter& presenter, Controller& controller, Callbacks callbacks);
    ~Recaller() = default;

  private:
    void buildComponent() override;
    bool navigation(ftxui::Event event);

    auto interval(Result result, Days currentInterval) -> Days;

    double easeMod{2.5};
    double hardMod{1.2};
    double intervalMod{1.0};
    double easyBonus{1.3};
  };
}
