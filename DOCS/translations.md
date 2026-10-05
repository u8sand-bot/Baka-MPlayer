## Translations

Translations live in `src/translations/` and are compiled into the binary at build time.
The commands below assume you have already configured a build directory (`cmake -B build`).

### To add a new translation

1. Update your local repo to make sure everything is up to date.
2. Create your language's file with `lupdate src -no-obsolete -locations none -ts src/translations/baka-mplayer_{lang-code}.ts`.
3. Re-run `cmake -B build` so the new file is picked up.
4. Open the `.ts` file with Qt Linguist and proceed to translate into your language.
5. Upon completion of the translation, run `cmake --build build --target update_translations`. This will trim the .ts file to the minimum required information for release.

### To update an existing translation

1. Update your local repo to make sure everything is up to date.
2. Run `cmake --build build --target update_translations` to regenerate the `.ts` files.
3. Make your changes with Qt Linguist.

If you want to submit a translation, you can create a git pull request.

For more information on Qt Linguist (the program used to translate Qt projects) see https://doc.qt.io/qt-6/qtlinguist-index.html
