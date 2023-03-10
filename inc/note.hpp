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

    template<typename T>
    using Timepoint = std::chrono::time_point<std::chrono::system_clock, T>;
    using Duration = std::chrono::seconds;

  public:
    enum class Label : std::int8_t
    {
      none = 0,
      ignored,
      map_of_content,
      content,
      card,
    };

    Note(
      std::filesystem::path filePath,
      std::string const& text,
      std::string name,
      Label label,
      Timepoint<std::chrono::milliseconds> modificationDate,
      Timepoint<std::chrono::seconds> recalledDate,
      Duration intervalDuration,
      double inEaseModifer,
      std::vector<std::string> tags);

    auto path() const -> std::string const& { return m_filePath; }
    auto name() const -> std::string const& { return m_name; }
    auto text() const -> std::string const& { return m_text; }
    auto label() const -> Label const& { return m_label; }
    auto tags() const -> std::set<std::string> const& { return m_tags; }
    auto headers() const -> std::vector<Header> const& { return m_headers; }
    auto modificationDate() const { return m_modificationDate; }
    auto recallDate() const { return m_recallDate; }

    auto easeModifier() const -> double { return m_easeModifier; }
    auto intervalDuration() const { return m_intervalDuration; }

    static auto getLabel(std::string name) -> Note::Label;
    static auto getLabelText(Note::Label label) -> std::string;

    static auto minimalEase() -> double { return 1.3; }
    static auto maximalEase() -> double { return 2.5; }

    void addTag(std::string tag);
    void setLabel(Label label);
    void incrementRecallDate(std::chrono::seconds duration);
    void setEaseModifier(double value);

  private:
    void parseText(std::string_view text, std::vector<Header>* headers, std::int_fast8_t minHeaderLevel);

    void skipWhitespaces(std::string* line);
    bool checkHeader(std::string_view line, std::int_fast8_t* level);

  private:
    std::string m_filePath{};
    std::string m_text{};
    std::string m_name{};
    Label m_label{};
    Timepoint<std::chrono::milliseconds> m_modificationDate{};
    Timepoint<std::chrono::seconds> m_recallDate{};
    Duration m_intervalDuration{};
    double m_easeModifier{};
    std::set<std::string> m_tags{};

    // Deprecated
    std::vector<Header> m_headers{};
  };
}
