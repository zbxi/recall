#include "screen/screenComponent.hpp"

namespace zbxi::recall
{
  ScreenComponent::ScreenComponent(Presenter& presenter, Controller& controller, Callbacks callbacks) :
    m_presenter{presenter},
    m_controller{controller},
    m_callbacks{callbacks}
  {
  }

  auto ScreenComponent::component() -> ftxui::Component
  {
    assert(m_component && "nullptr component");
    return m_component;
  }

  auto ScreenComponent::basicQuitHandler(ftxui::Event event) -> bool
  {
    if(event == ftxui::Event::Character('`')) {
      m_callbacks.minimize();
      return true;
    }
    return false;
  }

  auto ScreenComponent::previewElement(std::string const& text) -> ftxui::Element
  {
    using namespace ftxui;
    Elements paragraphs{};
    std::istringstream stream{text};
    std::string line;
    while(std::getline(stream, line)) {
      paragraphs.push_back(hflow(paragraph(line)));
    };

    return vbox(std::move(paragraphs)) | size(WIDTH, EQUAL, 50);
  }

  auto ScreenComponent::entryLabel(std::filesystem::path path) -> ftxui::Element
  {
    using namespace ftxui;
    if(!std::filesystem::is_regular_file(path)) {
      return text(" [i] ") | color(Color::Palette256::DarkSlateGray1);
    }

    auto const& note = m_presenter.notekeeper().noteByPath(path);
    switch(note.label()) {
      case Note::Label::card: {
        return text(" [A] ") | color(Color::Palette256::SeaGreen3);
      }
      case Note::Label::content: {
        return text(" [C] ") | color(Color::Palette256::DarkCyan);
      }
      case Note::Label::ignored: {
        return text(" [I] ") | color(Color::Palette256::Grey11);
      }
      case Note::Label::map_of_content: {
        return text(" [M] ") | color(Color::Palette256::DeepSkyBlue4);
      }
      case Note::Label::none: {
        return text(" [N] ") | color(Color::Palette256::LightSteelBlue);
      }
    }
  }
}
