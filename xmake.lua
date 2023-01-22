set_project("QLogViewer")

add_requires("yaml-cpp")

set_languages("c++17")

add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")


target("QLogViewer")
	set_kind("binary")
	add_files("src/**.cpp")
	add_files("src/**.h")
	add_files("src/**.ui")
	add_includedirs("src", {public = true});
	add_headerfiles("src/**.h");
	add_headerfiles("src/**.hpp");
	
	add_packages("yaml-cpp")
	
	add_rules("qt.application")
	add_frameworks("QtWidgets")
