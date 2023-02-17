#pragma once

#include "configuration.hpp"
#include "controller.hpp"
#include "notekeeper.hpp"

#include <cassert>
#include <memory>

namespace zbxi::recall
{
  class Locator
  {
    template<typename T>
    using unique = std::unique_ptr<T>;

  public:
    Locator() = default;
    ~Locator() = default;

    auto notekeeper() -> Notekeeper&;
    auto configuration() -> Configuration&;

    void setNotekeeper(unique<Notekeeper> notekeeper) { m_notekeeper = std::move(notekeeper); }
    void setConfiguration(unique<Configuration> configuration) { m_configuration = std::move(configuration); }

  private:
    unique<Notekeeper> m_notekeeper{};
    unique<Configuration> m_configuration{};
  };
}
