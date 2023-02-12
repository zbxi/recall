#include "controller.hpp"

namespace zbxi::recall
{
  Controller::Controller(Presenter* presenter) :
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

    if(std::filesystem::is_directory(path)) {
      // searches for are any .md
      for(auto& dir_entry : std::filesystem::directory_iterator(path)) {
        if(dir_entry.path().extension() == ".md") {
          m_presenter->errorMessage() = {};
          m_presenter->vaultHistory().push_back(path);
          m_presenter->inputStrings().front() = {};
          return true;
        }
      }

      m_presenter->errorMessage() = "There aren't any .md files there";
    };

    return false;
  }
}
