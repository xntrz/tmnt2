# TMNT2
Reverse engineering PC version of TMNT2: Battle Nexus (2004) based on dwarf pdb from leaked PS2 prototype [image](https://archive.org/details/TeenageMutantNinjaTurtles2BattleNexusJun142004prototype).\
If you encountered problem with running please open an issue and attach all needed information: at least info about your game image version and if possible screenshot with an error or assertion.

<p float="left">
<img src="scr1.png" width="250"/>
<!-- <img src="scr2.png" width="250"/> -->
<img src="scr3.png" width="250"/>
<img src="scr4.png" width="250"/>
</p>

# Current project status:
✔️ Basic gameplay\
✔️ Sound\
❌ Multilanguage support (known as Europe(En,Fr,De,Es,It) version - [issue](https://github.com/xntrz/tmnt2/issues/1#issuecomment-2643548662))\
🔨 Enemies & Bosses (33/76)
&nbsp;

# Building & Run
Currently building available only via premake5 & visual studio.\
This is simplified instruction, more detailed process you can see [here](https://github.com/xntrz/tmnt2/issues/1#issuecomment-2637289859).

**Requirements**
- Assets files from **PC version** of the game
- MS Visual Studio with fully support c++11 (Visual Studio 2015 or higher)
- MS DirectX 9.0 SDK with `DXSDK_DIR` environment variable is set

**Build**
- [generate](https://premake.github.io/docs/Using-Premake/#using-premake-to-generate-project-files) `.sln` file by [premake5](https://premake.github.io/)
- Open `.sln` file in `build/`
- Select Release or Debug build
- Hit `Build solution`

**Run**
- Place executable to assets folder or specify path to them as **run arg**
- Run (example of running from output bin dir with run args that listed below: **"TMNT2.exe -afspath=C:/tmnt2 -wnd"**)

**Run args**
- `-afspath=%PATH%` - specifies location of game assets files
- `-afsover` - allows to override afs files if exist at the same path
- `-wnd` - windowed mode
- `-noeval` - removes RW evaluation logo at left-down corner

**enemy_test.zip**\
To use enemy testbed extract folder from archive to the game assets directory.

&nbsp;

# 3rd party solutions used
* RenderWare v3.7 - [rw37sdk](https://archive.org/details/RenderwareStudio3.7SDKForWindows)
* Dwarf explorer - [dwex](https://github.com/sevaa/dwex)
* CRIWARE SDK - [crisdk](https://archive.org/details/cri-sdk)
* toon plugin - [rptoon](https://github.com/xntrz/rptoon)
