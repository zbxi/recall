#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace zbxi::recall
{
  constexpr std::int32_t blockSize{8 * 1024}; // 8KB

  struct Header
  {
    std::int_fast8_t level{0};
    std::string_view text{};
    std::vector<Header> children{};
  };

  class Note
  {
    using time_point = std::chrono::system_clock::time_point;

  public:
    enum class Label : std::int8_t
    {
      ignored = -1,
      none = 0,
      map_of_content = 1,
      content = 2,
      card = 3,
    };

    Note(std::string const& text, time_point modificationDate);
    Note(std::string&& text, time_point modificationDate);

    auto text() -> std::string_view { return m_text; }
    auto headers() -> std::span<Header> { return m_headers; }
    auto modificationDate() -> time_point { return m_modificationDate; }

  private:
    void init();
    void parseText(std::string_view text, std::vector<Header>* headers, std::int_fast8_t lineLevel);

    void nextLine(std::string_view buffer, std::size_t* index);
    void skipWhitespaces(std::string const& buffer, std::size_t* index);
    bool checkHeader(std::string_view buffer, std::size_t index, std::int_fast8_t* level);
    void appendLine(std::string_view buffer, std::size_t* index, std::string* str);

  private:
    std::string m_text{};
    std::vector<Header> m_headers{};
    time_point m_modificationDate{};
  };

  // Label label{Label::none};
  // std::vector<std::string> tags{};

  class Parser
  {
  public:
    auto parseFiles(std::vector<std::filesystem::path> notePaths) -> std::vector<Note>;
    auto parseFile(std::filesystem::path path) -> Note;

  private:
  };
}
