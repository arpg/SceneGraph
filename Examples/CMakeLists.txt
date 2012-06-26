FIND_PACKAGE(Pangolin REQUIRED)

set( BuildExamples true CACHE "" force )
IF(Pangolin_FOUND AND BuildExamples)
  ADD_SUBDIRECTORY(DisplayModel)
ENDIF()
