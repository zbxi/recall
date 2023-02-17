#include "presenter.hpp"

#include "locator.hpp"

namespace zbxi::recall
{

  Presenter::Presenter(Locator& locator) :
    m_locator{locator}
  {
  }

  Presenter::~Presenter()
  {
  }

  auto Presenter::notekeeper() const -> Notekeeper const&
  {
    return m_locator.notekeeper();
  }

  auto Presenter::configuration() const -> Configuration const&
  {
    return m_locator.configuration();
  }

  // auto Presenter::menuExplorerEntries() -> std::vector<std::string>&
  // {
  //   if(!m_explorerQueried) {
  //     queryFolderFiles(m_currentPath);
  //     m_explorerQueried = true;
  //   }
  //   return m_explorerMenuEntries;
  // }

  // void Presenter::setCurrentPath(std::filesystem::path path)
  // {
  //   std::string fullPath = m_locator.notekeeper().vaultPath() + path.string();
  //   if(!std::filesystem::is_directory(fullPath)) {
  //     throw std::runtime_error("Invalid path");
  //   }
  //
  //   m_currentPath = fullPath;
  // }
}
