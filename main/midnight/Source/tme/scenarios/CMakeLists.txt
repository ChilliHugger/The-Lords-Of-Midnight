set(HDR_TME_SCENARIOS)
set(SRC_TME_SCENARIOS)

include(Source/tme/scenarios/default/CMakeLists.txt)
include(Source/tme/scenarios/lom/CMakeLists.txt)
include(Source/tme/scenarios/lom_novel/CMakeLists.txt)
include(Source/tme/scenarios/ddr/CMakeLists.txt)
include(Source/tme/scenarios/citadel/CMakeLists.txt)

list(APPEND HDR_TME_SCENARIOS
  ${HDR_TME_SCENARIOS_DEFAULT}
  ${HDR_TME_SCENARIOS_LOM}
  ${HDR_TME_SCENARIOS_LOM_NOVEL}
  ${HDR_TME_SCENARIOS_DDR}
  ${HDR_TME_SCENARIOS_CITADEL}
  )

list(APPEND SRC_TME_SCENARIOS
  ${SRC_TME_SCENARIOS_DEFAULT}
  ${SRC_TME_SCENARIOS_LOM}
  ${SRC_TME_SCENARIOS_LOM_NOVEL}
  ${SRC_TME_SCENARIOS_DDR}
  ${SRC_TME_SCENARIOS_CITADEL}
  )
