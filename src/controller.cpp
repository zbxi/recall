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

  bool Controller::openVault(std::filesystem::path path, std::string* errorMessage)
  {
    auto setMessage = [&](std::string message) {
      if(errorMessage) {
        *errorMessage = message;
      }
    };

    if(!std::filesystem::exists(path)) {
      setMessage("File does not exist");
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
        // Open the notekeeper
        m_locator.setNotekeeper(std::make_unique<Notekeeper>(path));

        // Presenter
        auto& history = m_locator.configuration().vaultHistory();
        if(std::find(history.begin(), history.end(), path) == history.end()) {
          history.push_back(path);
        }

        return valid;
      }

      setMessage("There aren't any .md files there");
    };

    return valid;
  }
}
