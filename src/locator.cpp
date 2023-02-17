#include "locator.hpp"

namespace zbxi::recall
{
  auto Locator::notekeeper() -> Notekeeper&
  {
    assert(m_notekeeper && "nullptr to Notekeeper");
    return *m_notekeeper;
  }

  auto Locator::configuration() -> Configuration&
  {
    assert(m_configuration && "nullptr to Configuration");
    return *m_configuration;
  }
}
