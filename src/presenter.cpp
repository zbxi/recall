#include "presenter.hpp"

#include "locator.hpp"

namespace zbxi::recall
{

  Presenter::Presenter(Locator& locator) :
    m_locator{locator}
  {
  }

  Presenter::~Presenter()
  {
  }

  auto Presenter::notekeeper() const -> Notekeeper const&
  {
    return m_locator.notekeeper();
  }

  auto Presenter::configuration() const -> Configuration const&
  {
    return m_locator.configuration();
  }
}
