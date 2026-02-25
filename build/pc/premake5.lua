newoption {
    trigger     = "idfs",
    description = "Using file ids instead file names",
    category    = "project",
}


local drvmap = {
    opengl = "opengl",
    d3d9   = "d3d9",
}

newoption {
   trigger     = "rwdrv",
   description = "Using specific rw gx driver",
   default     = "d3d9",
   category    = "project",
   allowed = {
      { drvmap.opengl, "OpenGL"      },
      { drvmap.d3d9,   "Direct3D 9"  },
   }
}


rwos    = "win"
rwsdk   = "rwsdk37/%{rwos}"
rwdrv   = _OPTIONS["rwdrv"]
dir_bin = "%{wks.location}/../../../bin/%{rwdrv}/%{cfg.buildcfg}"
dir_obj = "%{wks.location}/../../../obj/%{rwdrv}/%{cfg.buildcfg}"
dir_src = "%{wks.location}/../../../src"
dir_lib = "%{wks.location}/../../../vendor"
dir_pch = "%{prj.location}/../../../src"


config_types = {
   -- NA
   "NA_DEBUG",
   "NA_RELEASE",   
   -- EU
   "EU_DEBUG",
   "EU_RELEASE",
}


local function isempty(str)
  return (str == nil) or (str == '')
end


local function getConfigs(filterStr, exclude)
    result = ""
    concatSep = " or "
    concatNum = 0
    for i, k in ipairs(config_types) do
      if (isempty(filterStr)) then
         if (concatNum ~= 0) then
            result = result..concatSep
         end
         result = result..k
         concatNum = concatNum + 1
      else
         doConcat = false
         if (string.find(k, filterStr)) then
            if not (exclude) then
               doConcat = true
            end
         else
            if (exclude) then
               doConcat = true
            end
         end      
         if doConcat then
            if (concatNum ~= 0) then
               result = result..concatSep
            end
            result = result..k
            concatNum = concatNum + 1
         end
      end      
    end
    return result
end


local function getConfigsAll()
   return getConfigs("")
end


local function getConfigsDebug()
   return getConfigs("DEBUG")
end


local function getConfigsRelease()
   return getConfigs("RELEASE")
end


local function getConfigsAllNA()
   return getConfigs("NA")
end


local function getConfigsAllEU()
   return getConfigs("EU")
end


workspace "TMNT2"      
   location "%{_ACTION}"
   filename "TMNT2"
   configurations { config_types }
   system "Windows"
   startproject "Game"
   targetdir "%{dir_bin}"
   objdir "%{dir_obj}"
   language "C++"
   rtti "Off"
   characterset "MBCS"
   defines { "WIN32", "_WINDOWS", "TARGET_PC" }
   includedirs 
   { 
      "%{dir_src}",
   }  
   files 
   { 
      "%{dir_src}/**.h",
      "%{dir_src}/**.c",
      "%{dir_src}/**.hpp",
      "%{dir_src}/**.cpp",
      "%{dir_src}/**.inl",
      "%{dir_src}/**.manifest",
   }
   filter { "kind:WindowedApp" }
      linkoptions { "/SAFESEH:NO", "/DYNAMICBASE:NO" }
   filter { "kind:WindowedApp or StaticLib" }
      linkoptions { "/MACHINE:X86" }   
   filter { "configurations:"..getConfigsDebug() }
      defines { "_DEBUG", "RWDEBUG" }
      symbols "On"
      optimize "Off"
      intrinsics "Off"
      inlining "Disabled"
      staticruntime "On"
      runtime "Debug"
      editandcontinue "On"
   filter { "configurations:"..getConfigsRelease() }
      defines { "NDEBUG" }
      symbols "Off"
      optimize "Full"
      intrinsics "On"
      inlining "Auto"
      staticruntime "On"
      runtime "Release"
      editandcontinue "Off"
      flags { "NoIncrementalLink", "NoBufferSecurityCheck", "NoRuntimeChecks", "MultiProcessorCompile" }
   filter {}


