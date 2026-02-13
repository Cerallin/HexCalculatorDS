set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO not set")
endif()

if(NOT DEFINED ENV{DEVKITARM})
    message(FATAL_ERROR "DEVKITARM not set")
endif()

set(DEVKITPRO $ENV{DEVKITPRO})
set(DEVKITARM $ENV{DEVKITARM})

set(CMAKE_C_COMPILER   ${DEVKITARM}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITARM}/bin/arm-none-eabi-g++)
set(CMAKE_CPP_COMPILER ${DEVKITARM}/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${DEVKITARM}/bin/arm-none-eabi-gcc)

set(CMAKE_AR ${DEVKITARM}/bin/arm-none-eabi-ar)
set(CMAKE_OBJCOPY ${DEVKITARM}/bin/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${DEVKITARM}/bin/arm-none-eabi-objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
