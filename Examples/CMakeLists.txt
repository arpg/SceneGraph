SET( BuildExamples true CACHE "" force )
IF(BuildExamples)
  ADD_SUBDIRECTORY(DisplayModel)
ENDIF()
