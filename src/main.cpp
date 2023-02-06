#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
//
#include "ftxui/component/captured_mouse.hpp"     // ftxui
#include "ftxui/component/component.hpp"          // Menu
#include "ftxui/component/component_options.hpp"  // MenuOption
#include "ftxui/component/screen_interactive.hpp" // ScreenInteractive
#include "ftxui/dom/elements.hpp"
//
#include "parser.hpp"
//


// void something()
// {
//   using namespace ftxui;
//
//   Element element = text("hello world") | center;
//   Element theBorder = border(element);
//
//   Screen screen = Screen::Create(Dimension::Full(), Dimension::Fit(theBorder));
//
//   Render(screen, theBorder);
//   screen.Print();
// }

void print(std::span<zbxi::recall::Header> headers)
{
  auto printChildren = [](std::span<zbxi::recall::Header> headers) {
    auto constexpr separator = "----------------------------->";
    for(auto& h : headers) {
      std::cout << "<--- level " << (int)h.level << " " << separator << std::endl;
      std::cout << h.text;
      std::cout << "<----------------------------------------->" << std::endl;
    }
  };

  printChildren(headers);
  for(auto& h : headers) {
    printChildren(h.children);
  }
}

int main()
{
  using namespace zbxi;
  std::filesystem::path path{"/tmp/Projects.md"};
  recall::Parser p;
  recall::Note note = p.parseFile(path);

  auto headers = note.headers();
  print(headers);
  return 0;

  auto line = "==>==>==>==>==>==>==>==>==>==>==>==>==>";
  std::cout << line << '\n'
            << note.text()
            << line << std::endl;
}

// void menu()
// {
//   using namespace ftxui;
//
//   ScreenInteractive screen = ScreenInteractive::TerminalOutput();
//
//   int selected{0};
//   std::vector<std::string> entries = {
//     "entry 1",
//     "entry 2",
//     "entry 3",
//   };
//
//   MenuOption option{
//     .on_enter = screen.ExitLoopClosure()};
//   auto menu = Menu(&entries, &selected, &option);
//
//   screen.Loop(menu);
//
//   std::cout << "Selected element = " << selected << std::endl;
// }
//
// void simple()
// {
//   using namespace ftxui;
//   Element document = border(hbox({text("left"), separator(), text("right")}));
//
//   Screen screen = Screen::Create(
//     Dimension::Full(),       // Width
//     Dimension::Fit(document) // Height
//   );
//
//   Render(screen, document);
//   screen.Print();
// }
