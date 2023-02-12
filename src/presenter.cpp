#include "presenter.hpp"

namespace zbxi::recall
{

  Presenter::Presenter(Notekeeper* notekeeper) :
    m_notekeeper{notekeeper}
  {
    m_userInputStrings.resize(1);
  }

  Presenter::~Presenter()
  {
  }

  auto Presenter::vaultHistory() -> std::vector<std::string>&
  {
    return m_vaultHistory;
  }
}
