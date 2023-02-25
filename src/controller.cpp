#include "controller.hpp"

#include "locator.hpp"

namespace zbxi::recall
{
  Controller::Controller(Locator& locator) :
    m_locator{locator}
  {
  }

  Controller::~Controller()
  {
  }

  void Controller::addNoteTag(std::filesystem::path path, std::string tag)
  {
    auto& note = m_locator.notekeeper().noteByPath(path);
    note.addTag(tag);
  }

  void Controller::setNoteLabel(std::filesystem::path path, Note::Label label)
  {
    auto& note = m_locator.notekeeper().noteByPath(path);
    note.setLabel(label);
  }

  void Controller::setNoteRecallDate(std::filesystem::path path, std::chrono::system_clock::time_point date)
  {
    auto& note = m_locator.notekeeper().noteByPath(path);
    note.setRecallDate(date);
  }

  bool Controller::openVault(std::filesystem::path path, std::string* errorMessage)
  {
    auto setMessage = [&](std::string message) {
      if(errorMessage) {
        *errorMessage = message;
      }
    };

    bool valid{false};
    if(!std::filesystem::exists(path)) {
      setMessage("File does not exist");
      return valid;
    }

    if(!std::filesystem::is_directory(path)) {
      setMessage("Path does not represent a directory");
      return valid;
    }

    // searches for any .md file
    for(auto& dir_entry : std::filesystem::directory_iterator(path)) {
      if(dir_entry.path().extension() == ".md") {
        valid = true;
        break;
      }
    }

    if(!valid) {
      setMessage("Directory does not contain any .md file");
      return valid;
    }

    // Open the notekeeper
    m_locator.setNotekeeper(std::make_unique<Notekeeper>(path));

    // Presenter
    auto& history = m_locator.configuration().vaultHistory();
    if(std::find(history.begin(), history.end(), path) == history.end()) {
      history.push_back(path);
    }

    return valid;
    setMessage("There aren't any .md files there");

    return valid;
  }

  void Controller::openNote(std::string name)
  {
    m_locator.notekeeper().openNote(name);
  }
}
