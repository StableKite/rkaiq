# - Check glibc version
# CHECK_GLIBC_VERSION()
#
# Once done this will define
#
#   GLIBC_VERSION - glibc version
#
macro (CHECK_GLIBC_VERSION)
    execute_process (
        COMMAND ${CMAKE_C_COMPILER} -print-file-name=ld-uClibc.so.1
        OUTPUT_VARIABLE UCLIBC
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (EXISTS "${UCLIBC}" AND NOT UCLIBC STREQUAL "ld-uClibc.so.1")
        set(C_LIBRARY_NAME "uclibc")
        message(STATUS "Detected ${C_LIBRARY_NAME} userspace")
    else()
        # The compiler frontend does not define the target C library ABI.
        # Clang targeting a GNU sysroot must use the same glibc prebuilts as GCC.
        set(C_LIBRARY_NAME "glibc")
        message(STATUS "Detected ${C_LIBRARY_NAME} userspace")
    endif()
endmacro (CHECK_GLIBC_VERSION)
