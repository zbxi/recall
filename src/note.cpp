#include "note.hpp"

namespace zbxi::recall
{
  Note::Note(std::string const& text,
    std::filesystem::path filePath,
    time_point modificationDate,
    TagObserver tagObserver,
    Label label,
    std::vector<std::string_view> tags) :
    m_text{text},
    m_filePath{filePath},
    m_modificationDate{modificationDate},
    m_label{label},
    m_tags{tags},
    m_newTagObserver{tagObserver}
  {
    assert(!m_filePath.empty());
    init();
  }

  Note::Note(std::string&& text,
    std::filesystem::path filePath,
    time_point modificationDate,
    TagObserver tagObserver,
    Label label,
    std::vector<std::string_view> tags) :
    m_text{std::move(text)},
    m_filePath{filePath},
    m_modificationDate{modificationDate},
    m_label{label},
    m_tags{tags},
    m_newTagObserver{tagObserver}
  {
    assert(!m_filePath.empty());
    init();
  }

  void Note::init()
  {
    auto constexpr lvl = 1;
    parseText(m_text, &m_headers, lvl);
  }

  void Note::newTag(std::string_view tag)
  {
    if(!m_newTagObserver) {
      throw std::runtime_error("method call on null object");
    }

    std::span<std::string> updatedTags{};
    if(m_newTagObserver(tag, &updatedTags)) {
      m_tags.clear();
      for(auto& e : updatedTags) {
        m_tags.push_back(e);
      }
    }
  }

  void Note::parseText(std::string_view text, std::vector<Header>* headers, std::int_fast8_t minHeaderLevel)
  {
    assert(headers && "nullptr");

    auto print = [&]() {
      std::cout << "+++++++++++++" << std::endl;
      std::cout << headers->back().text;
      std::cout << "+++++++++++++" << std::endl;
    };

    static_cast<void>(print);

    std::istringstream countStream{std::string(text)};
    std::string line{};
    std::int_fast8_t lineLevel{};
    auto& max = std::numeric_limits<std::int_fast8_t>::max;
    std::int_fast8_t headerLevel = max();

    while(std::getline(countStream, line)) {
      skipWhitespaces(&line);
      if(checkHeader(line, &lineLevel)) {
        if(lineLevel >= minHeaderLevel) {
          headerLevel = std::min(headerLevel, lineLevel);
        }
      }
    }

    if(headerLevel == max()) {
      return;
    }

    std::istringstream stream{std::string(text)};
    std::size_t firstIndex{};
    std::int64_t endIndex{-1};

    while(std::getline(stream, line)) {
      skipWhitespaces(&line);
      if(checkHeader(line, &lineLevel)) {
        if(lineLevel == headerLevel) {
          headers->push_back({
            .level = lineLevel,
            .text{text.data() + firstIndex, ((endIndex + 1) - firstIndex)},
          });
          // print();
          // headerInfos.push_back({});
          firstIndex = endIndex + 1;
        }
      }
      endIndex += line.length() + 1;
    }

    headers->push_back({
      .level = lineLevel,
      .text{text.data() + firstIndex, ((endIndex + 1) - firstIndex)},
    });

    // print();
    // printChildrenCount();

    // std::cout << "\n\n";
    for(auto& childHeader : (*headers)) {
      parseText(childHeader.text, &childHeader.children, headerLevel + 1);
    }
  }

  void Note::skipWhitespaces(std::string* line)
  {
    std::size_t i{};
    while(i < line->size()) {
      if((*line)[i] != ' ') {
        break;
      }
      ++i;
    }

    (*line) = line->substr(i, line->length() - i);
  }

  bool Note::checkHeader(std::string_view line, std::int_fast8_t* level)
  {
    if(line.front() != '#') {
      return false;
    }

    *level = 0;
    std::size_t i{};
    while(line.at(i) == '#') {
      ++(*level);
      ++i;
    }

    return true;
  }
}
