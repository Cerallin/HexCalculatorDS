cmake_minimum_required(VERSION 4.0.2)
project(HexCalculatorDS VERSION 1.0.0 LANGUAGES C CXX)

# ----------------------------------------
# 基础路径
# ----------------------------------------

if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO not set")
endif()

set(DEVKITPRO $ENV{DEVKITPRO})
set(LIBNDS ${DEVKITPRO}/libnds)
set(CALICO ${DEVKITPRO}/calico)

# ----------------------------------------
# Source files
# ----------------------------------------

file(GLOB_RECURSE SOURCES
    source/*.c
    source/*.cc
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    include
    ${LIBNDS}/include
    ${CALICO}/include
)

# ----------------------------------------
# Compile Options
# ----------------------------------------

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    __NDS__
    ARM9
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=arm946e-s
    -mthumb
    -mthumb-interwork
    -O2
    -ffunction-sections
    -fdata-sections
    -fno-rtti
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=arm946e-s
    -mthumb
    -mthumb-interwork
    -Wl,--gc-sections
    -specs=${CALICO}/share/ds9.specs
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ----------------------------------------
# Libraries
# ----------------------------------------

target_link_directories(${PROJECT_NAME}.elf PRIVATE
    ${LIBNDS}/lib
    ${CALICO}/lib
)

target_link_libraries(${PROJECT_NAME}.elf
    nds9
    calico_ds9
)

# ----------------------------------------
# Finale NDS file
# ----------------------------------------

find_program(NDSTOOL ndstool REQUIRED)
find_program(GRIT grit REQUIRED)

set(NDS_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nds)

set(GAME_TITLE "Hex Calculator")
set(GAME_SUBTITLE1 "c 2026")
set(GAME_SUBTITLE2 "Cerallin")
set(GAME_ICON ${CMAKE_SOURCE_DIR}/icon.bmp)

add_custom_command(
    OUTPUT ${NDS_FILE}
    COMMAND ${NDSTOOL}
        -c ${NDS_FILE}
        -9 $<TARGET_FILE:${PROJECT_NAME}.elf>
        -7 ${CALICO}/bin/ds7_maine.elf
        -b ${GAME_ICON}
        "${GAME_TITLE}\\;${GAME_SUBTITLE1}\\;${GAME_SUBTITLE2}"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Generating NDS ROM"
)

add_custom_target(nds ALL DEPENDS ${NDS_FILE})
