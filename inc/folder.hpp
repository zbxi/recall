#pragma once

#include <cassert>
#include <filesystem>
#include <span>
#include <vector>

namespace zbxi::recall
{
  class Folder
  {
  public:
    Folder(std::filesystem::path);
    ~Folder();

    auto files() const -> std::vector<std::filesystem::path> { return m_files; }
    auto folders() const -> std::vector<Folder> { return m_folders; }
    auto path() const -> std::filesystem::path { return m_path; }
    bool pathOf(std::string fileName, std::filesystem::path*) const;

  private:
    std::vector<Folder> m_folders{};
    std::vector<std::filesystem::path> m_files{};

    std::filesystem::path m_path{};
  };
}
