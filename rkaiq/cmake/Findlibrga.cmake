find_package(PkgConfig QUIET)
if(NOT RKAIQ_STATIC_RGA_PROVIDER)
    pkg_check_modules(librga QUIET "librga")
endif()

include(FindPackageHandleStandardArgs)
find_path(LIBRGA_IM2D_INCLUDE_DIR
    NAMES im2d.h
    PATHS ${LIBRGA_INCLUDE_DIR} ${LIBRGA_INCLUDE_DIRS}
    PATH_SUFFIXES rga im2d_api
)
find_path(LIBRGA_CORE_INCLUDE_DIR
    NAMES rga.h
    PATHS ${LIBRGA_INCLUDE_DIR} ${LIBRGA_INCLUDE_DIRS}
    PATH_SUFFIXES rga include
)
if(NOT LIBRGA_LIBRARY)
    find_library(LIBRGA_LIBRARY NAMES rga)
endif()
find_package_handle_standard_args(librga
    DEFAULT_MSGS
    LIBRGA_IM2D_INCLUDE_DIR
    LIBRGA_CORE_INCLUDE_DIR
    LIBRGA_LIBRARY
)
mark_as_advanced(
    LIBRGA_IM2D_INCLUDE_DIR
    LIBRGA_CORE_INCLUDE_DIR
    LIBRGA_LIBRARY
)

if(librga_FOUND)
    get_filename_component(LIBRGA_LIBRARY_DIR "${LIBRGA_LIBRARY}" DIRECTORY)
    set(LIBRGA_LIBRARIES "${LIBRGA_LIBRARY}")
    set(LIBRGA_INCLUDE_DIRS "${LIBRGA_IM2D_INCLUDE_DIR};${LIBRGA_CORE_INCLUDE_DIR}")
    set(LIBRGA_CFLAGS "-DHAS_LIBRGA")

    if(LIBRGA_LIBRARY AND NOT TARGET librga::librga)
        add_library(librga::librga UNKNOWN IMPORTED)
        set_target_properties(librga::librga PROPERTIES
            IMPORTED_LOCATION "${LIBRGA_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBRGA_INCLUDE_DIRS}"
        )
    endif()
endif()
