#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <span>
#include <string_view>
#include <vector>
//
#include <iostream>

namespace zbxi::recall
{
  struct Header
  {
    std::int_fast8_t level{0};
    std::string_view text{};
    std::vector<Header> children{};
  };

  class Note
  {
    using TagObserver = std::function<bool(std::string_view, std::span<std::string>*)>;
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

    Note(std::string const& text, time_point modificationDate, TagObserver tagObserver, Label label = Label::none, std::vector<std::string_view> tags = {});
    Note(std::string&& text, time_point modificationDate, TagObserver tagObserver, Label label = Label::none, std::vector<std::string_view> tags = {});

    auto path() -> std::string_view { return m_filePath; }
    auto text() -> std::string_view { return m_text; }
    auto label() -> Label& { return m_label; }
    auto tags() -> std::span<std::string_view> { return m_tags; }
    auto headers() -> std::span<Header> { return m_headers; }
    auto modificationDate() -> time_point { return m_modificationDate; }

    void newTag(std::string_view tag);

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

    // Info
    std::string m_filePath{};
    time_point m_modificationDate{};
    Label m_label{};
    std::vector<std::string_view> m_tags{};

    // Observers
    TagObserver m_newTagObserver;
  };
}
