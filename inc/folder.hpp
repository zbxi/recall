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

    auto files() const -> std::vector<std::filesystem::path> const& { return m_files; }
    auto folders() const -> std::vector<Folder> const& { return m_folders; }
    auto path() const -> std::filesystem::path { return m_path; }
    bool pathOf(std::string fileName, std::filesystem::path*) const;
    bool relativePathOf(std::filesystem::path const&, std::filesystem::path*);
    bool fullPathOf(std::filesystem::path const&, std::filesystem::path*);

  private:
    std::vector<Folder> m_folders{};
    std::vector<std::filesystem::path> m_files{};

    std::filesystem::path m_path{};
  };
}
