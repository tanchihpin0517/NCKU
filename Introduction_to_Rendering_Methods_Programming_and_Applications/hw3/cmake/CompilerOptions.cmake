if (MSVC)
    # CMake enforce /W3 by default. This line tend to delete the regulation.
    string (REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}")
endif()

if (MSVC)
    set(${PROJECT_NAME}_CXX_FLAGS_DEBUG
        "/W4"
        "/WX"
    )

    if(NOT (MSVC_VERSION LESS 1910)) # MSVC v14.1 (Visual Studio 2017 15.0)
        list(APPEND ${PROJECT_NAME}_CXX_FLAGS_DEBUG "/permissive-")
    endif()
    
    set(${PROJECT_NAME}_CXX_FLAGS_RELEASE
        "/W1"
        "/O2"
    )
else()
    set(${PROJECT_NAME}_CXX_FLAGS_DEBUG
        "-Og"
        "-fno-common"
        "-pedantic"
        "-pedantic-errors"
        "-W"
        "-Wall"
        "-Wconversion"
        "-Wdouble-promotion"
        "-Werror"
        "-Wextra"
        "-Wfloat-equal"
        "-Wformat=2"
        "-Winline"
        "-Wno-long-long"
        "-Wshadow"
        "-Wsign-promo"
        "-Wundef"
    )
    set(${PROJECT_NAME}_CXX_FLAGS_RELEASE "-O3")
endif()
