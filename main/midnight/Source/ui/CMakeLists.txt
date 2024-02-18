FILE(GLOB HDR_UI_CHARACTERS Source/ui/characters/*.h)
FILE(GLOB SRC_UI_CHARACTERS Source/ui/characters/*.cpp)

FILE(GLOB HDR_UI Source/ui/*.h)
FILE(GLOB SRC_UI Source/ui/*.cpp)

list(APPEND HDR_UI
  ${HDR_UI_CHARACTERS}
)

list(APPEND SRC_UI
  ${SRC_UI_CHARACTERS}
)
