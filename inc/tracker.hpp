#pragma once

#include "screen/fileExplorer.hpp"
#include "screen/screenComponent.hpp"

namespace zbxi::recall::component
{
  class Tracker : public ScreenComponent
  {
  public:
    Tracker(Presenter& presenter, Controller& controller, Callbacks callbacks, FileExplorer::EntryInfo info);
    ~Tracker() = default;

  private:
    void buildComponent() override;
    void updatePreview();
    void updateTags();
    void updateLabel();
    bool navigation(ftxui::Event event);

    auto currentNote() -> Note const& { return m_presenter.notekeeper().noteByPath(m_info.path); }

    FileExplorer::EntryInfo m_info{};
    ftxui::MenuOption m_menuOption;
    ftxui::InputOption m_inputOption;

    int m_toggleLabelEntry{};
    std::vector<std::string> m_toggleLabels{
      "None",
      "Ignored",
      "MOC",
      "Content",
      "Card",
    };

    std::string m_tagInput{};
    std::string m_tags{};
    std::string m_label{};
  };
}
