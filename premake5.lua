dir_bin = "%{wks.location}/../../bin/%{cfg.buildcfg}"
dir_obj = "%{wks.location}/../../obj/%{prj.name}/%{cfg.buildcfg}"
dir_src = "%{wks.location}/../../src"
dir_lib = "%{wks.location}/../../vendor"
dir_pch = "src"


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
   location "build/%{_ACTION}"
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
      "%{dir_lib}/rwsdk37/include/d3d9", 
      "%{dir_lib}/cri/include", 
   }
   libdirs { 
      "$(DXSDK_DIR)Lib/x86",
      "%{dir_lib}/cri/lib/x86",
   }
   resincludedirs { "%{dir_src}/System/PC" }
   forceincludes "Game\\pch.hpp"
   pchheader "Game/pch.hpp"
   pchsource "%{dir_pch}/Game/pch.cpp"
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
      "rtbmp.lib",
      "rtpng.lib",
      "rpworld.lib",
      "rtpitexd.lib",
      "rwcore.lib",
      "rtfsyst.lib",
      "rtanim.lib",
      "rpuvanim.lib",
      "rprandom.lib",
      "rphanim.lib",
      "rpcollis.lib",
      "rt2d.lib",
      "rt2danim.lib",
      "rpskin.lib",
      "rppvs.lib",
      "rplodatm.lib",
      "rpmatfx.lib",
      "rtdict.lib",
      "rtquat.lib",
      "rtintsec.lib",
      "rtcharse.lib",
      "legacy_stdio_definitions.lib",
      "rptoon.lib"
   } 
   linkoptions {
      "/ignore:4099", -- PDB 'X' was not found with 'X' or at 'X'; linking object as if no debug info
   }
   filter { "configurations:"..getConfigsDebug() }
      libdirs { "%{dir_lib}/rwsdk37/lib/d3d9/debug", "%{dir_bin}" }
   filter { "configurations:"..getConfigsRelease() }
      libdirs { "%{dir_lib}/rwsdk37/lib/d3d9/release", "%{dir_bin}" }
   filter {}
end


project "TMNT2"
   project_base_initialize()
   filter { "configurations:"..getConfigsRelease() }
      removefiles {  "src/Game/Sequence/Test/**" }
   filter { "configurations:"..getConfigsAllEU() }
      defines { "TMNT2_BUILD_EU" }
   --filter { "configurations:"..getConfigsAllNA() }
   --   debugargs { "-afspath=Z:/tmnt2_na -wnd" }
   --filter { "configurations:"..getConfigsAllEU() }
   --   debugargs { "-afspath=Z:/tmnt2_eu -wnd -lang=en" }