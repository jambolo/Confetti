cmake_minimum_required (VERSION 3.10)

include(GoogleTest)

add_definitions(
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
    -DVC_EXTRALEAN
    -D_CRT_SECURE_NO_WARNINGS
    -D_SECURE_SCL=0
    -D_SCL_SECURE_NO_WARNINGS
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(googletest/googletest/include)

file(GLOB FILES *.cpp)
foreach(FILE ${FILES})
    get_filename_component(TEST ${FILE} NAME_WE)
    add_executable(Misc_${TEST} ${FILE})
    target_link_libraries(Confetti_${TEST} ${PROJECT_NAME} gtest gtest_main)
    gtest_discover_tests(Confetti_${TEST})
    target_compile_features(Confetti_${TEST} PUBLIC cxx_std_17)
    set_target_properties(Confetti_${TEST} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

add_subdirectory(googletest)
