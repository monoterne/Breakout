# Breakout

A clone of original 1976 Atari game.

Use **A/D** or the **L/R arrows** to choose a level and move the paddle.

Press **Enter** or **Space** to start or return to the menu.

Maps are created using `.txt` files with 0s and 1s.

## Build and run

From the project root, build the game and launch it once the build succeeds. The script uses `pkg-config` to find raylib 5.5 or newer.

```sh
./build.sh && ./Breakout
```

From inside `src`, run `../build.sh && ../Breakout`.

### macOS

`xcode-select --install`

```sh
brew install raylib pkgconf
```

### Ubuntu

```sh
sudo apt update
sudo apt install build-essential pkg-config libraylib-dev
```

 build raylib once from source:

```sh
raylib_src=$(mktemp -d)
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git "$raylib_src"
cmake -S "$raylib_src" -B "$raylib_src/build" -DBUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=ON
cmake --build "$raylib_src/build" --parallel
sudo cmake --install "$raylib_src/build"
sudo ldconfig
```
