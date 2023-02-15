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
}
