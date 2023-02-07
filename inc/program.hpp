#pragma once

#include "note.hpp"
#include "parser.hpp"
//
#include <filesystem>
#include <span>
//
#include <sqlite3.h>
//
#include "ftxui/component/captured_mouse.hpp"     // ftxui
#include "ftxui/component/component.hpp"          // Menu
#include "ftxui/component/component_options.hpp"  // MenuOption
#include "ftxui/component/screen_interactive.hpp" // ScreenInteractive
#include "ftxui/dom/elements.hpp"


namespace zbxi::recall
{
  class Program
  {
  public:
    Program();
    void run();

  private:
    void printHeaders(std::span<zbxi::recall::Header> headers);
    void printTime(zbxi::recall::Note& note);
    void printTags(Note& note);

  private:
  };
}

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
//
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
//
