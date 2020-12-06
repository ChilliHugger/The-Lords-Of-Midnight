FILE(GLOB HDR_LIBRARY src/library/inc/*.h)
FILE(GLOB SRC_LIBRARY src/library/src/*.cpp)

list(APPEND HDR_LIBRARY
  src/library/chilli.h
  )
