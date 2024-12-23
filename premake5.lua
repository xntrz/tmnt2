dir_bin = "%{wks.location}/../../bin/%{cfg.buildcfg}"
dir_obj = "%{wks.location}/../../obj/%{cfg.buildcfg}/%{prj.name}"
dir_src = "%{wks.location}/../../src"
dir_lib = "%{wks.location}/../../vendor"
dir_pch = "src"


workspace "TMNT2"      
   location "build/%{_ACTION}"
   filename "TMNT2"
   configurations { "Debug", "Release" }
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
   filter "configurations:Debug"
      defines { "_DEBUG", "RWDEBUG" }
      symbols "On"
      optimize "Off"
      intrinsics "Off"
      inlining "Disabled"
      staticruntime "On"
      runtime "Debug"
      editandcontinue "On"
   filter "configurations:Release"
      defines { "NDEBUG" }
      symbols "Off"
      optimize "Full"
      intrinsics "On"
      inlining "Auto"
      staticruntime "On"
      runtime "Release"
      editandcontinue "Off"
      flags { "NoIncrementalLink", "NoBufferSecurityCheck", "NoRuntimeChecks", "MultiProcessorCompile" }


project "TMNT2"
   dependson { "System" }
   targetname "%{wks.name}"
   kind "WindowedApp"
   removefiles { 
      "src/System/Common/MemPool.*", -- unused in PC version
      "src/System/Common/Debug/**.*", -- TODO
      "src/Game/Component/Enemy/ConcreteEnemyCharacter/**.*", -- TODO
      "src/Game/Component/Enemy/ConcreteAIModerator/**.*", -- TODO
   }
   includedirs  
   {  
      "$(DXSDK_DIR)Include",
		"%{dir_lib}/rwsdk37/include/d3d9", 
		"%{dir_lib}/cri/include", 
	}
   libdirs  
   { 
      "$(DXSDK_DIR)Lib/x86",
      "%{dir_lib}/cri/lib/x86",
   }
   resincludedirs { "%{dir_src}/System/PC" }
   forceincludes "Game\\pch.hpp"
   pchheader "Game/pch.hpp"
   pchsource "%{dir_pch}/Game/pch.cpp"
   files 
   {    
      "%{dir_src}/System/**.rc",
   }
   links 
   { 
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
      "/ignore:4099" -- PDB 'X' was not found with 'X' or at 'X'; linking object as if no debug info
   }
   filter { "configurations:Debug" }
      libdirs  { "%{dir_lib}/rwsdk37/lib/d3d9/debug", "%{dir_bin}" }
   filter { "configurations:Release" }
      libdirs  {  "%{dir_lib}/rwsdk37/lib/d3d9/release", "%{dir_bin}" }