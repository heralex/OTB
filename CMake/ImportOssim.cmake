MESSAGE(STATUS "Importing Ossim...")

SET(DEFAULT_OTB_USE_EXTERNAL_OSSIM  OFF)

# OSGeo4W provides an "ossim" package : use it otherwise if it is installed and not used by OTB
# we get conflicts (because OSGeo4W include dir is included for other dependencies 
IF(WIN32)
  SET(DEFAULT_OTB_USE_EXTERNAL_OSSIM  ON)
ENDIF()

OPTION(OTB_USE_EXTERNAL_OSSIM "Use an outside build of Ossim." ${DEFAULT_OTB_USE_EXTERNAL_OSSIM})
MARK_AS_ADVANCED(OTB_USE_EXTERNAL_OSSIM)

IF(OTB_USE_EXTERNAL_OSSIM)
  FIND_PACKAGE(Ossim REQUIRED)
ELSE(OTB_USE_EXTERNAL_OSSIM)
  SET(OSSIM_LIBRARIES otbossim)
ENDIF(OTB_USE_EXTERNAL_OSSIM)
