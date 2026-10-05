# Changelog

## 2.1.0

A modernization release: Baka MPlayer builds and runs again on current Windows, macOS and Linux.

* New: Qt 6 and CMake build; official builds for Windows (x86_64) and macOS (Apple Silicon)
* New: Native Wayland support on Linux; video is rendered through libmpv's OpenGL render API on macOS and Linux
* New: Automatic update checks on all platforms (weekly, can be disabled with the `autoUpdate` setting); on Windows updates download and install themselves
* New: The Windows build is portable and self-contained (Qt linked statically, bundled libmpv and yt-dlp)
* New: yt-dlp is found even when it isn't on the PATH (e.g. Homebrew on macOS), and bundled with the Windows and macOS builds
* New: On Windows, log output goes to the console the player was started from, or to `baka-mplayer.log`
* Changed: Motion interpolation uses mpv's `interpolation` option (old `vo` settings are migrated)
* Changed: Update checks use GitHub releases
* Removed: Windows taskbar thumbnail buttons (no longer available in Qt 6)
* Removed: 32-bit Windows builds
* Fixed: "Always on top" on macOS
* Fixed: The update check never ran because its date was not saved
