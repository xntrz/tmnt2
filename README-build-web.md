> # Building & Run: Web

Building for Web perfoming via [Emscripten SDK](https://emscripten.org/index.html).     
Currently this is mostly Windows related guide.

**Requirements**
- Assets files from **PC version** of the game
- [CMake](https://cmake.org/download/) 3.26.0 or higher
- Installed and activated [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) 5.0.2 or higher
- For Windows:
    - Installed [MSYS2](https://www.msys2.org/)
    - Installed GNU Make via MSYS2 terminal: `pacman -S make`
    - Added `C:\msys64\usr\bin` in PATH

**Build**
- Open terminal session and navigate to SDK root:
    - For Windows: open `cmd` and navigate to `emsdk`
- Activate SDK env vars for current session:
    - For Windows: exec `emsdk_env.bat`
- In same session navigate to root dir of the project
- Generate makefile: `cmake --preset web-release -DOPT_EU_BUILD=OFF`    
    - `OPT_EU_BUILD` set to ON or OFF depending on your game assets type: EU or NA (how to know see [Game Region](README-game-region.md))   
- Then call build on it: `cmake --build --preset web-release`

**Run**
- First way to run is use sdk util `emrun` and using `worker fs` for assets:
    - Running server   
      Navigate to bin dir `bin/web/opengl/release` and exec:    
      `emrun --no_browser --serve_after_close --port 6931 index.html`  
    - Configuring asset paths    
      No needed. It will requests to select game assets dir at startup when run in browser.     
    - Running game    
      Navigate to the index page by server machine ip addr and specified port.    
      For example: `http://192.168.1.36:6931/` 

- Second way to run is use another http server than `emrun` and `remote fs` for game assets streaming over network:
    - Running server   
      For `remote fs` support of handling range headers is required that is missed in `emrun`   
      So for this you need more advanced solution like 'rangehttpserver' or 'nginx'.    
      Lets take first one for example how to install & run:  
        - So you need [python3](https://www.python.org/downloads/) installed and added in PATH   
        - Then open terminal and exec: `pip install rangehttpserver`   
        - Then navigate to `bin/web/opengl/release`  
        - Then exec `python3 -m RangeHTTPServer --bind 0.0.0.0 6931`    
    - Configuring asset paths      
      Lets take 'rangehttpserver' for example:     
        - Simplest way is create `data/eu` or `data/na` in `bin/web/opengl/release` dir depending    
          on you game [assets type](README-game-region.md) and move or copy them here.  
          (also move `data/Common` from root project path to your assets dir)
        - Or create symbolic link to assets (for windows see [rangehttpserver.bat](rangehttpserver.bat) for example)
    - Running game     
      Navigate to the index page by server machine ip addr and specified port.  
      For example: `http://192.168.1.36:6931/?rfs`        
    
**Run args**
- `lang=%LOCALE%` - forcefully sets language (available only for EU build, value to set: `en`, `fr`, `de`, `es`, `it`)
- `mobile` - forcefully sets check for mobile platform as success even on desktop browser
- `rfs` - activates `remote fs` for game assets streaming over network

**Notes**
- To run a game, the browser should fully support the WASM SIMD feature
- Movies playback is currently unavailable for web version
- On mobile platform minor customization for touch stick added in: Options -> Control Setting
- On mobile platform multiple press inputs were simplified into single or chain press for touch-friendly: 
    - Attack: A -> A, B -> A, C -> B    
    - Lift: Guard then B      
- If you want to use you own [gl4es](https://github.com/ptitSeb/gl4es) make sure to customize your sources of it for renderware like [that](https://github.com/xntrz/gl4es/commit/cb429bd4ec1dbc4292c2adb8cd233604fdf2cf24) - this will eliminates one of perfomance downgrade. Otherwise all static geometry will be upload from wasm memory to gpu every frame instead of using vbo