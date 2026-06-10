cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(test C ASM)

set(MCU_FLAGS "-mcpu=cortex-m3 -mthumb")

set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nano.specs -T${CMAKE_SOURCE_DIR}/STM32F103C8_FLASH.ld -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map")

add_definitions(-DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER)

include_directories(
    SYSTEM/sys
    SYSTEM/delay
    SYSTEM/usart
    HARDWARE/LCD
    HARDWARE/TOUCH
    HARDWARE/TEST
    HARDWARE/UI
)

set(SOURCES
    USER/startup_stm32f10x_hd.s
    USER/test.c
    SYSTEM/sys/sys.c
    SYSTEM/delay/delay.c
    SYSTEM/usart/usart.c
    HARDWARE/LCD/LCD.c
    HARDWARE/TOUCH/ctp.c
    HARDWARE/TOUCH/ctiic.c
    HARDWARE/TOUCH/touch.c
    "HARDWARE/TEST/test code.c"
    HARDWARE/UI/ui_main.c
    HARDWARE/UI/ui_timing_menu.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}.elf>
)
