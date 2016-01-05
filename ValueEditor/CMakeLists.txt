cmake_minimum_required(VERSION 2.6)
 
PROJECT(FileMenu)
 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

IF(APPLE)
  SET(CMAKE_CXX_FLAGS "-std=c++03 -stdlib=libstdc++")
  SET(CMAKE_EXE_LINKER_FLAGS "-stdlib=libstdc++")
ENDIF()

QT4_WRAP_CPP(MOCSrcs VETreeWidget.h BaseModelItem.h BaseViewItem.h ColorViewItem.h DoubleSlider.h FloatViewItem.h FloatSliderViewItem.h RTValViewItem.h Vec3ViewItem.h ViewItemChildRouter.h)
 
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${root_SOURCE_DIR}/include)

include_directories($ENV{FABRIC_DIR}/include)
link_directories($ENV{FABRIC_DIR}/lib)
add_definitions(-DFEC_SHARED)

file(GLOB AppSrcs Tests/SampleApp/*.cpp)
file(GLOB LibSrcs *.cpp)

ADD_EXECUTABLE(TestProject ${AppSrcs} ${LibSrcs} ${MOCSrcs})
TARGET_LINK_LIBRARIES(TestProject ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} FabricCore)

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(TestProject PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
elseif(UNIX)
  # Nothing special required
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()
