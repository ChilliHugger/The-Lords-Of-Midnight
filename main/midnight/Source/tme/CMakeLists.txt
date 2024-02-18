FILE(GLOB HDR_TME Source/tme/baseinc/*.h)
FILE(GLOB SRC_TME Source/tme/base/*.cpp)
FILE(GLOB SRC_UTILS Source/tme/utils/*.cpp)
FILE(GLOB_RECURSE SRC_TME_SCENARIO Source/tme/scenario/*.cpp)

include(Source/tme/scenarios/CMakeLists.txt)

list(APPEND HDR_TME
  ${HDR_TME_SCENARIOS}
  Source/tme/tme_interface.h
  Source/tme/tme.h
  Source/tme/TMEMapBuilder.h
  )

list(APPEND SRC_TME
  ${SRC_TME_SCENARIOS}
  ${SRC_TME_SCENARIO}
  ${SRC_UTILS}
  Source/tme/tme.cpp
  Source/tme/TMEMapBuilder.cpp
  )
