# [Baka MPlayer](https://u8sand.github.io/Baka-MPlayer/)

![](https://github.com/u8sand/Baka-MPlayer/raw/master/website/screenshots/img-2.png "Main Screenshot")

[![Build](https://github.com/u8sand/Baka-MPlayer/actions/workflows/build.yml/badge.svg)](https://github.com/u8sand/Baka-MPlayer/actions/workflows/build.yml)


## Overview

Baka MPlayer is a free and open source, cross-platform, **libmpv** based multimedia player.
Its simple design reflects the idea for an uncluttered, simple, and enjoyable environment for watching tv shows.


## Requirements

* A C++17 compiler (gcc, clang, or MSVC/MinGW)
* CMake (>= 3.16)
* pkg-config
* libmpv (mpv >= 0.33, with the OpenGL render API)
* Qt 6 (>= 6.2): Core, Gui, Widgets, Network, Svg, OpenGL, OpenGLWidgets, LinguistTools
* libX11 (Linux, optional: enables "always on top" and "dim lights" on X11)
* yt-dlp (optional, for streaming online videos; the prebuilt Windows and macOS builds bundle a copy, but an installed one takes precedence)

### Get the font

Baka MPlayer was designed around the font called Noto Sans. Noto Sans was used because of its open source nature and its broad support for Unicode characters. Having the correct font installed insures that what you see is what was intended.

[Get it here.](https://fonts.google.com/noto/specimen/Noto+Sans)


## Compilation

All platforms use the same CMake build:
```
git clone https://github.com/u8sand/Baka-MPlayer.git
cd Baka-MPlayer
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
Translations are compiled and embedded into the binary automatically.

Build options:

| Option | Default | Description |
| --- | --- | --- |
| `BAKA_SETTINGS_FILE` | `bakamplayer` | Settings file name (without `.ini`) |
| `BAKA_LANG` | *(locale)* | Force a language code instead of using the system locale |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Installation prefix |

### Linux

Install the dependencies. On Debian/Ubuntu (24.04 or newer):
```
sudo apt install cmake g++ pkgconf libmpv-dev libx11-dev libgl-dev \
    qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools
```
On Fedora:
```
sudo dnf install cmake gcc-c++ pkgconf mpv-libs-devel libX11-devel \
    qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```
On Arch Linux:
```
sudo pacman -S cmake mpv qt6-base qt6-svg qt6-tools libx11
```
Then build as above and install:
```
sudo cmake --install build
```
Both X11 and Wayland are supported. The configuration file will be created on first run and will be written to `~/.config/bakamplayer.ini`.

### Windows

Builds use [MSYS2](https://www.msys2.org/) with a static Qt, and the self-contained libmpv from
[mpv-winbuild](https://github.com/shinchiro/mpv-winbuild-cmake/releases) (the `mpv-dev-x86_64-*.7z` archive).
From a UCRT64 shell, with the archive extracted to `mpv-dev`:
```
pacman -S mingw-w64-ucrt-x86_64-{toolchain,cmake,ninja,qt6-static}
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/ucrt64/qt6-static \
    -DMPV_INCLUDE_DIR="$PWD/mpv-dev/include" -DMPV_LIBRARY="$PWD/mpv-dev/libmpv.dll.a" \
    -DCMAKE_EXE_LINKER_FLAGS=-static
cmake --build build
```
Put `build/baka-mplayer.exe` next to `mpv-dev/libmpv-2.dll` (and optionally `yt-dlp.exe`) and it is ready to run. The configuration file is written next to the executable.

### macOS

Install the dependencies with [Homebrew](https://brew.sh/):
```
brew install cmake pkgconf qt mpv
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build
"$(brew --prefix qt)/bin/macdeployqt" build/Baka-MPlayer.app
```
Then drag `build/Baka-MPlayer.app` to your Applications folder.

The prebuilt macOS app is ad-hoc signed but not notarized, so macOS will block it the first time.
After copying it to Applications, either right-click it and choose **Open**, or run:
```
xattr -dr com.apple.quarantine /Applications/Baka-MPlayer.app
```

### Prebuilt binaries

Every push is built for Linux, Windows and macOS by [GitHub Actions](.github/workflows/build.yml); the binaries are attached to each run as artifacts.

### Other languages

You can check out which languages we currently support in `src/translations/`. See [DOCS/translations.md](DOCS/translations.md) to contribute one.


## Bug reports

Please use the [issues tracker](https://github.com/u8sand/Baka-MPlayer/issues) provided by GitHub to send us bug reports or feature requests.

