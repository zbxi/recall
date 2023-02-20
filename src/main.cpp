#include "program.hpp"

void bug()
{
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen();

  std::vector<std::string> entries{};

  // too many entries -> separator does not render
  for(int i{}; i < 180; ++i) {
    entries.push_back("[" + std::to_string(i) + "]");
  }

  int selected{};
  Component menu = Menu(&entries, &selected);

  screen.Loop(Renderer(menu, [menu] {
    return center(border(hbox({
      vbox({
        frame(menu->Render()),
        separator(),           // <-----     here
        text(" Bottom-Left "), // <----- and here
      }),
      separator(),
      text(" Right "),
    })));
  }));
}

int main()
{
  try {
    zbxi::recall::Program{}.run();
  }
  catch(std::runtime_error const& e) {
    std::cout << "[RuntimeError] " << e.what() << std::endl;
  }
}
