#pragma once

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

  private:
    Locator& m_locator;
  };
}
