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

  auto Presenter::explorerEntries() -> std::vector<std::string>&
  {
    if(m_notekeeper->connected() && !m_explorerQueried) {
      queryFolderFiles(m_currentPath);
      m_explorerQueried = true;
    }
    return m_explorerEntries;
  }

  void Presenter::queryFolderFiles(std::filesystem::path path)
  {
    m_explorerEntries.clear();
    for(auto& e : std::filesystem::directory_iterator(path)) {
      m_explorerEntries.push_back(e.path());
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
