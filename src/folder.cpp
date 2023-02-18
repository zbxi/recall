#include "folder.hpp"

namespace zbxi::recall
{
  Folder::Folder(std::filesystem::path path) :
    m_path{path}
  {
    namespace fs = std::filesystem;
    std::vector<fs::path> innerFolders{};
    for(auto& e : fs::directory_iterator(path)) {
      if(fs::is_directory(e)) {
        innerFolders.push_back(e);
        continue;
      }
      m_files.push_back(e);
    }

    for(auto& e : innerFolders) {
      m_folders.push_back(Folder{e});
    }
  }

  Folder::~Folder()
  {
  }

  // For files -> no stem: file.png X || file OK
  // For folder -> bar at end: myfolder X || myfolder/ OK
  bool Folder::pathOf(std::string fileName, std::filesystem::path* path) const
  {
    assert(!fileName.empty() && "This function does not accept empty file names");
    assert(path && "This function does not accept nullptr");
    for(auto& e : m_files) {
      if(e.stem() == fileName) {
        *path = e;
        return true;
      }
    }

    for(auto& e : m_folders) {
      if(e.path().stem() == fileName.substr(0, fileName.size() - 1)) {
        *path = e.path();
        return true;
      }
    }

    return false;
  }
}
