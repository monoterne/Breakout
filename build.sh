#!/bin/sh
set -eu

cd "$(dirname "$0")"
if [ "$(uname -s)" = Darwin ]; then
    export PATH="$PATH:/opt/homebrew/bin:/usr/local/bin"
fi

raylib_flags=$("${PKG_CONFIG:-pkg-config}" --cflags --libs 'raylib >= 5.5')

printf 'Building Breakout...\n'
# xargs keeps quoted paths from pkg-config together as compiler arguments.
printf '%s\n' "${CPPFLAGS:-}" "${CXXFLAGS:-}" '-std=c++11 src/main.cpp' \
    "${LDFLAGS:-}" "$raylib_flags" "${LDLIBS:-}" |
    xargs "${CXX:-c++}" -Wall -Wextra -o Breakout
printf 'Built %s/Breakout\n' "$PWD"
