#include "controller.hpp"

namespace zbxi::recall
{
  Controller::Controller(Notekeeper* notekeeper, Presenter* presenter) :
    m_notekeeper{notekeeper},
    m_presenter{presenter}
  {
  }

  Controller::~Controller()
  {
  }

  bool Controller::openVault(std::filesystem::path path)
  {
    if(!std::filesystem::exists(path)) {
      m_presenter->errorMessage() = "File does not exist";
      return false;
    }

    bool valid{false};
    if(std::filesystem::is_directory(path)) {
      // searches for any .md file
      for(auto& dir_entry : std::filesystem::directory_iterator(path)) {
        if(dir_entry.path().extension() == ".md") {
          valid = true;
          break;
        }
      }

      if(valid) {
        // Notekeeper
        m_notekeeper->openVault(path);

        // Presenter
        m_presenter->errorMessage() = {};
        m_presenter->inputStrings().front() = {};
        m_presenter->setCurrentPath("");
        auto& history = m_presenter->vaultHistory();
        if(std::find(history.begin(), history.end(), path) == history.end()) {
          history.push_back(path);
        }

        return true;
      }

      m_presenter->errorMessage() = "There aren't any .md files there";
    };

    return valid;
  }
}
