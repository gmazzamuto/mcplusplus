if(DOXYGEN_FOUND)
  file(GLOB DOXYGEN_FILES . *.dox)
  configure_file(Doxyfile.in
                 ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(documentation SOURCES ${DOXYGEN_FILES} references.bib DoxygenLayout.xml stylesheet.css
                    COMMAND mkdir -p  ${DOCUMENTATION_DIR} && ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
