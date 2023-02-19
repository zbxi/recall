#include "recaller.hpp"

namespace zbxi::recall
{
  // just testing
  enum class Result
  {
    Fail,
    PassHard,
    PassNormal,
    PassEasy,
  };

  // void interval(
  //   std::chrono::days delay,
  //   std::int_fast16_t factor,
  //   double intervalModifier,
  //   double intervalModifierEasy,
  //   double intervalModifierNew,
  //   Result result)
  // {
  //   assert(factor >= 0 && factor <= 1300 && "Invalid factor");
  //
  //   double recentInterval{};
  //   // auto d = delay.count();
  //   // auto& i = recentInterval;
  //   // auto& m = intervalModifier;
  //   // auto& m0 = intervalModifierNew;
  //   // auto& m4 = intervalModifierEasy;
  //
  //   using enum Result;
  //   switch(result) {
  //     case Fail: {
  //       i = m0 * i;
  //     }
  //     case PassHard: {
  //       // i = std::max(i + 1, (i + d / 4) * 1.2 * m);
  //     }
  //     case PassNormal: {
  //       // i = std::max(i)
  //     }
  //     case PassEasy: {
  //     }
  //   }
  // }
}
