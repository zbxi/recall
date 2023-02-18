#pragma once

#include "screen/home.hpp"
#include "screen/screenComponent.hpp"

namespace zbxi::recall::component
{
  class VaultSelector : public ScreenComponent
  {
  public:
    VaultSelector(Presenter& presenter, Controller& controller, Callbacks callbacks);
    ~VaultSelector() = default;

  private:
    void buildComponent() override;
    void openVault(std::filesystem::path path);

    std::string m_errorMessage{};
    std::string m_input{};
    int m_menuEntry{};
  };
}
