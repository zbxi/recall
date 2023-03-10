#include "note.hpp"

namespace zbxi::recall
{

  Note::Note(
    std::filesystem::path filePath,
    std::string const& text,
    std::string name,
    Label label,
    Timepoint<std::chrono::milliseconds> modificationDate,
    Timepoint<std::chrono::seconds> recalledDate,
    Duration intervalDuration,
    double inEaseModifier,
    std::vector<std::string> tags) :
    m_filePath{filePath},
    m_text{text},
    m_name{name},
    m_label{label},
    m_modificationDate{modificationDate},
    m_recallDate{recalledDate},
    m_intervalDuration{intervalDuration},
    m_easeModifier{inEaseModifier},
    m_tags{tags.begin(), tags.end()}
  {
    assert(!name.empty());
    assert(!m_filePath.empty());
    assert(modificationDate.time_since_epoch().count() != 0);
    assert(m_easeModifier >= Note::minimalEase() && m_easeModifier <= Note::maximalEase());
    // auto constexpr lvl = 1;
    // parseText(m_text, &m_headers, lvl);
  }

  void Note::addTag(std::string tag)
  {
    m_tags.insert(tag);
  }

  void Note::setLabel(Label label)
  {
    m_label = label;
  }

  void Note::incrementRecallDate(std::chrono::seconds duration)
  {
    m_intervalDuration = duration;
    m_recallDate += duration;
  }

  void Note::setEaseModifier(double value)
  {
    value = std::clamp(value, Note::minimalEase(), Note::maximalEase());
    m_easeModifier = value;
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

  auto Note::getLabel(std::string name) -> Note::Label
  {
    using enum Note::Label;
    if(name == "None") {
      return none;
    } else if(name == "Ignored") {
      return ignored;
    } else if(name == "MOC") {
      return map_of_content;
    } else if(name == "Content") {
      return content;
    } else if(name == "Card") {
      return card;
    } else {
      throw std::runtime_error("Failed to identify label");
    }
  }

  auto Note::getLabelText(Note::Label label) -> std::string
  {
    using enum Note::Label;
    if(label == none) {
      return "None";
    } else if(label == ignored) {
      return "Ignored";
    } else if(label == map_of_content) {
      return "MOC";
    } else if(label == content) {
      return "Content";
    } else if(label == card) {
      return "Card";
    } else {
      throw std::runtime_error("Failed to identify label");
    }
  }
}
