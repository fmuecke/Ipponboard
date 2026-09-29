# Workaround: A Qt install whose CMake package files export only the Debug
# configuration (e.g. a debug build installed over a release-only prefix)
# makes CMake silently link the debug Qt libraries (/MDd) into Release builds.
# The mixed CRT runtimes corrupt QString/QByteArray heap ownership across
# module boundaries. If the package lacks Release entries but the release
# libraries exist, patch the imported targets so each configuration links
# its own Qt runtime.
#
# Must be included in the top-level CMakeLists.txt before any add_subdirectory()
# that calls fix_qt_imported_configs(), because CMake commands are only visible
# after their definition is processed.
function(fix_qt_imported_configs qtdir)
	if(NOT MSVC)
		return()
	endif()
	if(USE_QT6)
		set(qt_prefix Qt6)
	else()
		set(qt_prefix Qt5)
	endif()
	foreach(module
			Core Gui Widgets Network Core5Compat PrintSupport Multimedia XmlPatterns
			EntryPointImplementation)
		if(NOT TARGET ${qt_prefix}::${module})
			continue()
		endif()
		# Qt6EntryPointImplementation is a STATIC helper whose file is named
		# Qt6EntryPoint[.lib|d.lib], unlike the shared modules (Qt6<Module>).
		set(libname ${module})
		if(module STREQUAL "EntryPointImplementation")
			set(libname "EntryPoint")
		endif()
		get_target_property(target_type ${qt_prefix}::${module} TYPE)
		if(NOT target_type MATCHES "SHARED")
			get_target_property(rel_loc ${qt_prefix}::${module} IMPORTED_LOCATION_RELEASE)
		else()
			get_target_property(rel_loc ${qt_prefix}::${module} IMPORTED_IMPLIB_RELEASE)
		endif()
		if(rel_loc)
			continue()
		endif()
		set(release_lib "${qtdir}/lib/${qt_prefix}${libname}.lib")
		set(release_dll "${qtdir}/bin/${qt_prefix}${libname}.dll")
		if(NOT target_type MATCHES "SHARED")
			if(NOT EXISTS "${release_lib}")
				message(WARNING "${qt_prefix}::${module} exports no Release configuration and ${release_lib} is missing; Release builds will link debug Qt libraries")
				continue()
			endif()
		elseif(NOT (EXISTS "${release_lib}" AND EXISTS "${release_dll}"))
			message(WARNING "${qt_prefix}::${module} exports no Release configuration and ${release_lib} is missing; Release builds will link debug Qt libraries")
			continue()
		endif()
		get_target_property(cfgs ${qt_prefix}::${module} IMPORTED_CONFIGURATIONS)
		if(cfgs STREQUAL "cfgs-NOTFOUND")
			set(cfgs "")
		endif()
		if(NOT ";${cfgs};" MATCHES ";Release;")
			set_property(TARGET ${qt_prefix}::${module} APPEND PROPERTY IMPORTED_CONFIGURATIONS Release)
		endif()
		if(NOT target_type MATCHES "SHARED")
			set_target_properties(${qt_prefix}::${module} PROPERTIES
				IMPORTED_LOCATION_RELEASE "${release_lib}"
			)
		else()
			set_target_properties(${qt_prefix}::${module} PROPERTIES
				IMPORTED_LOCATION_RELEASE "${release_dll}"
				IMPORTED_IMPLIB_RELEASE "${release_lib}"
			)
		endif()
		message(STATUS "${qt_prefix}::${module} exports no Release configuration; patched to use ${release_lib}")
	endforeach()
endfunction()
