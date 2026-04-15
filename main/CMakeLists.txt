set(NP2KAI_DIR "/root/projects/np2kai")

file(GLOB_RECURSE NP2KAI_SRCS "${NP2KAI_DIR}/*.c")

set(NP2KAI_EXCLUDE_PATTERNS
    ".*/sdl/.*"
    ".*/windows/.*"
    ".*/x/.*"
    ".*/jni/.*"
    ".*/vst3sdk/.*"
    ".*/cmake/.*"
    ".*/sample/.*"
    ".*/network/.*"
    ".*/accessories/.*"
    ".*/np2tool/.*"
    ".*/i386hax/.*"
    ".*/i286c/.*"
    ".*/i286x/.*"
    ".*/embed/.*"
    ".*/win9x/.*"
    ".*/debug.*"
    ".*/sound/mame/.*"
    ".*/fpu/fpemul\.c"
    ".*/fpu/fpemul_.*"
    ".*/fpu/softfloat/.*"
    ".*/fpu/softfloat3/.*"
    ".*/wab/.*"
    ".*/lio/.*"
    ".*/test_.*\\.c"
    ".*gen\\.c"
)

foreach(PATTERN ${NP2KAI_EXCLUDE_PATTERNS})
    list(FILTER NP2KAI_SRCS EXCLUDE REGEX "${PATTERN}")
endforeach()

set(PORT_SRCS
    "main.c"
    "np2_memory.c"
    "np2_display.c"
    "np2_dosio.c"
    "np2_timing.c"
    "np2_sound.c"
    "np2_input.c"
    "np2_stubs.c"
)

set(NP2KAI_INCLUDES
    "${NP2KAI_DIR}"
    "${NP2KAI_DIR}/i386c"
    "${NP2KAI_DIR}/i386c/ia32"
    "${NP2KAI_DIR}/i386c/ia32/instructions"
    "${NP2KAI_DIR}/i386c/ia32/instructions/fpu"
    "${NP2KAI_DIR}/bios"
    "${NP2KAI_DIR}/cbus"
    "${NP2KAI_DIR}/common"
    "${NP2KAI_DIR}/diskimage"
    "${NP2KAI_DIR}/diskimage/cd"
    "${NP2KAI_DIR}/diskimage/fd"
    "${NP2KAI_DIR}/fdd"
    "${NP2KAI_DIR}/font"
    "${NP2KAI_DIR}/generic"
    "${NP2KAI_DIR}/io"
    "${NP2KAI_DIR}/io/pci"
    "${NP2KAI_DIR}/mem"
    "${NP2KAI_DIR}/misc"
    "${NP2KAI_DIR}/sound"
    "${NP2KAI_DIR}/sound/getsnd"
    "${NP2KAI_DIR}/vram"
    "."
)

idf_component_register(
    SRCS ${PORT_SRCS} ${NP2KAI_SRCS}
    INCLUDE_DIRS ${NP2KAI_INCLUDES}
    REQUIRES
        driver
        esp_lcd
        fatfs
        sdmmc
        esp_driver_sdmmc
        nvs_flash
        esp_psram
        esp_hw_support
        freertos
        esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    SUPPORT_IA32
    SUPPORT_IA32_HAXM
    SUPPORT_PC9821
    SUPPORT_PEGC
    USE_CUSTOM_HOOKINST
    SUPPORT_NP2_TICKCOUNT
    USE_TSC
    NP2KAI_GIT_TAG="esp32p4-port"
    NP2KAI_GIT_HASH="0000000"
)

# Make NP2kai angle bracket includes work
target_include_directories(${COMPONENT_LIB} SYSTEM PRIVATE ${NP2KAI_INCLUDES})

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Wno-pointer-to-int-cast
    -Wno-int-to-pointer-cast
    -Wno-unused-value
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-const-variable
    -Wno-format
    -Wno-format-truncation
    -Wno-address
    -Wno-return-type
    -Wno-maybe-uninitialized
    -Wno-uninitialized
    -Wno-missing-braces
    -Wno-missing-field-initializers
    -Wno-unknown-pragmas
    -Wno-comment
    -Wno-parentheses
    -Wno-misleading-indentation
    -Wno-memset-elt-size
    -Wno-strict-aliasing
    -Wno-overflow
    -Wno-sizeof-pointer-memaccess
)


