#include "pebble_process_info.h"
#include "src/resource_ids.auto.h"

const PebbleProcessInfo __pbl_app_info __attribute__ ((section (".pbl_header"))) = {
  .header = "PBLAPP",
  .struct_version = { PROCESS_INFO_CURRENT_STRUCT_VERSION_MAJOR, PROCESS_INFO_CURRENT_STRUCT_VERSION_MINOR },
  .sdk_version = { PROCESS_INFO_CURRENT_SDK_VERSION_MAJOR, PROCESS_INFO_CURRENT_SDK_VERSION_MINOR },
  .process_version = { 2, 0 },
  .load_size = 0xb6b6,
  .offset = 0xb6b6b6b6,
  .crc = 0xb6b6b6b6,
  .name = "Misfit MC",
  .company = "hanamj@shaw.ca",
  .icon_resource_id = RESOURCE_ID_IMAGE_CREEP,
  .sym_table_addr = 0xA7A7A7A7,
  .flags = 0,
  .num_reloc_entries = 0xdeadcafe,
  .uuid = { 0x24, 0x5C, 0x4A, 0x36, 0xE0, 0x17, 0x43, 0x1A, 0x9F, 0x3D, 0x92, 0x66, 0xD3, 0xA9, 0xFC, 0x26 },
  .virtual_size = 0xb6b6
};
