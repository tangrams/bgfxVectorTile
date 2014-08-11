solution "bgfxVectorTile"
	
	BGFX_DIR = (path.getabsolute("..") .. "/bgfx/")
	BX_DIR = (BGFX_DIR .. "../bx/")

	configurations { 
		"Debug", 
		"Release" 
	}

	configuration { "Debug" }
		targetdir "bin/debug"

	configuration { "Release" }
		targetdir "bin/release"

	if _ACTION == "clean" then
		os.rmdir("bin")
	end

	project "bgfxVectorTile"
		
		language "C++"
		
		kind "WindowedApp"

		files {
			"src/**.cpp",
			"src/**.h",
			"include/common/**.mm"
		}

		includedirs {
			"include",
			"include/common",
			BX_DIR .. "include",
			BX_DIR .. "include/compat/osx",
			BGFX_DIR .. "include",
			BGFX_DIR .. "3rdparty"
		}

		libdirs {
			"libs"
		}

		links {
			"Cocoa.framework",
			"OpenGL.framework",
			"CoreFoundation.framework",
			"bgfxDebug",
			"example-commonDebug",
			"tess2"
		}

		flags {
			"StaticRuntime",
			"NoMinimalRebuild",
			"NoPCH",
			"NativeWChar",
			"NoRTTI",
			"NoExceptions",
			"NoEditAndContinue",
			"Symbols",
		}

		postbuildcommands { 
			"mkdir -p bin/debug/bgfxVectorTile.app/Contents/Resources",
			"cp data/* bin/debug/bgfxVectorTile.app/Contents/Resources/" 
		}

