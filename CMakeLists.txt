cmake_minimum_required(VERSION 3.12.4)

project(
  jctest
  VERSION 0.5
  LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CMakeDependentOption)
cmake_dependent_option(JCTEST_INSTALL "Install jctest" ON
                       "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF)

add_library(${PROJECT_NAME} INTERFACE)

if(JCTEST_INSTALL)
  install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    PUBLIC_HEADER
    INCLUDES
    DESTINATION include)

  include(WriteBasicConfigVersionFile)
  write_basic_config_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake VERSION
    ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)

  install(FILES src/jc_test.h DESTINATION include)
  install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()

cmake_dependent_option(JCTEST_TEST "Build tests" ON
                       "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF)
if(JCTEST_TEST)
  find_program(CCACHE ccache)
  if(CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  else()
    message(STATUS "ccache not found")
  endif()

  find_program(CLANG_TIDY clang-tidy)
  if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} --fix --header-filter=inc/)
  else()
    message(STATUS "clang-tidy not found")
  endif()

  find_program(IWYU include-what-you-use)
  if(IWYU)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU};-Xiwyu;--cxx17ns")
  else()
    message(STATUS "include-what-you-use not found")
  endif()

  macro(add_test_gtest NAME)
    add_executable(${NAME} ${ARGN})
    target_include_directories(${NAME} PRIVATE src)
    gtest_discover_tests(${NAME})
  endmacro()

  macro(add_test_by_name NAME)
    add_test_gtest(${NAME} test/main.cpp test/${NAME}.cpp)
  endmacro()

  enable_testing()
  add_executable(test_doctest test/test_doctest.cpp)
  target_include_directories(test_doctest PRIVATE src)
  add_test(test_doctest test_doctest)

  include(GoogleTest)
  foreach(test IN ITEMS test_array test_death test_empty test_expect
                        test_params test_typed_test)
    add_test_by_name(${test})
  endforeach()
endif()
