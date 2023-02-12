#pragma once

#include "notekeeper.hpp"
#include "presenter.hpp"

#include <filesystem>

namespace zbxi::recall
{
  // Used by the view to control the Model
  class Controller
  {
  public:
    Controller(Notekeeper* notekeeper, Presenter* presenter);
    ~Controller();

    bool openVault(std::filesystem::path path);

  private:
    Notekeeper* m_notekeeper; // avoiding std::shared_ptr
    Presenter* m_presenter;
  };
}
