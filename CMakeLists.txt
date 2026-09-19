cmake_minimum_required(VERSION 3.18)
project(HexCalculatorDS VERSION 1.3.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------
# Build types
# ----------------------------------------

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Wno-unused-variable -O2 -s -w -flto -ffast-math")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -g -O2 -flto -ffast-math")

# ----------------------------------------
# Options
# ----------------------------------------
set(THEME "light" CACHE STRING "Color theme (default: light)")

# Valid themes: light, dark
if (THEME STREQUAL "light")
    set(THEME_DEFINITION THEME_LIGHT)
elseif (THEME STREQUAL "dark")
    set(THEME_DEFINITION THEME_DARK)
else()
    message(FATAL_ERROR "Invalid theme: ${THEME}. Valid themes are: light, dark")
endif()

message(STATUS "Current theme: ${THEME}")

# ----------------------------------------
# Environments
# ----------------------------------------

if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO not set")
endif()

set(DEVKITPRO $ENV{DEVKITPRO})
set(LIBNDS ${DEVKITPRO}/libnds)

find_program(GRIT grit REQUIRED)
find_program(PYTHON python3 REQUIRED)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/project.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/project.h
    @ONLY
)

# ----------------------------------------
# Font tiles
# ----------------------------------------

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mainFont.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-main-tilemap.png
        -ftc
        -gB4
        -gzl
        -gTFF00FF
        -m
        -o mainFont
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-main-tilemap.png
    COMMENT "Generating main font tilemap"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subFont.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-tilemap.png
        -ftc
        -gB4
        -gzl
        -gTFF00FF
        -m
        -o subFont
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-tilemap.png
    COMMENT "Generating sub font tilemap"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subBinaryFont.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-binary-tilemap.png
        -ftc
        -gB4
        -gzl
        -gTFF00FF
        -m
        -o subBinaryFont
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-binary-tilemap.png
    COMMENT "Generating sub binary font tilemap"
)

# Last 8x8 of binary tilemap; -p! shares palette with subFont / theme subPal
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subDigitFocus.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-binary-tilemap.png
        -ftc
        -gB4
        -gTFF00FF
        -m
        -p!
        -al384
        -aw8
        -at0
        -ah8
        -o subDigitFocus
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-binary-tilemap.png
    COMMENT "Generating sub digit focus tile (shared palette)"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subNumber.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-numbers.png
        -ftc
        -gB4
        -gzl
        -gTFF00FF
        -m
        -o subNumber
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-numbers.png
    COMMENT "Generating sub number tilemap"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subVersion.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-version.png
        -ftc
        -gB4
        -gzl
        -gTFF00FF
        -m
        -p!
        -o subVersion
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tiles/HexCalculatorDS-sub-version.png
    COMMENT "Generating sub version tilemap (shared palette with subNumber)"
)

# ----------------------------------------
# Subscreen Image
# ----------------------------------------

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subscreenArea.bmp
    COMMAND ${PYTHON}
        ${CMAKE_CURRENT_SOURCE_DIR}/subscreen_maker/run.py
        ${THEME}
        ${CMAKE_CURRENT_SOURCE_DIR}/design/HexCalculatorDS-sub.png
        ${CMAKE_CURRENT_BINARY_DIR}/subscreenArea.bmp
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/design/HexCalculatorDS-sub.png
        ${CMAKE_CURRENT_SOURCE_DIR}/subscreen_maker/run.py
    COMMENT "Generating subscreen area BMP"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subscreenImage.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_BINARY_DIR}/subscreenArea.bmp
        -ftc
        -gB8
        -gb
        -gzl
        -gTFF00FF
        -m
        -o subscreenImage
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/subscreenArea.bmp
    COMMENT "Generating subscreen image"
)

# ----------------------------------------
# Subscreen Binary Image
# ----------------------------------------

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryArea.bmp
    COMMAND ${PYTHON}
        ${CMAKE_CURRENT_SOURCE_DIR}/subscreen_maker/run_binary.py
        ${THEME}
        ${CMAKE_CURRENT_SOURCE_DIR}/design/HexCalculatorDS-binary.png
        ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryArea.bmp
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/design/HexCalculatorDS-binary.png
        ${CMAKE_CURRENT_SOURCE_DIR}/subscreen_maker/run_binary.py
    COMMENT "Generating subscreen binary area BMP"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryImage.c
    COMMAND ${GRIT}
        ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryArea.bmp
        -ftc
        -gB8
        -gb
        -gzl
        -gTFF00FF
        -m
        -o subscreenBinaryImage
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryArea.bmp
    COMMENT "Generating subscreen binary image"
)

# ----------------------------------------
# ARM7 (minimal: no WiFi / maxmod)
# ----------------------------------------

add_executable(${PROJECT_NAME}.arm7.elf
    arm7/source/main.c
)

