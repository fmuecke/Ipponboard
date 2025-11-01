function(ipponboard_add_windeploy target)
    if(NOT MSVC)
        return()
    endif()

    set(options INCLUDE_TRANSLATIONS)
    cmake_parse_arguments(IP_WINDEPLOY "${options}" "" "" ${ARGN})

    if(NOT DEFINED IPPONBOARD_VCINSTALLDIR)
        cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH _cl_dir)
        cmake_path(GET _cl_dir PARENT_PATH _host_dir)
        cmake_path(GET _host_dir PARENT_PATH _bin_dir)
        cmake_path(GET _bin_dir PARENT_PATH _tools_msvc_version_dir)
        cmake_path(GET _tools_msvc_version_dir PARENT_PATH _tools_msvc_dir)
        cmake_path(GET _tools_msvc_dir PARENT_PATH _tools_dir)
        cmake_path(GET _tools_dir PARENT_PATH _vc_install_dir)

        set(IPPONBOARD_VCINSTALLDIR "${_vc_install_dir}" CACHE INTERNAL "MSVC VC installation dir")
        set(IPPONBOARD_VCTOOLSINSTALLDIR "${_tools_msvc_version_dir}" CACHE INTERNAL "MSVC tools install dir")
    endif()

    set(_deploy_env
        "VCINSTALLDIR=${IPPONBOARD_VCINSTALLDIR}"
        "VCToolsInstallDir=${IPPONBOARD_VCTOOLSINSTALLDIR}"
    )
    if(MSVC_REDIST_DIR)
        list(APPEND _deploy_env "VCToolsRedistDir=${MSVC_REDIST_DIR}")
    endif()

    set(_deploy_args --no-system-d3d-compiler)
    if(NOT IP_WINDEPLOY_INCLUDE_TRANSLATIONS)
        list(APPEND _deploy_args --no-translations)
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env ${_deploy_env}
                ${QTDIR}/bin/windeployqt
                ${_deploy_args}
                "$<TARGET_FILE:${target}>"
        COMMENT "Deploying Qt runtime for ${target}"
    )
endfunction()
