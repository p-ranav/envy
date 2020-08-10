cmake_minimum_required(VERSION 3.8)

if(DEFINED PROJECT_NAME)
  set(ENVY_SUBPROJECT ON)
endif()

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
  project(envy VERSION 1.0.0 LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/p-ranav/envy"
    DESCRIPTION "envy: Deserialize environment variables into type-safe structs in modern C++")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "3.9")
  project(envy VERSION 1.0.0 LANGUAGES CXX
    DESCRIPTION "envy: Deserialize environment variables into type-safe structs in modern C++")
else()
  project(envy VERSION 1.9.0 LANGUAGES CXX)
endif()

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(envy INTERFACE)
add_library(envy::envy ALIAS envy)

target_compile_features(envy INTERFACE cxx_std_14)
target_include_directories(envy INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_link_libraries(envy INTERFACE Threads::Threads)

if(NOT ENVY_SUBPROJECT)
  configure_package_config_file(envyConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/envyConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/envy)
  write_basic_package_version_file(envyConfigVersion.cmake
                                   COMPATIBILITY AnyNewerVersion)

  configure_file(envy.pc.in envy.pc @ONLY)

  install(TARGETS envy EXPORT envyTargets)
  install(EXPORT envyTargets
          FILE envyTargets.cmake
          NAMESPACE envy::
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/envy)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/envyConfig.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/envyConfigVersion.cmake
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/envy)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/envy.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
  install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/envy
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          USE_SOURCE_PERMISSIONS
          PATTERN "*.hpp")
  install(FILES LICENSE LICENSE.json LICENSE.visit_struct
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/licenses/envy)

  if(EXISTS "${PROJECT_SOURCE_DIR}/.gitignore")
    # Simple glob to regex conversion (.gitignore => CPACK_SOURCE_IGNORE_FILES)
    file(READ ".gitignore" DOT_GITIGNORE)
    string(REPLACE ";" "RANDOMSEQUENCE" DOT_GITIGNORE "${DOT_GITIGNORE}")
    string(REPLACE "\n" ";" DOT_GITIGNORE "${DOT_GITIGNORE}")
    string(REPLACE "RANDOMSEQUENCE" "\\;" DOT_GITIGNORE "${DOT_GITIGNORE}")
    foreach(IGNORE_LINE ${DOT_GITIGNORE})
      if(NOT IGNORE_LINE OR IGNORE_LINE MATCHES "^#")
        continue()
      endif()
      string(REPLACE "\\" "\\\\" IGNORE_LINE "${IGNORE_LINE}")
      string(REPLACE "." "\\\\." IGNORE_LINE "${IGNORE_LINE}")
      string(REPLACE "*" ".*" IGNORE_LINE "${IGNORE_LINE}")
      string(REPLACE "+" "\\\\+" IGNORE_LINE "${IGNORE_LINE}")
      list(APPEND CPACK_SOURCE_IGNORE_FILES "${IGNORE_LINE}")
    endforeach()
  endif()

  # extra ignored files
  list(APPEND CPACK_SOURCE_IGNORE_FILES
    .editorconfig
    .git
    .gitignore
    .travis.yml
    .appveyor.yml
  )
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
  set(CPACK_GENERATOR "TGZ;TXZ")
  set(CPACK_SOURCE_GENERATOR "TGZ;TXZ")
  include(CPack)
endif()