target_include_directories(${PROJECT_NAME}.arm7.elf PRIVATE
    ${LIBNDS}/include
)

target_compile_definitions(${PROJECT_NAME}.arm7.elf PRIVATE
    ARM7
)

target_compile_options(${PROJECT_NAME}.arm7.elf PRIVATE
    -mcpu=arm7tdmi
    -mtune=arm7tdmi
    -mthumb
    -mthumb-interwork
    -ffunction-sections
    -fdata-sections
    -fomit-frame-pointer
    -ffast-math
)

target_link_options(${PROJECT_NAME}.arm7.elf PRIVATE
    -mcpu=arm7tdmi
    -mthumb
    -mthumb-interwork
    -Wl,--gc-sections
    -Wl,--nmagic
    -specs=ds_arm7.specs
)

target_link_directories(${PROJECT_NAME}.arm7.elf PRIVATE
    ${LIBNDS}/lib
)

target_link_libraries(${PROJECT_NAME}.arm7.elf
    nds7
)

# ----------------------------------------
# ARM9 source files
# ----------------------------------------

file(GLOB_RECURSE SOURCES
    source/*.c
    source/*.cc
)

add_executable(${PROJECT_NAME}.arm9.elf 
    ${SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/mainFont.c
    ${CMAKE_CURRENT_BINARY_DIR}/subFont.c
    ${CMAKE_CURRENT_BINARY_DIR}/subBinaryFont.c
    ${CMAKE_CURRENT_BINARY_DIR}/subDigitFocus.c
    ${CMAKE_CURRENT_BINARY_DIR}/subNumber.c
    ${CMAKE_CURRENT_BINARY_DIR}/subVersion.c
    ${CMAKE_CURRENT_BINARY_DIR}/subscreenImage.c
    ${CMAKE_CURRENT_BINARY_DIR}/subscreenBinaryImage.c
)

target_include_directories(${PROJECT_NAME}.arm9.elf PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIBNDS}/include
)

# ----------------------------------------
# ARM9 Compile Options
# ----------------------------------------

target_compile_definitions(${PROJECT_NAME}.arm9.elf PRIVATE
    ARM9
)

target_compile_options(${PROJECT_NAME}.arm9.elf PRIVATE
    -mcpu=arm946e-s
    -mthumb
    -mthumb-interwork
    -ffunction-sections
    -fdata-sections
    
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-asynchronous-unwind-tables>
)

target_link_options(${PROJECT_NAME}.arm9.elf PRIVATE
    -mcpu=arm946e-s
    -mthumb
    -mthumb-interwork
    -Wl,--gc-sections
    -specs=ds_arm9.specs
)

set_target_properties(${PROJECT_NAME}.arm9.elf PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ----------------------------------------
# ARM9 Libraries
# ----------------------------------------

target_link_directories(${PROJECT_NAME}.arm9.elf PRIVATE
    ${LIBNDS}/lib
)

target_link_libraries(${PROJECT_NAME}.arm9.elf
    nds9
)

# ----------------------------------------
# Finale NDS file
# ----------------------------------------

find_program(NDSTOOL ndstool REQUIRED)

set(NDS_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nds)

set(GAME_TITLE "HexCalcDS")
set(GAME_AUTHOR "Cerallin")
set(GAME_ICON ${CMAKE_SOURCE_DIR}/design/HexCalculatorDS-icon.bmp)

add_custom_command(
    OUTPUT ${NDS_FILE}
    COMMAND ${NDSTOOL}
        -c ${NDS_FILE}
        -7 $<TARGET_FILE:${PROJECT_NAME}.arm7.elf>
        -9 $<TARGET_FILE:${PROJECT_NAME}.arm9.elf>
        -b ${GAME_ICON} "${GAME_TITLE}\;v${PROJECT_VERSION}\;${GAME_AUTHOR}"
        -g CALC
        -n 2 0
        >/dev/null
    DEPENDS ${PROJECT_NAME}.arm7.elf ${PROJECT_NAME}.arm9.elf
    COMMENT "Generating NDS ROM"
)

add_custom_target(nds ALL DEPENDS ${NDS_FILE})

# ----------------------------------------
# clangd: drop grit .c from compile_commands.json
# ----------------------------------------
# Those entries make clangd treat project headers as C (no libstdc++), which
# breaks <array> / #include_next <math.h>. Re-run after the DB is refreshed.
add_custom_target(filter_compile_commands ALL
    COMMAND ${CMAKE_COMMAND}
        -DCOMPILE_COMMANDS=${CMAKE_BINARY_DIR}/compile_commands.json
        -P ${CMAKE_SOURCE_DIR}/cmake/filter-compile-commands.cmake
    COMMENT "Filtering .c entries from compile_commands.json for clangd"
    VERBATIM
)
