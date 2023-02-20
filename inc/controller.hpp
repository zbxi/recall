#pragma once

#include "note.hpp"

#include <filesystem>

namespace zbxi::recall
{
  // Used by the view to control the Model
  class Locator;
  class Controller
  {
  public:
    Controller(Locator& locator);
    ~Controller();

    bool openVault(std::filesystem::path path, std::string* errorMessage);
    void addNoteTag(std::filesystem::path path, std::string tag);
    void setNoteLabel(std::filesystem::path path, Note::Label tag);

  private:
    Locator& m_locator;
  };
}
