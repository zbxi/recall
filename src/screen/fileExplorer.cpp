#include "screen/fileExplorer.hpp"

namespace zbxi::recall::component
{
  FileExplorer::FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path) :
    ScreenComponent(presenter, controller, callbacks),
    m_folder{path}
  {
    queryFiles();
    updatePreview();
    buildComponent();
  }

  void FileExplorer::buildComponent()
  {
    using namespace ftxui;

    MenuOption menuOption{
      .on_change = std::bind(&FileExplorer::updatePreview, this),
      .on_enter = {},
    };
    Component menu = Menu(&m_menuEntries, &m_menuEntry, menuOption) | frame;

    m_previewComponent = Renderer([this] {
      namespace fs = std::filesystem;
      fs::path notePath{};
      if(!m_folder.pathOf(m_menuEntries.at(m_menuEntry), &notePath)) {
        throw std::runtime_error("Failed to identify note path");
      }

      Note const& note = m_presenter.notekeeper().noteByPath(notePath);
      return previewElement(std::string{note.text()});
    });

    auto may = Maybe([this] { return m_shouldPreview; });
    auto window = Container::Horizontal({
      menu,
      Renderer([] { return separator(); }) | may,
      m_previewComponent | may,
    });

    m_component = window | borderRounded | center | CatchEvent([this](ftxui::Event event) -> bool { return basicQuitHandler(event); });
  }

  void FileExplorer::updatePreview()
  {
    using namespace ftxui;
    namespace fs = std::filesystem;
    fs::path path;
    m_folder.pathOf(m_menuEntries.at(m_menuEntry), &path);
    if(fs::is_regular_file(path)) {
      m_shouldPreview = true;
    } else {
      m_shouldPreview = false;
    }
  }

  auto FileExplorer::previewElement(std::string const& text) -> ftxui::Element
  {
    using namespace ftxui;
    Elements paragraphs{};
    std::istringstream stream{text};
    std::string line;
    while(std::getline(stream, line)) {
      paragraphs.push_back(hflow(paragraph(line)));
    };

    return vbox(std::move(paragraphs)) | frame;
  }

  void FileExplorer::queryFiles()
  {
    namespace fs = std::filesystem;
    m_menuEntries.clear();

    // Folders
    for(auto& e : m_folder.folders()) {
      auto path = e.path();
      if(path.string().front() != '.') {
        m_menuEntries.push_back(path);
      }
    }

    // Files
    for(auto& e : m_folder.files()) {
      if(fs::is_regular_file(e) && e.extension() == ".md") {
        m_menuEntries.push_back(e);
      }
    }

    auto fileName = [this](std::filesystem::path e) -> std::string {
      std::size_t pathLength = m_presenter.notekeeper().vaultFolder().path().string().length() + 1; // +1 for the '/'
      std::string entryStr = e.string();
      std::string path = std::string{entryStr.begin() + pathLength, entryStr.end()};

      if(fs::is_regular_file(e)) {
        assert(e.extension() == ".md");
        path.resize(path.length() - 3); // -3 for the '.md'
      } else if(fs::is_directory(e)) {
        path.append("/");
      }

      return path;
    };

    for(auto& e : m_menuEntries) {
      e = fileName(e);
    }
  }
}
