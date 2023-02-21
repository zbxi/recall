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

  bool Folder::relativePathOf(std::filesystem::path const& fullPath, std::filesystem::path* inout)
  {
    assert(!fullPath.empty() && "This function does not accept empty paths");
    assert(inout && "This function does not accept nullptr");

    std::string path = fullPath.string();
    std::string const& currentPath = m_path;

    for(std::size_t i{}; i < currentPath.length(); ++i) {
      if(path.at(i) != currentPath.at(i)) {
        return false;
      }
    }

    path.erase(0, currentPath.length() + 1);
    *inout = path;

    return true;
  }

  bool Folder::fullPathOf(std::filesystem::path const& relativePath, std::filesystem::path* inout)
  {
    assert(!relativePath.empty() && "This function does not accept empty paths");
    assert(inout && "This function does not accept nullptr");

    std::filesystem::path path = m_path;
    path.append(relativePath.string());
    if(!std::filesystem::exists(path)) {
      return false;
    }

    *inout = path;
    return true;
  }
}
