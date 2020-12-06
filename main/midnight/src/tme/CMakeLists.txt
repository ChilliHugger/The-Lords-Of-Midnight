FILE(GLOB HDR_TME src/tme/baseinc/*.h)
FILE(GLOB SRC_TME src/tme/base/*.cpp)
FILE(GLOB SRC_TME_SCENARIO src/tme/scenario/*.cpp)

include(src/tme/scenarios/CMakeLists.txt)

list(APPEND HDR_TME
  ${HDR_TME_SCENARIOS}
  src/tme/tme_interface.h
  src/tme/tme.h
  src/tme/TMEMapBuilder.h
  )

list(APPEND SRC_TME
  ${SRC_TME_SCENARIOS}
  ${SRC_TME_SCENARIO}
  src/tme/tme.cpp
  src/tme/TMEMapBuilder.cpp
  )
