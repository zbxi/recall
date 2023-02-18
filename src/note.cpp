#include "note.hpp"

namespace zbxi::recall
{
  Note::Note(std::string const& text, time_point modificationDate, TagObserver tagObserver, Label label, std::vector<std::string_view> tags) :
    m_text{text},
    m_modificationDate{modificationDate},
    m_label{label},
    m_tags{tags},
    m_newTagObserver{tagObserver}
  {
    init();
  }

  Note::Note(std::string&& text, time_point modificationDate, TagObserver tagObserver, Label label, std::vector<std::string_view> tags) :
    m_text{std::move(text)},
    m_modificationDate{modificationDate},
    m_label{label},
    m_tags{tags},
    m_newTagObserver{tagObserver}
  {
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

  void Note::parseText(std::string_view text, std::vector<Header>* headers, std::int_fast8_t headerLevel)
  {
    std::size_t textIndex{};
    std::size_t firstIndex{};
    std::int_fast8_t lineLevel{};
    std::vector<std::size_t> headersChildren{0};

    auto print = [&]() {
      std::cout << "+++++++++++++" << std::endl;
      std::cout << headers->back().text;
      std::cout << "+++++++++++++" << std::endl;
    };

    auto printChildrenCount = [&]() {
      std::cout << "<= ";

      for(auto& e : headersChildren) {
        std::cout << e << " ";
      }

      std::cout << ">=" << std::endl;
    };

    static_cast<void>(print);
    static_cast<void>(printChildrenCount);

    std::istringstream stream{std::string(text)};
    std::string line{};

    while(std::getline(stream, line)) {
    }

    while(textIndex < text.size()) {
      if(checkHeader(text, textIndex, &lineLevel)) {
        if(lineLevel > headerLevel) {
          ++headersChildren.back();
        }
        if(lineLevel == headerLevel) {
          headers->push_back({
            .level = lineLevel,
            .text{text.data() + firstIndex, (textIndex - firstIndex)},
          });
          print();
          headersChildren.push_back(0);
          firstIndex = textIndex;
          nextLine(text, &textIndex);
          continue;
        }
      }

      nextLine(text, &textIndex);
    }

    headers->push_back({
      .level = lineLevel,
      .text{text.data() + firstIndex, (textIndex - firstIndex)},
    });

    print();
    printChildrenCount();

    for(std::size_t i{}; i < headersChildren.size(); ++i) {
      if(!headersChildren[i])
        continue;

      auto& header = (*headers)[i];
      parseText(header.text, &(header.children), lineLevel + 1);
    }
  }

  void Note::nextLine(std::string_view buffer, std::size_t* index)
  {
    while(*index < buffer.size() && buffer[*index] != '\n') {
      ++(*index);
    }
    ++(*index);
  }

  void Note::skipWhitespaces(std::string const& buffer, std::size_t* index)
  {
    while(std::isspace(buffer[*index])) {
      ++(*index);
    }
  }

  bool Note::checkHeader(std::string_view buffer, std::size_t index, std::int_fast8_t* level)
  {
    if(buffer[index] != '#') {
      return false;
    }

    *level = 0;
    while(buffer[index] == '#') {
      ++(*level);
      ++index;
    }

    return true;
  }

  void Note::appendLine(std::string_view buffer, std::size_t* index, std::string* str)
  {
    std::size_t endIndex{*index};

    // get count till end of the line
    while(endIndex < buffer.size() && buffer[endIndex] != '\n') {
      // std::cout << buffer[endIndex];
      // std::cout.flush();
      ++endIndex;
    }
    ++endIndex; // include '\n'
    // std::cout << std::endl;

    std::size_t count = (endIndex - *index);
    str->append(&buffer[*index], count);

    // skip to next line
    (*index) += count;
  }
}
