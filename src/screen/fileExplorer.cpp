#include "screen/fileExplorer.hpp"

#include "tracker.hpp"

namespace zbxi::recall::component
{
  FileExplorer::FileExplorer(Presenter& presenter, Controller& controller, Callbacks callbacks, std::filesystem::path path) :
    ScreenComponent(presenter, controller, callbacks),
    m_folder{path}
  {
    queryFiles();
    queryLabels();
    updatePreview();
    buildComponent();
  }

  void FileExplorer::buildComponent()
  {
    using namespace ftxui;

    MenuEntryOption entryOption{
      .transform = [](EntryState const& state) {
        Element textElement = text(" " + state.label);
        if(state.focused) {
          textElement = textElement | inverted;
        }
        if(state.active) {
          textElement = textElement | bold;
        }
        return textElement;
      },
    };

    m_menuOption = {
      // .elements_prefix = [] { return text("#prefix"); },
      // .elements_postfix = [] { return text("#postfix"); },
      .entries = entryOption,
      .on_change = std::bind(&FileExplorer::updatePreview, this),
      .on_enter = [this] {
        if(!std::filesystem::is_regular_file(currentPath())) {
          return;
        }
        auto const& note = currentNote();
        spawnChild<Tracker>(EntryInfo{
          .name = note.name(),
          .path = note.path(),
          .onUpdateLabel = std::bind(&FileExplorer::queryLabels, this)});
      },
    };

    Component menu = Menu(&m_fileNames, &m_menuEntry, &m_menuOption);
    Component menuComponent = Renderer(menu, [this, menu] {
      return hbox({vbox(m_entryLabels), menu->Render()}) | vscroll_indicator | frame;
    }) | CatchEvent(std::bind(&FileExplorer::navigation, this, std::placeholders::_1));

    m_previewComponent = Renderer([this] {
      return previewElement(currentNote().text());
    });

    auto may = Maybe([this] { return m_canPreview && m_shouldPreview; });
    auto window = Container::Horizontal({
      menuComponent,
      Renderer([] { return separator(); }) | may,
      m_previewComponent | may,
    });

    m_component = window | borderRounded | center | CatchEvent([this](ftxui::Event event) -> bool { return basicQuitHandler(event); });
  }

  void FileExplorer::queryFiles()
  {
    namespace fs = std::filesystem;
    m_fileNames.clear();

    // Folders
    for(auto& e : m_folder.folders()) {
      auto path = e.path();
      if(path.string().front() != '.') {
        m_fileNames.push_back(path);
      }
    }

    // Files
    for(auto& e : m_folder.files()) {
      if(fs::is_regular_file(e) && e.extension() == ".md") {
        m_fileNames.push_back(e);
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

    // ftxui::MenuEntryOption entryOption{};
    for(auto& e : m_fileNames) {
      e = fileName(e);
    }
  }

  void FileExplorer::queryLabels()
  {
    using namespace ftxui;
    m_entryLabels.clear();
    for(std::size_t entryIndex{}; entryIndex < m_fileNames.size(); ++entryIndex) {
      m_entryLabels.push_back(entryLabel(getPath(entryIndex)));
    }
  }

  void FileExplorer::updatePreview()
  {
    using namespace ftxui;
    namespace fs = std::filesystem;
    fs::path path;
    m_folder.pathOf(m_fileNames.at(m_menuEntry), &path);
    if(fs::is_regular_file(path)) {
      m_canPreview = true;
    } else {
      m_canPreview = false;
    }
  }

  auto FileExplorer::getPath(std::size_t entryIndex) -> std::filesystem::path
  {
    std::filesystem::path path;
    if(!m_folder.pathOf(m_fileNames.at(entryIndex), &path)) {
      throw std::runtime_error("Failed to identify entry path");
    }
    return path;
  }

  bool FileExplorer::navigation(ftxui::Event event)
  {
    using namespace ftxui;
    if(event == Event::Character('G')) {
      m_menuEntry = m_fileNames.size() - 1;
      m_menuOption.focused_entry = m_menuEntry;
      updatePreview();
      return true;
    }
    if(event == Event::Character('g')) {
      if(m_lastEvent == Event::Character('g')) {
        m_menuEntry = 0;
        m_menuOption.focused_entry = 0;
        m_lastEvent = {};
        updatePreview();
        return true;
      } else {
        m_lastEvent = Event::Character('g');
      }
    }
    if(event == Event::Character('o')) {
      m_controller.openNote(currentNote().name());
      return true;
    }
    if(event == Event::Character('p')) {
      m_shouldPreview = !m_shouldPreview;
    }
    return false;
  }
}