function project_base_initialize()
   dependson { "System" }
   targetname "%{wks.name}"
   kind "WindowedApp"
   --warnings "High"
   removefiles { 
      "src/System/Common/MemPool.*", -- unused in PC version
      "src/System/Common/Debug/**.*", -- TODO
   }
   includedirs {  
      "$(DXSDK_DIR)Include",
      "%{dir_lib}/%{rwsdk}/include/%{rwdrv}", 
      "%{dir_lib}/cri/include", 
   }
   libdirs { 
      "$(DXSDK_DIR)Lib/x86",
      "%{dir_lib}/cri/lib/x86",
   }
   resincludedirs { "%{dir_src}/System/PC" }
   forceincludes "Game\\pch.hpp"
   pchheader ("Game/pch.hpp")
   pchsource ("../../src/Game/pch.cpp") -- must be relative to the premake5 script
   files {    
      "%{dir_src}/System/**.rc",
   }
   links  { 
      -- directx
      "dinput8.lib",
      "dxguid.lib",
      "dsound.lib",
      -- win32
      "winmm.lib",
      "ksuser.lib",
      -- cri
      "cri_adxpcx86_dsound8.lib",
      "cri_mwsfdpcx86.lib",
      --"cri_adxpcx86_xaudio2.lib",
      
      -- rw
      "rwcore.lib",
      
      "rpcollis.lib",
      "rphanim.lib",
      "rplodatm.lib",
      "rpmatfx.lib",
      "rppvs.lib",
      "rprandom.lib",
      "rpskin.lib",
      "rptoon.lib",
      "rpuvanim.lib",
      "rpworld.lib",
      
      "rt2d.lib",
      "rt2danim.lib",
      "rtanim.lib",
      "rtbmp.lib",
      "rtcharse.lib",
      "rtdict.lib",
      "rtfsyst.lib",
      "rtintsec.lib",
      "rtpitexd.lib",
      "rtpng.lib",
      "rtquat.lib",

      "legacy_stdio_definitions.lib",
   } 
   linkoptions {
      "/ignore:4099", -- PDB 'X' was not found with 'X' or at 'X'; linking object as if no debug info
   }
   buildoptions {
      "/wd4200", -- nonstandard extension used: zero-sized array in struct/union
      "/wd4201", -- nonstandard extension used: nameless struct/union
      "/wd4996", -- function or variable may be unsafe
      "/wd4100", -- unreferenced formal parameter
      "/wd4189", -- local variable is initialized but not referenced
      "/wd4505", -- unreferenced local function has been removed
      "/wd5054", -- operator X deprecated between enumerations of different types
   }
   filter { "configurations:"..getConfigsDebug() }
      libdirs { "%{dir_lib}/%{rwsdk}/lib/%{rwdrv}/debug", "%{dir_bin}" }
   filter { "configurations:"..getConfigsRelease() }
      libdirs { "%{dir_lib}/%{rwsdk}/lib/%{rwdrv}/release", "%{dir_bin}" }
   filter {}
end


externalrule "preproc_check"
   fileExtension ".targets"
   filename "preproc_check"
   

externalrule "preproc"
   fileExtension ".targets"
   filename "preproc"


project "TMNT2"
   project_base_initialize()   
   filter { "configurations:"..getConfigsRelease() }
      removefiles {  "src/Game/Sequence/Test/**" }   
   filter { "configurations:"..getConfigsAllEU() }
      defines { "TMNT2_BUILD_EU" }   
   filter{}

   -- init driver defines
   local drv = drvmap[_OPTIONS["rwdrv"]]
   defines { "TMNT2_RWDRV_"..string.upper(drv) }
   
   -- init idfsyst if requested
   if _OPTIONS["idfs"] then
      defines { "TMNT2_IDFSYST" }
      rules { "preproc_check", "preproc" }
   end

   -- dbg args
   --filter { "configurations:"..getConfigsAllNA() }
   --   debugargs { "-afspath=Z:/tmnt2_na -wnd" }
   --filter { "configurations:"..getConfigsAllEU() }
   --   debugargs { "-afspath=Z:/tmnt2_eu -wnd -lang=en" }