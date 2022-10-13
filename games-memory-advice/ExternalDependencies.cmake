# Used to override the FetchContent functions in Tensorflow lite library,
# so that dependencies are accessed from external/ instead of being downloaded

function(OverridableFetchContent_Declare CONTENT_NAME)
  set(OVERRIDABLE_ARGS
    GIT_REPOSITORY
    GIT_TAG
    URL
    URL_HASH
    URL_MD5
  )

  set( EXTERNAL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." PARENT_SCOPE)
  set( SOURCE_NAME "${CONTENT_NAME}_SOURCE_DIR")
  set( BINARY_NAME "${CONTENT_NAME}_BINARY_DIR")
  set( ${SOURCE_NAME} "${EXTERNAL_SOURCE_DIR}/${CONTENT_NAME}" PARENT_SCOPE)
  set( ${BINARY_NAME} "${CMAKE_CURRENT_BINARY_DIR}/${CONTENT_NAME}" PARENT_SCOPE)

endfunction()

function(OverridableFetchContent_GetProperties CONTENT_NAME)
  set(EXPORT_VARIABLE_ARGS SOURCE_DIR BINARY_DIR POPULATED)
  cmake_parse_arguments(ARGS
    ""
    "${EXPORT_VARIABLE_ARGS}"
    ""
    ${ARGN}
  )
endfunction()


function(OverridableFetchContent_Populate CONTENT_NAME)
endfunction()