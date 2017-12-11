require("netbeans")
require("cmake")
require("raw")

workspace("Chip8Emulador")
    configurations({"Debug","Release"})
    platforms({"Windows","Linux"})
    location("build/%{_ACTION}")

    project("Chip8Emulador")
        --location("Chip8Emulador")
        kind("ConsoleApp")
        language("C++")
        targetdir("bin/%{cfg.buildcfg}")

        files({"include/**.h","src/**.cpp","src/**.c","**.lua"})

        includedirs ({
		    "include"
		})

        links{"SDL2main","SDL2","SDL2_image"}

        --postbuildcommands({"{COPY} roms/ %{cfg.buildtarget.directory}/roms"})

        filter("configurations:Debug")
            defines({"DEBUG"})
            symbols("On")

        filter("configurations:Release")
            defines({"NDEBUG"})
            optimize("On")

        filter("platforms:Windows")
            defines({"WINDOWS"})
            system("windows")
            includedirs ({
                "libs/SDL2-2.0.5/include",
                "libs/SDL2_image-2.0.1/include",
                "libs/SDL2_ttf-2.0.14/include"
            })
            libdirs ({
                "libs/SDL2-2.0.5/lib/x86",
                "libs/SDL2_image-2.0.1/lib/x86",
                "libs/SDL2_ttf-2.0.14/lib/x86"
            })

        filter("platforms:Linux")
            system("linux")
            defines({"LINUX"})