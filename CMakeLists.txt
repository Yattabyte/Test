################
### KeyTest ###
################
cmake_minimum_required(VERSION 3.9.2)
project(KeyTest VERSION 0.0.0 DESCRIPTION "Test")
include(ExternalProject)

# Set mandatory requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS YES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
option(BUILD_TESTING "Build Unit Tests" ON)
option(CODE_COVERAGE "Enable code coverage reporting for GCC/Clang" OFF)
option(STATIC_ANALYSIS "Enable static code analysis using GCC" OFF)

# Set compilation flags per-compiler
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Oi /Ot /GF /Qpar /std:c++17 /Zc:rvalueCast /W4 /permissive-")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Gy /MDd /JMC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /GL /MD")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG:incremental")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FASTLINK /INCREMENTAL:NO")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER Other)
elseif(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wshadow -Wno-unused-value -Wunused-variable -Wunused-parameter -Wunused-function -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
endif()

# Add source files
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
add_subdirectory(external/glfw)
add_subdirectory(src)

# Optionally build unit tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)

    # Optionally perform static code analysis tests
    if (STATIC_ANALYSIS)
        include(CTest)
        find_program(MEMORYCHECK_COMMAND valgrind)
        if (CTEST_MEMORYCHECK_COMMAND)
            set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
        endif()
    endif()
endif()


#################
# DOXYGEN CHECK #
#################
configure_file(Doxyfile.in ${PROJECT_SOURCE_DIR}/Doxyfile.out)
option(KeyTest_BUILD_DOCS "Generate documentation" true)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND AND KeyTest_BUILD_DOCS)
    set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/docs/)
    set(DOXYGEN_MARKDOWN_SUPPORT YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)
    doxygen_add_docs(KeyTest_BUILD_DOCS ${PROJECT_SOURCE_DIR} COMMENT "Generate documentation")
endif()