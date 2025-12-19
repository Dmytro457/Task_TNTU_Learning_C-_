# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "Address_books_autogen"
  "CMakeFiles\\Address_books_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Address_books_autogen.dir\\ParseCache.txt"
  )
endif()
