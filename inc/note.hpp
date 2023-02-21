#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <set>
#include <span>
#include <sstream>
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
    using TagObserver = std::function<void(std::string_view, std::vector<std::string_view>*)>;
    using time_point = std::int64_t;

  public:
    enum class Label : std::int8_t
    {
      none = 0,
      ignored,
      map_of_content,
      content,
      card,
    };

    Note(std::string const& text,
      std::string name,
      std::filesystem::path filePath,
      time_point modificationDate,
      Label label = Label::none,
      std::vector<std::string> tags = {});

    auto path() const -> std::string const& { return m_filePath; }
    auto name() const -> std::string const& { return m_name; }
    auto text() const -> std::string const& { return m_text; }
    auto label() const -> Label const& { return m_label; }
    auto tags() const -> std::set<std::string> const& { return m_tags; }
    auto headers() const -> std::vector<Header> const& { return m_headers; }
    auto modificationDate() const -> time_point { return m_modificationDate; }

    static auto getLabel(std::string name) -> Note::Label;
    static auto getLabelText(Note::Label label) -> std::string;

    void addTag(std::string tag);
    void setLabel(Label label);

  private:
    void init();
    void parseText(std::string_view text, std::vector<Header>* headers, std::int_fast8_t minHeaderLevel);

    void skipWhitespaces(std::string* line);
    bool checkHeader(std::string_view line, std::int_fast8_t* level);

  private:
    std::string m_text{};
    std::vector<Header> m_headers{};

    // Info
    std::string m_name{};
    std::string m_filePath{};
    time_point m_modificationDate{};
    Label m_label{};
    std::set<std::string> m_tags{};
  };
}
