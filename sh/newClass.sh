#/usr/bin/env bash

name=$1
Name="${1^}"

echo -e "#pragma once\n\nnamespace zbxi::recall {\n  class $Name {\n\n  };\n}" > inc/$name.hpp
echo -e "#include \"$name.hpp\"\n\nnamespace zbxi::recall {\n\n}" > src/$name.cpp
