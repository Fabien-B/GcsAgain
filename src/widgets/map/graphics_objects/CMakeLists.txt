
set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_line.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_circle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_aircraft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_track.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_text.cpp
    
)

set(INC_DIRS ${INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE ${SOURCE}  PARENT_SCOPE)
set(INC_DIRS ${INC_DIRS}  PARENT_SCOPE)

