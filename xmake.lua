set_toolchains("clang")
set_languages("c++20")
set_defaultmode("debug")
set_policy("build.warning", true)

add_rules("mode.release", "mode.debug")
add_requires(--[[ "conan::ftxui", { alias = "ftxui" }, ]] "sqlite")

if is_mode("debug") then
  local sanitize = { "-fsanitize=undefined", "-fsanitize=leak" }
  local warnings = {
    "-Wall",
    "-Wextra", --[[ "-Weffc++", ]]
    "-Wshadow",
  }
  -- "-Wfatal-errors"

  add_cxxflags(warnings, sanitize, "-pedantic-errors")
  add_ldflags(sanitize, "-lubsan")
end

target("program")
set_default(true)
set_kind("binary")
add_files("src/**.cpp")
add_packages("sqlite")
add_linkdirs("dep/ftxui-4.0.0-Linux/lib")
add_links(":libftxui-component.a", ":libftxui-dom.a", ":libftxui-screen.a")
add_includedirs("inc", "dep/ftxui-4.0.0-Linux/include")
