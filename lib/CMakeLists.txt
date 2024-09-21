cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(argon
  #GIT_REPOSITORY https://github.com/stellar-aria/argon
  #GIT_TAG caa7cd8056351d4fea353c1e572fa62e04a49971
  URL https://github.com/stellar-aria/argon/archive/refs/tags/v0.1.0.tar.gz
  URL_HASH MD5=2cbc9a803e52ded736f1e28eca8b14aa
)
FetchContent_MakeAvailable(argon)

# tinyusb
FetchContent_Declare(tinyusb
  URL https://github.com/hathach/tinyusb/archive/refs/tags/0.17.0.tar.gz
)

add_library(tinyusb_config INTERFACE)
target_include_directories(tinyusb_config INTERFACE ${CMAKE_CURRENT_LIST_DIR}/tinyusb)

FetchContent_MakeAvailable(tinyusb)

add_library(tinyusb)
target_sources(tinyusb PRIVATE
    ${tinyusb_SOURCE_DIR}/src/tusb.c
    ${tinyusb_SOURCE_DIR}/src/common/tusb_fifo.c
    ${tinyusb_SOURCE_DIR}/src/device/usbd.c
    ${tinyusb_SOURCE_DIR}/src/device/usbd_control.c
    ${tinyusb_SOURCE_DIR}/src/class/midi/midi_device.c
)

target_include_directories(tinyusb PUBLIC
    ${tinyusb_SOURCE_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/../src/RZA1/system
    ${CMAKE_CURRENT_LIST_DIR}/../src/
)

target_link_libraries(tinyusb PUBLIC
    tinyusb_config
)

add_library(tinyusb-rusb2)
target_sources(tinyusb-rusb2 PRIVATE
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/dcd_rusb2.c
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/hcd_rusb2.c
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/rusb2_common.c
)
target_link_libraries(tinyusb-rusb2 PUBLIC
    tinyusb
    tinyusb_config
)
