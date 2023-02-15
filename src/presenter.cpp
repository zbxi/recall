#include "presenter.hpp"

namespace zbxi::recall
{

  Presenter::Presenter(Notekeeper* notekeeper, Configuration* configuration) :
    m_notekeeper{notekeeper},
    m_configuration{configuration}
  {
    m_userInputStrings.resize(1);
  }

  Presenter::~Presenter()
  {
  }

  auto Presenter::menuExplorerEntries() -> std::vector<std::string>&
  {
    if(m_notekeeper->connected() && !m_explorerQueried) {
      queryFolderFiles(m_currentPath);
      m_explorerQueried = true;
    }
    return m_explorerMenuEntries;
  }

  void Presenter::queryFolderFiles(std::filesystem::path folderPath)
  {
    m_explorerMenuEntries.clear();
    m_explorerEntries.clear();

    for(auto& e : std::filesystem::directory_iterator(folderPath)) {
      if(e.is_directory() && e.path().string().front() != '.') {
        m_explorerEntries.push_front(e);
      } else if(e.is_regular_file() && e.path().extension() == ".md") {
        m_explorerEntries.push_back(e);
      }
    }

    auto fileName = [this](std::filesystem::path e) -> std::string {
      std::size_t pathLength = m_notekeeper->vaultPath().length() + 1; // +1 for the '/'
      std::string entryStr = e.string();
      std::string path = std::string{entryStr.begin() + pathLength, entryStr.end()};

      if(std::filesystem::is_regular_file(e)) {
        assert(e.extension() == ".md");
        path.resize(path.length() - 3); // -3 for the '.md'
      } else if(std::filesystem::is_directory(e)) {
        path.append("/");
      }

      return path;
    };

    for(auto& e : m_explorerEntries) {
      m_explorerMenuEntries.push_back(fileName(e));
    }
  }

  void Presenter::setCurrentPath(std::filesystem::path path)
  {
    std::string fullPath = m_notekeeper->vaultPath() + path.string();
    if(!std::filesystem::is_directory(fullPath)) {
      throw std::runtime_error("Invalid path");
    }

    m_currentPath = fullPath;
  }
}
