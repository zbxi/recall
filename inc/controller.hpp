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

    bool openVault(std::filesystem::path, std::string*);
    void openNote(std::filesystem::path);
    void addNoteTag(std::filesystem::path, std::string);
    void setNoteLabel(std::filesystem::path, Note::Label);
    void setNoteRecallDate(std::filesystem::path, std::chrono::system_clock::time_point);

  private:
    Locator& m_locator;
  };
}
