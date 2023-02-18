#pragma once

#include "screen/screenComponent.hpp"

#include <sstream>

namespace zbxi::recall::component
{
  class FileExplorer : public ScreenComponent
  {
  public:
    FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path);
    ~FileExplorer() = default;

  private:
    void buildComponent();
    void queryFiles();
    void updatePreview();
    auto previewElement(std::string const& text) -> ftxui::Element;

    ftxui::Component m_previewComponent{};

    int m_menuEntry{};
    bool m_shouldPreview;
    std::vector<std::string> m_menuEntries{};
    Folder m_folder;
  };
}