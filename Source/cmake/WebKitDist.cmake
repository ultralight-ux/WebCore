macro(WEBKIT_DECLARE_DIST_TARGETS _port _tarball_prefix _manifest)
    find_package(Xz REQUIRED)

    configure_file(
        ${_manifest}
        ${PROJECT_BINARY_DIR}/manifest.txt
    )

    add_custom_target(distcheck
        COMMENT "Checking release tarball: ${_tarball_prefix}-${PROJECT_VERSION}.tar"
        DEPENDS ${PROJECT_BINARY_DIR}/manifest.txt
        DEPENDS WebKit
        DEPENDS doc-all
        COMMAND ${TOOLS_DIR}/Scripts/make-dist
                --check --port=${_port}
                --tarball-name=${_tarball_prefix}
                --source-dir=${CMAKE_SOURCE_DIR}
                --build-dir=${PROJECT_BINARY_DIR}
                --version=${PROJECT_VERSION}
                ${PROJECT_BINARY_DIR}/manifest.txt
        COMMAND ${XZ_EXECUTABLE} -evfQ
                ${PROJECT_BINARY_DIR}/${_tarball_prefix}-${PROJECT_VERSION}.tar
        USES_TERMINAL
        VERBATIM
    )

    add_custom_command(
        COMMENT "Creating release tarball: ${_tarball_prefix}-${PROJECT_VERSION}.tar.xz"
        OUTPUT ${PROJECT_BINARY_DIR}/${_tarball_prefix}-${PROJECT_VERSION}.tar.xz
        MAIN_DEPENDENCY ${PROJECT_BINARY_DIR}/manifest.txt
        COMMAND ${TOOLS_DIR}/Scripts/make-dist
                --tarball-name=${_tarball_prefix}
                --source-dir=${CMAKE_SOURCE_DIR}
                --build-dir=${PROJECT_BINARY_DIR}
                --version=${PROJECT_VERSION}
                ${PROJECT_BINARY_DIR}/manifest.txt
        COMMAND ${XZ_EXECUTABLE} -evfQ
                ${PROJECT_BINARY_DIR}/${_tarball_prefix}-${PROJECT_VERSION}.tar
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target(dist
        DEPENDS ${PROJECT_BINARY_DIR}/${_tarball_prefix}-${PROJECT_VERSION}.tar.xz
        DEPENDS WebKit
        DEPENDS doc-all
    )
endmacro()
