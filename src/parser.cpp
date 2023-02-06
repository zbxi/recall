#include "parser.hpp"

#include <cstdint>
#include <iostream>

namespace zbxi::recall
{
  Note::Note(std::string const& text, time_point modificationDate) :
    m_text{text},
    m_modificationDate{modificationDate}
  {
    init();
  }

  Note::Note(std::string&& text, time_point modificationDate) :
    m_text{std::move(text)},
    m_modificationDate{modificationDate}
  {
    init();
  }

  void Note::init()
  {
    auto constexpr lvl = 1;
    parseText(m_text, &m_headers, lvl);
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
    while(buffer[*index] != '\n') {
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
      std::cout << buffer[endIndex];
      std::cout.flush();
      ++endIndex;
    }
    ++endIndex; // include '\n'
    std::cout << std::endl;

    std::size_t count = (endIndex - *index);
    str->append(&buffer[*index], count);

    // skip to next line
    (*index) += count;
  }

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  auto Parser::parseFiles(std::vector<std::filesystem::path> notePaths) -> std::vector<Note>
  {
    std::vector<Note> notes{};
    // notes.reserve(notePaths.size());
    //
    // Header header{};
    // for(auto& path : notePaths) {
    // }
    return notes;
  }

  auto Parser::parseFile(std::filesystem::path path) -> Note
  {
    std::fstream file{path, file.binary | file.in | file.ate};

    if(!file.is_open()) {
      throw std::runtime_error("Failed to open file: \"" + path.string() + "\"");
    }

    // File size
    std::fstream::pos_type fileSize = file.tellg();
    file.seekg(file.beg);

    // Read
    std::string buffer{};
    buffer.resize(fileSize);
    file.read(&buffer[0], fileSize);

    std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
    std::chrono::system_clock::time_point systemTime = std::chrono::file_clock::to_sys(fileTime);
    std::int64_t seconds = std::chrono::duration_cast<std::chrono::seconds>(systemTime.time_since_epoch()).count();

    // std::chrono::file_clock::time_point time = std::filesystem::last_write_time(path);
    // time.time_since_epoch();
    // std::chrono::steady_clock::time_point::time_since_epoch();

    return Note{std::move(buffer), systemTime};
  }
}
