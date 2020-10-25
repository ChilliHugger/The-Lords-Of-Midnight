FILE(GLOB HDR_UI_CHARACTERS src/ui/characters/*.h)
FILE(GLOB SRC_UI_CHARACTERS src/ui/characters/*.cpp)

FILE(GLOB HDR_UI src/ui/*.h)
FILE(GLOB SRC_UI src/ui/*.cpp)

list(APPEND HDR_UI
  ${HDR_UI_CHARACTERS}
)

list(APPEND SRC_UI
  ${SRC_UI_CHARACTERS}
)
