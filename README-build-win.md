> # Building & Run: Windows

Currently building for Windows available only for x86 and via Visual Studio.

**Requirements**
- Assets files from **PC version** of the game
- [CMake](https://cmake.org/download/) 3.26.0 or higher (make sure add to `PATH`)
- MS Visual Studio with fully support c++11 (Visual Studio 2015 or higher)
- MS DirectX 9.0 SDK with `DXSDK_DIR` environment variable is set

**Build**
- Navigate to root dir of the project
- Where `vs2022` your Visual Studio version:
    - for Visual Studio 2015 is `vs2015`
    - for Visual Studio 2017 is `vs2017`
    - and so on... (see [CMakePresets](CMakePresets.json) for details)
- Generate `.sln` file: `cmake --preset vs2022 -DOPT_EU_BUILD=OFF`
    - `OPT_EU_BUILD` set to ON or OFF depending on your game assets type: EU or NA (how to know see [Game Region](README-game-region.md))
- Then call build on it: `cmake --build --preset vs2022-release`

**Run**
- Place executable to assets folder or specify path to them as **run arg**
- Run

**Run args**
- `-afspath=%PATH%` - specifies location of game assets files
- `-wnd` - windowed mode
- `-noeval` - removes RW evaluation logo at left-down corner
- `-lang=%LOCALE%` - forcefully sets language (available only for EU build, value to set: `en`, `fr`, `de`, `es`, `it`)
