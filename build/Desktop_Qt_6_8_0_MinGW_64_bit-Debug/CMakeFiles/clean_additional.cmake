# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\EDA_Player_new_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\EDA_Player_new_autogen.dir\\ParseCache.txt"
  "EDA_Player_new_autogen"
  )
endif()
