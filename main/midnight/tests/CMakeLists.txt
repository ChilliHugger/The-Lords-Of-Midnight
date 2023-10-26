if(TESTS)
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.1)

  FetchContent_MakeAvailable(Catch2)

  FILE(GLOB HDR tests/*.h)
  FILE(GLOB SRC tests/*.cpp)

  FILE(GLOB_RECURSE HDR_APP tests/${APP_NAME}/*.h)
  FILE(GLOB_RECURSE SRC_APP tests/${APP_NAME}/*.cpp)

  FILE(GLOB HDR_MOCKS tests/mocks/*.h)
  FILE(GLOB SRC_MOCKS tests/mocks/*.cpp)

  FILE(GLOB HDR_STEPS tests/steps/*.h)
  FILE(GLOB SRC_STEPS tests/steps/*.cpp)

  FILE(GLOB HDR_TESTS_TESTS tests/tests/*.h)
  FILE(GLOB SRC_TESTS_TESTS tests/tests/*.cpp)

  list(APPEND GAME_HEADER
    ${HDR}
    ${HDR_APP}
    ${HDR_TESTS}
    ${HDR_STEPS}
    ${HDR_MOCKS}
  )

  list(APPEND GAME_SOURCE
    ${SRC}
    ${SRC_APP}
    ${SRC_TESTS}
    ${SRC_STEPS}
    ${SRC_MOCKS}
  )

  add_compile_options(-D_UNIT_TESTS_)
  add_compile_options(-DMX_DEBUG)

endif()
