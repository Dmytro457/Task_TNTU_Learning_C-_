# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Product_catalog_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Product_catalog_autogen.dir\\ParseCache.txt"
  "Product_catalog_autogen"
  )
endif()
