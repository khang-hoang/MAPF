# Generated by Boost 1.71.0

# address-model=64

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  _BOOST_SKIPPED("libboost_type_erasure.dylib" "64 bit, need 32")
  return()
endif()

# layout=tagged

# toolset=xgcc42

# link=shared

if(DEFINED Boost_USE_STATIC_LIBS)
  if(Boost_USE_STATIC_LIBS)
    _BOOST_SKIPPED("libboost_type_erasure.dylib" "shared, Boost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}")
    return()
  endif()
else()
  if(WIN32)
    _BOOST_SKIPPED("libboost_type_erasure.dylib" "shared, default on Windows is static, set Boost_USE_STATIC_LIBS=OFF to override")
    return()
  endif()
endif()

# runtime-link=shared

if(Boost_USE_STATIC_RUNTIME)
  _BOOST_SKIPPED("libboost_type_erasure.dylib" "shared runtime, Boost_USE_STATIC_RUNTIME=${Boost_USE_STATIC_RUNTIME}")
  return()
endif()

# runtime-debugging=off

if(Boost_USE_DEBUG_RUNTIME)
  _BOOST_SKIPPED("libboost_type_erasure.dylib" "release runtime, Boost_USE_DEBUG_RUNTIME=${Boost_USE_DEBUG_RUNTIME}")
  return()
endif()

# threading=single

# variant=release

if(NOT "${Boost_USE_RELEASE_LIBS}" STREQUAL "" AND NOT Boost_USE_RELEASE_LIBS)
  _BOOST_SKIPPED("libboost_type_erasure.dylib" "release, Boost_USE_RELEASE_LIBS=${Boost_USE_RELEASE_LIBS}")
  return()
endif()

if(Boost_VERBOSE OR Boost_DEBUG)
  message(STATUS "  [x] libboost_type_erasure.dylib")
endif()

# Target file name: libboost_type_erasure.dylib

get_target_property(__boost_imploc Boost::type_erasure IMPORTED_LOCATION_RELEASE)
if(__boost_imploc)
  message(WARNING "Target Boost::type_erasure already has an imported location '${__boost_imploc}', which will be overwritten with '${_BOOST_LIBDIR}/libboost_type_erasure.dylib'")
endif()
unset(__boost_imploc)

set_property(TARGET Boost::type_erasure APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

set_target_properties(Boost::type_erasure PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE CXX
  IMPORTED_LOCATION_RELEASE "${_BOOST_LIBDIR}/libboost_type_erasure.dylib"
  )

set_target_properties(Boost::type_erasure PROPERTIES
  MAP_IMPORTED_CONFIG_MINSIZEREL Release
  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )

set_target_properties(Boost::type_erasure PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "BOOST_TYPE_ERASURE_DYN_LINK"
  )

list(APPEND _BOOST_TYPE_ERASURE_DEPS chrono system thread headers)
