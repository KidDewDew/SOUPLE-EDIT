# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\appSoupleEdit_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appSoupleEdit_autogen.dir\\ParseCache.txt"
  "appSoupleEdit_autogen"
  "souple_pdf\\CMakeFiles\\souple_pdf_autogen.dir\\AutogenUsed.txt"
  "souple_pdf\\CMakeFiles\\souple_pdf_autogen.dir\\ParseCache.txt"
  "souple_pdf\\souple_pdf_autogen"
  )
endif()
