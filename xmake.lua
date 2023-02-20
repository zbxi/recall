set_toolchains("clang")
set_languages("c++20")
set_defaultmode("debug")
set_policy("build.warning", true)

add_rules("mode.release", "mode.debug")

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

package("ftxui")
do
  add_deps("cmake")
  set_sourcedir(path.join(os.scriptdir(), "dep/FTXUI-4.0.0"))
  on_install(function(package)
    local configs = {}
    table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
    table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
    import("package.tools.cmake").install(package, configs)
  end)
  on_test(function(package)
    -- assert(package:has_cfuncs("add", {includes = "foo.h"}))
  end)
  package_end()
end

add_requires("ftxui", "sqlite3")

target("program")
do
  set_default(true)
  set_kind("binary")
  add_files("src/**.cpp")
  add_packages("sqlite3", "ftxui")
  -- add_linkdirs("dep/ftxui-4.0.0-Linux/lib")
  -- add_links(":libftxui-component.a", ":libftxui-dom.a", ":libftxui-screen.a")
  add_includedirs("inc" --[[ , "dep/ftxui-4.0.0-Linux/include" ]])
end
