include(FetchContent)

if(NOT TARGET Catch2::Catch2WithMain)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.11.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(Catch2)
endif()
