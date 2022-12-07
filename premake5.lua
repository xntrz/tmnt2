dir_bin = "%{wks.location}/../../bin/%{prj.name}/%{cfg.buildcfg}"
dir_obj = "%{wks.location}/../../obj/%{prj.name}/%{cfg.buildcfg}"
dir_src = "%{wks.location}/../../src"
dir_lib = "%{wks.location}/../../vendor"
dir_pch = "src"

workspace "tmnt2"
   location "build/%{_ACTION}"
   filename "tmnt2"
   configurations { "Debug", "Release" }
   system "Windows"
   startproject "Base"
   targetdir "%{dir_bin}"
   objdir "%{dir_obj}"
   language "C++"
   rtti "Off"
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
      "%{dir_src}/**.rc",
      "%{dir_src}/**.manifest",
   }
   filter { "kind:WindowedApp or SharedLib" }
      linkoptions { "/DYNAMICBASE:NO" }   
   filter "configurations:Debug"
      defines { "WIN32", "_DEBUG", "_WINDOWS" }
      symbols "On"
      optimize "Off"
      intrinsics "Off"
      inlining "Disabled"
      staticruntime "On"
      runtime "Debug"
      editandcontinue "On"
   filter "configurations:Release"
      defines { "WIN32", "NDEBUG", "_WINDOWS" }
      symbols "Off"
      optimize "Full"
      intrinsics "On"
      inlining "Auto"
      staticruntime "On"
      runtime "Release"
      editandcontinue "Off"
      flags { "NoIncrementalLink", "NoBufferSecurityCheck", "NoRuntimeChecks", "MultiProcessorCompile" }

project "game"
   targetname "%{wks.name}"
   characterset "MBCS"
   defines { "_TARGET_PC" }
   kind "WindowedApp"
   includedirs  {  "%{dir_lib}/rwsdk37/include/d3d9", "%{dir_lib}/cri/include", }
   libdirs  { "%{dir_lib}/cri/lib/x86" }
   resincludedirs { "%{dir_src}/System/PC" }
   forceincludes "System\\Common\\pch.hpp"
   pchheader "System/Common/pch.hpp"
	pchsource "%{dir_pch}/System/Common/pch.cpp"
   linkoptions { "/SAFESEH:NO", "/DYNAMICBASE:NO" }
   links 
   { 
      "dinput8.lib",
      "dxguid.lib",
      "winmm.lib",
      "ksuser.lib",
      "dsound.lib",
      "cri_adxpcx86_dsound8.lib",
      "cri_mwsfdpcx86.lib",
      --"cri_adxpcx86_xaudio2.lib",
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
   } 
   filter { "configurations:Debug" }
      defines { "RWDEBUG" }
      libdirs  { "%{dir_lib}/rwsdk37/lib/d3d9/debug" }
   filter { "configurations:Release" }
      libdirs  { "%{dir_lib}/rwsdk37/lib/d3d9/release" }