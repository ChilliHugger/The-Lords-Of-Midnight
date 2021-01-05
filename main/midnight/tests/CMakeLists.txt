if(TESTS)
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.1)

  FetchContent_MakeAvailable(Catch2)

  FILE(GLOB HDR_TESTS1 tests/${APP_NAME}/*.h)
  FILE(GLOB SRC_TESTS1 tests/${APP_NAME}/*.cpp)
  FILE(GLOB HDR_TESTS2 tests/*.h)
  FILE(GLOB SRC_TESTS2 tests/*.cpp)

  list(APPEND GAME_HEADER
    ${HDR_TESTS1}
    ${HDR_TESTS2}
  )

  list(APPEND GAME_SOURCE
    ${SRC_TESTS1}
    ${SRC_TESTS2}
  )

  add_compile_options(-D_UNIT_TESTS_)

endif()
