set(HDR_TME_SCENARIOS)
set(SRC_TME_SCENARIOS)

include(src/tme/scenarios/default/CMakeLists.txt)
include(src/tme/scenarios/lom/CMakeLists.txt)
include(src/tme/scenarios/ddr/CMakeLists.txt)
include(src/tme/scenarios/citadel/CMakeLists.txt)

list(APPEND HDR_TME_SCENARIOS
  ${HDR_TME_SCENARIOS_DEFAULT}
  ${HDR_TME_SCENARIOS_LOM}
  ${HDR_TME_SCENARIOS_DDR}
  ${HDR_TME_SCENARIOS_CITADEL}
  )

list(APPEND SRC_TME_SCENARIOS
  ${SRC_TME_SCENARIOS_DEFAULT}
  ${SRC_TME_SCENARIOS_LOM}
  ${SRC_TME_SCENARIOS_DDR}
  ${SRC_TME_SCENARIOS_CITADEL}
  )
