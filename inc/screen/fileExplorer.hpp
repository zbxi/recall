#pragma once

#include "screen/screenComponent.hpp"

#include <cstdlib>
#include <sstream>

namespace zbxi::recall::component
{
  class FileExplorer : public ScreenComponent
  {
  public:
    struct EntryInfo
    {
      std::string name;
      std::filesystem::path path;

      std::function<void()> onUpdateLabel{};
    };

    FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path);
    ~FileExplorer() = default;

  private:
    void buildComponent() override;
    void queryFiles();
    void queryLabels();
    void updatePreview();

    auto getPath(std::size_t entryIndex) -> std::filesystem::path;

    bool navigation(ftxui::Event event);

    auto currentNote() -> Note const& { return m_presenter.notekeeper().noteByPath(getPath(m_menuEntry)); }
    auto currentPath() -> std::filesystem::path { return getPath(m_menuEntry); };
    auto getNote(std::size_t index) -> Note const& { return m_presenter.notekeeper().noteByPath(getPath(index)); }

    ftxui::Event m_lastEvent{};
    ftxui::MenuOption m_menuOption{};
    std::vector<std::string> m_fileNames{};
    std::vector<ftxui::Element> m_entryLabels{};
    int m_menuEntry{};

    ftxui::Component m_previewComponent{};
    bool m_canPreview;
    bool m_shouldPreview;

    Folder m_folder;
  };
}
