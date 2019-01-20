cmake_minimum_required (VERSION 3.4)

if (${PROJECT_NAME}_BUILD_UNIT_TESTS)

    set(EXE ${PROJECT_NAME}-test)
    
    set(SOURCES
        main.cpp
    )

    add_executable(${EXE} ${SOURCES})
    target_compile_definitions(${EXE}
        PRIVATE
            -DNOMINMAX
            -DWIN32_LEAN_AND_MEAN
            -DVC_EXTRALEAN
            -D_CRT_SECURE_NO_WARNINGS
            -D_SECURE_SCL=0
            -D_SCL_SECURE_NO_WARNINGS
    )
    target_link_libraries(${EXE} ${PROJECT_NAME} Dxx Math Misc Time Wx)

endif(${PROJECT_NAME}_BUILD_UNIT_TESTS)
