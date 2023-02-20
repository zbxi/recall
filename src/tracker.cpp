#include "tracker.hpp"

namespace zbxi::recall::component
{
  Tracker::Tracker(Presenter& presenter, Controller& controller, Callbacks callbacks, FileExplorer::EntryInfo info) :
    ScreenComponent(presenter, controller, callbacks),
    m_info{info}
  {
    updateLabel();
    updateTags();
    buildComponent();
  }

  void Tracker::updatePreview()
  {
    throw std::runtime_error("not implemented");
  }

  void Tracker::updateLabel()
  {
    Note::Label label = currentNote().label();
    m_label = Note::getLabelText(label);
    m_info.onUpdateLabel();
  }

  void Tracker::updateTags()
  {
    m_tags.clear();
    for(auto& e : currentNote().tags()) {
      m_tags += " " + std::string{e} + " ";
    }
  }

  void Tracker::buildComponent()
  {
    using namespace ftxui;

    m_menuOption = {
      .direction = MenuOption::Right,
      .on_enter = [this] {
        m_controller.setNoteLabel(m_info.path, Note::getLabel(m_toggleLabels.at(m_toggleLabelEntry)));
        updateLabel();
      },
    };
    m_inputOption = {
      .on_enter = [this] {
        m_controller.addNoteTag(m_info.path, m_tagInput);
        m_tagInput.clear();
        updateTags();
      },
    };

    Component menu = Menu(&m_toggleLabels, &m_toggleLabelEntry, &m_menuOption);
    Component input = Input(&m_tagInput, "type new tags", &m_inputOption);
    Component descriptions = Renderer([] {
      return vbox({
        filler() | size(WIDTH, EQUAL, 1),
        text("Label"),
        text("Tag"),
        filler() | size(WIDTH, EQUAL, 1),
      });
    });

    // auto may = Maybe([this] { return m_shouldPreview && m_canPreview; });
    // Component preview = Renderer([this] {
    //   auto const& headers = noteHeaders();
    //   return previewElement(std::string{headers.at(m_menuEntry).text});
    // });

    Component options = Container::Vertical({
      menu,
      input,
    });

    Component window = Renderer(options, [this, menu, input] {
      return hbox({vbox({
        text(m_info.name) | center,
        separator(),
        hbox({text(" Label"), filler() | size(WIDTH, EQUAL, 3), menu->Render()}),
        hbox({text(" Tag  "), filler() | size(WIDTH, EQUAL, 3), input->Render()}),
        separator(),
        hbox({text(" Tags: "), text("[" + m_tags + "]")}),
        hbox({text(" Label: "), text(m_label), filler() | size(HEIGHT, EQUAL, 1)}),
      })});
    });

    m_component = window | borderRounded | center | CatchEvent(std::bind(&Tracker::navigation, this, std::placeholders::_1));
  };

  bool Tracker::navigation(ftxui::Event event)
  {
    using namespace ftxui;
    if(event == Event::Character('`')) {
      m_callbacks.minimize();
      return true;
    }
    return false;
  }
}
