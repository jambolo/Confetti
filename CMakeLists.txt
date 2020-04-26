cmake_minimum_required (VERSION 3.10)
project(Confetti VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "A C++ particle system using Vulkan")

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)

#########################################################################
# Build                                                                 #
#########################################################################

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    find_package(glm REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(Misc REQUIRED)
    find_package(Vkx REQUIRED)
    if(WIN32)
        find_package(Msxmlx REQUIRED)
        find_package(Wx REQUIRED)
    endif()
endif()

set(PUBLIC_INCLUDE_PATHS
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(PRIVATE_INCLUDE_PATHS
    .
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}
)

set(SOURCES
    include/Confetti/Appearance.h
    include/Confetti/Builder.h
    include/Confetti/Confetti.h
    include/Confetti/Configuration.h
    include/Confetti/Emitter.h
    include/Confetti/EmitterVolume.h
    include/Confetti/Environment.h
    include/Confetti/JsonConfiguration.h
    include/Confetti/Particle.h
    include/Confetti/ParticleSystem.h
    include/Confetti/PointParticle.h
    include/Confetti/SphereParticle.h
    include/Confetti/StreakParticle.h
    include/Confetti/TexturedParticle.h
    include/Confetti/XmlConfiguration.h
    
    Appearance.cpp
    Builder.cpp
    Emitter.cpp
    EmitterVolume.cpp
    Environment.cpp
    JsonConfiguration.cpp
    Particle.cpp
    ParticleSystem.cpp
    PointParticle.cpp
    SphereParticle.cpp
    StreakParticle.cpp
    TexturedParticle.cpp
    XmlConfiguration.cpp
)
source_group(Sources FILES ${SOURCES})

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC
    Vkx::Vkx
    Vulkan::Vulkan
    Misc::Misc
    nlohmann_json::nlohmann_json
    glm::glm
)
if(WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Msxmlx::Msxmlx
    Wx::Wx
)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_PATHS} PRIVATE ${PRIVATE_INCLUDE_PATHS})
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -DVC_EXTRALEAN
        -D_CRT_SECURE_NO_WARNINGS
        -D_SECURE_SCL=0
        -D_SCL_SECURE_NO_WARNINGS
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#configure_file("${PROJECT_SOURCE_DIR}/Version.h.in" "${PROJECT_BINARY_DIR}/Version.h")

#########################################################################
# Documentation                                                         #
#########################################################################

set(${PROJECT_NAME}_DOXYGEN_OUTPUT_DIRECTORY "" CACHE PATH "Doxygen output directory (empty to disable)")
if(${PROJECT_NAME}_DOXYGEN_OUTPUT_DIRECTORY)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${${PROJECT_NAME}_DOXYGEN_OUTPUT_DIRECTORY})
        configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile)
        add_custom_target(doxygen ALL ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
        )
    endif(DOXYGEN_FOUND)
endif()

#########################################################################
# Testing                                                               #
#########################################################################

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    message(STATUS "Testing is enabled. Turn on BUILD_TESTING to build tests.")
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

#########################################################################
# Installation                                                          #
#########################################################################

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Exporting from the build tree
export(EXPORT ${PROJECT_NAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake NAMESPACE ${PROJECT_NAME}::)

# Register package in user's package registry
export(PACKAGE ${PROJECT_NAME})
