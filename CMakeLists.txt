cmake_minimum_required (VERSION 3.10)
project(Confetti VERSION 0.1.0 LANGUAGES CXX)

#configure_file("${PROJECT_SOURCE_DIR}/Version.h.in" "${PROJECT_BINARY_DIR}/Version.h")

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)

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

find_package(nlohmann_json REQUIRED)
find_package(Misc REQUIRED)
find_package(Msxmlx REQUIRED)

set(PUBLIC_INCLUDE_PATHS
    include
)

set(PRIVATE_INCLUDE_PATHS
    .
    include/Confetti
)

set(SOURCES
    include/Confetti/Appearance.h
    include/Confetti/Builder.h
    include/Confetti/Confetti.h
    include/Confetti/Configuration.h
    include/Confetti/Emitter.h
    include/Confetti/EmitterVolume.h
    include/Confetti/Environment.h
    include/Confetti/Particle.h
    include/Confetti/ParticleSystem.h
    include/Confetti/PointParticle.h
    include/Confetti/SphereParticle.h
    include/Confetti/StreakParticle.h
    include/Confetti/TexturedParticle.h
    
    Appearance.cpp
    Builder.cpp
    Configuration.cpp
    Emitter.cpp
    EmitterVolume.cpp
    Environment.cpp
    Particle.cpp
    ParticleSystem.cpp
    PointParticle.cpp
    SphereParticle.cpp
    StreakParticle.cpp
    TexturedParticle.cpp
)
source_group(Sources FILES ${SOURCES})

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_PATHS} PRIVATE ${PRIVATE_INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME}
    Misc::Misc
    nlohmann_json::nlohmann_json
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} Dxx Wx Msxmlx::Msxlmx MyMath)
endif()

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

if(WIN32)
    add_subdirectory(Dxx)
    add_subdirectory(Msxmlx)
    add_subdirectory(MyMath)
    add_subdirectory(Wx)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    message(STATUS "Testing is enabled. Turn on BUILD_TESTING to build tests.")
    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()
