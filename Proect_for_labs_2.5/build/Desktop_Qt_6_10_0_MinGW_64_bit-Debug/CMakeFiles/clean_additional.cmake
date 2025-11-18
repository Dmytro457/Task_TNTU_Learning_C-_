# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Custom_Calculatro_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Custom_Calculatro_autogen.dir\\ParseCache.txt"
  "Custom_Calculatro_autogen"
  )
endif()
