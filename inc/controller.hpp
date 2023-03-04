#pragma once

#include "note.hpp"

#include <filesystem>

namespace zbxi::recall
{
  // Used by the view to control the Model
  class Locator;
  class Controller
  {
    template<typename T>
    using Timepoint = std::chrono::time_point<std::chrono::system_clock, T>;

  public:
    Controller(Locator& locator);
    ~Controller();

    bool openVault(std::filesystem::path, std::string*);
    void openNote(std::string);
    void addNoteTag(std::filesystem::path, std::string);
    void setNoteLabel(std::filesystem::path, Note::Label);
    void incrementNoteRecall(std::filesystem::path, std::chrono::seconds);
    void setNoteEaseModifier(std::filesystem::path, double);

  private:
    Locator& m_locator;
  };
}
