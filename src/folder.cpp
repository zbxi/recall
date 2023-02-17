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

  bool Folder::pathOf(std::string fileName, std::filesystem::path* path)
  {
    assert(path && "This function does not accept nullptr");
    for(auto& e : m_files) {
      if(e.stem() == fileName) {
        *path = e;
        return true;
      }
    }

    return false;
  }
}
