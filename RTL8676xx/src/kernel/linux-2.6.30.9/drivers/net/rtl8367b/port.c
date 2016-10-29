#include <rtl8367b_asicdrv_acl.h>
#include <rtl8367b_asicdrv.h>
#include <rtl8367b_asicdrv_dot1x.h>
#include <rtl8367b_asicdrv_qos.h>
#include <rtl8367b_asicdrv_scheduling.h>
#include <rtl8367b_asicdrv_fc.h>
#include <rtl8367b_asicdrv_port.h>
#include <rtl8367b_asicdrv_phy.h>
#include <rtl8367b_asicdrv_igmp.h>
#include <rtl8367b_asicdrv_unknownMulticast.h>
#include <rtl8367b_asicdrv_rma.h>
#include <rtl8367b_asicdrv_vlan.h>
#include <rtl8367b_asicdrv_lut.h>
#include <rtl8367b_asicdrv_led.h>
#include <rtl8367b_asicdrv_svlan.h>
#include <rtl8367b_asicdrv_meter.h>
#include <rtl8367b_asicdrv_inbwctrl.h>
#include <rtl8367b_asicdrv_storm.h>
#include <rtl8367b_asicdrv_misc.h>
#include <rtl8367b_asicdrv_portIsolation.h>
#include <rtl8367b_asicdrv_cputag.h>
#include <rtl8367b_asicdrv_trunking.h>
#include <rtl8367b_asicdrv_mirror.h>
#include <rtl8367b_asicdrv_mib.h>
#include <rtl8367b_asicdrv_interrupt.h>
#include <rtl8367b_asicdrv_green.h>
#include <rtl8367b_asicdrv_eee.h>
#include <rtl8367b_asicdrv_eav.h>
#include <rtl8367b_asicdrv_hsb.h>
#include <smi.h>

#include <rtk_api.h>

#include <rtk_error.h>

//#include <string.h>
#include <linux/slab.h>

rtk_uint16      (*init_para)[2];
rtk_uint16      init_size;
#define MDC_MDIO_OPERATION

#if defined(CHIP_RTL8367RB) || defined(RTK_X86_ASICDRV) || defined(RTK_ASICDRV_INIT)
rtk_uint16 ChipData30[][2]= {
/*Code of Func*/
{0x1B03, 0x0876}, {0x1200, 0x7FC4}, {0x0301, 0x0026}, {0x1722, 0x0E14},
{0x205F, 0x0002}, {0x2059, 0x1A00}, {0x205F, 0x0000}, {0x207F, 0x0002},
{0x2077, 0x0000}, {0x2078, 0x0000}, {0x2079, 0x0000}, {0x207A, 0x0000},
{0x207B, 0x0000}, {0x207F, 0x0000}, {0x205F, 0x0002}, {0x2053, 0x0000},
{0x2054, 0x0000}, {0x2055, 0x0000}, {0x2056, 0x0000}, {0x2057, 0x0000},
{0x205F, 0x0000}, {0x12A4, 0x110A}, {0x12A6, 0x150A}, {0x13F1, 0x0013},
{0x13F4, 0x0010}, {0x13F5, 0x0000}, {0x0018, 0x0F00}, {0x0038, 0x0F00},
{0x0058, 0x0F00}, {0x0078, 0x0F00}, {0x0098, 0x0F00}, {0x12B6, 0x0C02},
{0x12B7, 0x030F}, {0x12B8, 0x11FF}, {0x12BC, 0x0004}, {0x1362, 0x0115},
{0x1363, 0x0002}, {0x1363, 0x0000}, {0x133F, 0x0030}, {0x133E, 0x000E},
{0x221F, 0x0007}, {0x221E, 0x002D}, {0x2218, 0xF030}, {0x221F, 0x0007},
{0x221E, 0x0023}, {0x2216, 0x0005}, {0x2215, 0x00B9}, {0x2219, 0x0044},
{0x2215, 0x00BA}, {0x2219, 0x0020}, {0x2215, 0x00BB}, {0x2219, 0x00C1},
{0x2215, 0x0148}, {0x2219, 0x0096}, {0x2215, 0x016E}, {0x2219, 0x0026},
{0x2216, 0x0000}, {0x2216, 0x0000}, {0x221E, 0x002D}, {0x2218, 0xF010},
{0x221F, 0x0007}, {0x221E, 0x0020}, {0x2215, 0x0D00}, {0x221F, 0x0000},
{0x221F, 0x0000}, {0x2217, 0x2160}, {0x221F, 0x0001}, {0x2210, 0xF25E},
{0x221F, 0x0007}, {0x221E, 0x0042}, {0x2215, 0x0F00}, {0x2215, 0x0F00},
{0x2216, 0x7408}, {0x2215, 0x0E00}, {0x2215, 0x0F00}, {0x2215, 0x0F01},
{0x2216, 0x4000}, {0x2215, 0x0E01}, {0x2215, 0x0F01}, {0x2215, 0x0F02},
{0x2216, 0x9400}, {0x2215, 0x0E02}, {0x2215, 0x0F02}, {0x2215, 0x0F03},
{0x2216, 0x7408}, {0x2215, 0x0E03}, {0x2215, 0x0F03}, {0x2215, 0x0F04},
{0x2216, 0x4008}, {0x2215, 0x0E04}, {0x2215, 0x0F04}, {0x2215, 0x0F05},
{0x2216, 0x9400}, {0x2215, 0x0E05}, {0x2215, 0x0F05}, {0x2215, 0x0F06},
{0x2216, 0x0803}, {0x2215, 0x0E06}, {0x2215, 0x0F06}, {0x2215, 0x0D00},
{0x2215, 0x0100}, {0x221F, 0x0001}, {0x2210, 0xF05E}, {0x221F, 0x0000},
{0x2217, 0x2100}, {0x221F, 0x0000}, {0x220D, 0x0003}, {0x220E, 0x0015},
{0x220D, 0x4003}, {0x220E, 0x0006}, {0x221F, 0x0000}, {0x2200, 0x1340},
{0x133F, 0x0010}, {0x12A0, 0x0058}, {0x12A1, 0x0058}, {0x133E, 0x000E},
{0x133F, 0x0030}, {0x221F, 0x0000}, {0x2210, 0x0166}, {0x221F, 0x0000},
{0x133E, 0x000E}, {0x133F, 0x0010}, {0x133F, 0x0030}, {0x133E, 0x000E},
{0x221F, 0x0005}, {0x2205, 0xFFF6}, {0x2206, 0x0080}, {0x2205, 0x8B6E},
{0x2206, 0x0000}, {0x220F, 0x0100}, {0x2205, 0x8000}, {0x2206, 0x0280},
{0x2206, 0x28F7}, {0x2206, 0x00E0}, {0x2206, 0xFFF7}, {0x2206, 0xA080},
{0x2206, 0x02AE}, {0x2206, 0xF602}, {0x2206, 0x0153}, {0x2206, 0x0201},
{0x2206, 0x6602}, {0x2206, 0x80B9}, {0x2206, 0xE08B}, {0x2206, 0x8CE1},
{0x2206, 0x8B8D}, {0x2206, 0x1E01}, {0x2206, 0xE18B}, {0x2206, 0x8E1E},
{0x2206, 0x01A0}, {0x2206, 0x00E7}, {0x2206, 0xAEDB}, {0x2206, 0xEEE0},
{0x2206, 0x120E}, {0x2206, 0xEEE0}, {0x2206, 0x1300}, {0x2206, 0xEEE0},
{0x2206, 0x2001}, {0x2206, 0xEEE0}, {0x2206, 0x2166}, {0x2206, 0xEEE0},
{0x2206, 0xC463}, {0x2206, 0xEEE0}, {0x2206, 0xC5E8}, {0x2206, 0xEEE0},
{0x2206, 0xC699}, {0x2206, 0xEEE0}, {0x2206, 0xC7C2}, {0x2206, 0xEEE0},
{0x2206, 0xC801}, {0x2206, 0xEEE0}, {0x2206, 0xC913}, {0x2206, 0xEEE0},
{0x2206, 0xCA30}, {0x2206, 0xEEE0}, {0x2206, 0xCB3E}, {0x2206, 0xEEE0},
{0x2206, 0xDCE1}, {0x2206, 0xEEE0}, {0x2206, 0xDD00}, {0x2206, 0xEEE2},
{0x2206, 0x0001}, {0x2206, 0xEEE2}, {0x2206, 0x0100}, {0x2206, 0xEEE4},
{0x2206, 0x8860}, {0x2206, 0xEEE4}, {0x2206, 0x8902}, {0x2206, 0xEEE4},
{0x2206, 0x8C00}, {0x2206, 0xEEE4}, {0x2206, 0x8D30}, {0x2206, 0xEEEA},
{0x2206, 0x1480}, {0x2206, 0xEEEA}, {0x2206, 0x1503}, {0x2206, 0xEEEA},
{0x2206, 0xC600}, {0x2206, 0xEEEA}, {0x2206, 0xC706}, {0x2206, 0xEE85},
{0x2206, 0xEE00}, {0x2206, 0xEE85}, {0x2206, 0xEF00}, {0x2206, 0xEE8B},
{0x2206, 0x6750}, {0x2206, 0xEE8B}, {0x2206, 0x6632}, {0x2206, 0xEE8A},
{0x2206, 0xD448}, {0x2206, 0xEE8A}, {0x2206, 0xD548}, {0x2206, 0xEE8A},
{0x2206, 0xD649}, {0x2206, 0xEE8A}, {0x2206, 0xD7F8}, {0x2206, 0xEE8B},
{0x2206, 0x85E2}, {0x2206, 0xEE8B}, {0x2206, 0x8700}, {0x2206, 0xEEFF},
{0x2206, 0xF600}, {0x2206, 0xEEFF}, {0x2206, 0xF7FC}, {0x2206, 0x04F8},
{0x2206, 0xE08B}, {0x2206, 0x8EAD}, {0x2206, 0x2023}, {0x2206, 0xF620},
{0x2206, 0xE48B}, {0x2206, 0x8E02}, {0x2206, 0x2877}, {0x2206, 0x0225},
{0x2206, 0xC702}, {0x2206, 0x26A1}, {0x2206, 0x0281}, {0x2206, 0xB302},
{0x2206, 0x8496}, {0x2206, 0x0202}, {0x2206, 0xA102}, {0x2206, 0x27F1},
{0x2206, 0x0228}, {0x2206, 0xF902}, {0x2206, 0x2AA0}, {0x2206, 0x0282},
{0x2206, 0xB8E0}, {0x2206, 0x8B8E}, {0x2206, 0xAD21}, {0x2206, 0x08F6},
{0x2206, 0x21E4}, {0x2206, 0x8B8E}, {0x2206, 0x0202}, {0x2206, 0x80E0},
{0x2206, 0x8B8E}, {0x2206, 0xAD22}, {0x2206, 0x05F6}, {0x2206, 0x22E4},
{0x2206, 0x8B8E}, {0x2206, 0xE08B}, {0x2206, 0x8EAD}, {0x2206, 0x2305},
{0x2206, 0xF623}, {0x2206, 0xE48B}, {0x2206, 0x8EE0}, {0x2206, 0x8B8E},
{0x2206, 0xAD24}, {0x2206, 0x08F6}, {0x2206, 0x24E4}, {0x2206, 0x8B8E},
{0x2206, 0x0227}, {0x2206, 0x6AE0}, {0x2206, 0x8B8E}, {0x2206, 0xAD25},
{0x2206, 0x05F6}, {0x2206, 0x25E4}, {0x2206, 0x8B8E}, {0x2206, 0xE08B},
{0x2206, 0x8EAD}, {0x2206, 0x260B}, {0x2206, 0xF626}, {0x2206, 0xE48B},
{0x2206, 0x8E02}, {0x2206, 0x830D}, {0x2206, 0x021D}, {0x2206, 0x6BE0},
{0x2206, 0x8B8E}, {0x2206, 0xAD27}, {0x2206, 0x05F6}, {0x2206, 0x27E4},
{0x2206, 0x8B8E}, {0x2206, 0x0281}, {0x2206, 0x4402}, {0x2206, 0x045C},
{0x2206, 0xFC04}, {0x2206, 0xF8E0}, {0x2206, 0x8B83}, {0x2206, 0xAD23},
{0x2206, 0x30E0}, {0x2206, 0xE022}, {0x2206, 0xE1E0}, {0x2206, 0x2359},
{0x2206, 0x02E0}, {0x2206, 0x85EF}, {0x2206, 0xE585}, {0x2206, 0xEFAC},
{0x2206, 0x2907}, {0x2206, 0x1F01}, {0x2206, 0x9E51}, {0x2206, 0xAD29},
{0x2206, 0x20E0}, {0x2206, 0x8B83}, {0x2206, 0xAD21}, {0x2206, 0x06E1},
{0x2206, 0x8B84}, {0x2206, 0xAD28}, {0x2206, 0x42E0}, {0x2206, 0x8B85},
{0x2206, 0xAD21}, {0x2206, 0x06E1}, {0x2206, 0x8B84}, {0x2206, 0xAD29},
{0x2206, 0x36BF}, {0x2206, 0x34BF}, {0x2206, 0x022C}, {0x2206, 0x31AE},
{0x2206, 0x2EE0}, {0x2206, 0x8B83}, {0x2206, 0xAD21}, {0x2206, 0x10E0},
{0x2206, 0x8B84}, {0x2206, 0xF620}, {0x2206, 0xE48B}, {0x2206, 0x84EE},
{0x2206, 0x8ADA}, {0x2206, 0x00EE}, {0x2206, 0x8ADB}, {0x2206, 0x00E0},
{0x2206, 0x8B85}, {0x2206, 0xAD21}, {0x2206, 0x0CE0}, {0x2206, 0x8B84},
{0x2206, 0xF621}, {0x2206, 0xE48B}, {0x2206, 0x84EE}, {0x2206, 0x8B72},
{0x2206, 0xFFBF}, {0x2206, 0x34C2}, {0x2206, 0x022C}, {0x2206, 0x31FC},
{0x2206, 0x04F8}, {0x2206, 0xFAEF}, {0x2206, 0x69E0}, {0x2206, 0x8B85},
{0x2206, 0xAD21}, {0x2206, 0x42E0}, {0x2206, 0xE022}, {0x2206, 0xE1E0},
{0x2206, 0x2358}, {0x2206, 0xC059}, {0x2206, 0x021E}, {0x2206, 0x01E1},
{0x2206, 0x8B72}, {0x2206, 0x1F10}, {0x2206, 0x9E2F}, {0x2206, 0xE48B},
{0x2206, 0x72AD}, {0x2206, 0x2123}, {0x2206, 0xE18B}, {0x2206, 0x84F7},
{0x2206, 0x29E5}, {0x2206, 0x8B84}, {0x2206, 0xAC27}, {0x2206, 0x10AC},
{0x2206, 0x2605}, {0x2206, 0x0205}, {0x2206, 0x23AE}, {0x2206, 0x1602},
{0x2206, 0x0535}, {0x2206, 0x0282}, {0x2206, 0x30AE}, {0x2206, 0x0E02},
{0x2206, 0x056A}, {0x2206, 0x0282}, {0x2206, 0x75AE}, {0x2206, 0x0602},
{0x2206, 0x04DC}, {0x2206, 0x0282}, {0x2206, 0x04EF}, {0x2206, 0x96FE},
{0x2206, 0xFC04}, {0x2206, 0xF8F9}, {0x2206, 0xE08B}, {0x2206, 0x87AD},
{0x2206, 0x2321}, {0x2206, 0xE0EA}, {0x2206, 0x14E1}, {0x2206, 0xEA15},
{0x2206, 0xAD26}, {0x2206, 0x18F6}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
{0x2206, 0xE5EA}, {0x2206, 0x15F6}, {0x2206, 0x26E4}, {0x2206, 0xEA14},
{0x2206, 0xE5EA}, {0x2206, 0x15F7}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
{0x2206, 0xE5EA}, {0x2206, 0x15FD}, {0x2206, 0xFC04}, {0x2206, 0xF8F9},
{0x2206, 0xE08B}, {0x2206, 0x87AD}, {0x2206, 0x233A}, {0x2206, 0xAD22},
{0x2206, 0x37E0}, {0x2206, 0xE020}, {0x2206, 0xE1E0}, {0x2206, 0x21AC},
{0x2206, 0x212E}, {0x2206, 0xE0EA}, {0x2206, 0x14E1}, {0x2206, 0xEA15},
{0x2206, 0xF627}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
{0x2206, 0xE2EA}, {0x2206, 0x12E3}, {0x2206, 0xEA13}, {0x2206, 0x5A8F},
{0x2206, 0x6A20}, {0x2206, 0xE6EA}, {0x2206, 0x12E7}, {0x2206, 0xEA13},
{0x2206, 0xF726}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
{0x2206, 0xF727}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
{0x2206, 0xFDFC}, {0x2206, 0x04F8}, {0x2206, 0xF9E0}, {0x2206, 0x8B87},
{0x2206, 0xAD23}, {0x2206, 0x38AD}, {0x2206, 0x2135}, {0x2206, 0xE0E0},
{0x2206, 0x20E1}, {0x2206, 0xE021}, {0x2206, 0xAC21}, {0x2206, 0x2CE0},
{0x2206, 0xEA14}, {0x2206, 0xE1EA}, {0x2206, 0x15F6}, {0x2206, 0x27E4},
{0x2206, 0xEA14}, {0x2206, 0xE5EA}, {0x2206, 0x15E2}, {0x2206, 0xEA12},
{0x2206, 0xE3EA}, {0x2206, 0x135A}, {0x2206, 0x8FE6}, {0x2206, 0xEA12},
{0x2206, 0xE7EA}, {0x2206, 0x13F7}, {0x2206, 0x26E4}, {0x2206, 0xEA14},
{0x2206, 0xE5EA}, {0x2206, 0x15F7}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
{0x2206, 0xE5EA}, {0x2206, 0x15FD}, {0x2206, 0xFC04}, {0x2206, 0xF8FA},
{0x2206, 0xEF69}, {0x2206, 0xE08B}, {0x2206, 0x86AD}, {0x2206, 0x2146},
{0x2206, 0xE0E0}, {0x2206, 0x22E1}, {0x2206, 0xE023}, {0x2206, 0x58C0},
{0x2206, 0x5902}, {0x2206, 0x1E01}, {0x2206, 0xE18B}, {0x2206, 0x651F},
{0x2206, 0x109E}, {0x2206, 0x33E4}, {0x2206, 0x8B65}, {0x2206, 0xAD21},
{0x2206, 0x22AD}, {0x2206, 0x272A}, {0x2206, 0xD400}, {0x2206, 0x01BF},
{0x2206, 0x34F2}, {0x2206, 0x022C}, {0x2206, 0xA2BF}, {0x2206, 0x34F5},
{0x2206, 0x022C}, {0x2206, 0xE0E0}, {0x2206, 0x8B67}, {0x2206, 0x1B10},
{0x2206, 0xAA14}, {0x2206, 0xE18B}, {0x2206, 0x660D}, {0x2206, 0x1459},
{0x2206, 0x0FAE}, {0x2206, 0x05E1}, {0x2206, 0x8B66}, {0x2206, 0x590F},
{0x2206, 0xBF85}, {0x2206, 0x6102}, {0x2206, 0x2CA2}, {0x2206, 0xEF96},
{0x2206, 0xFEFC}, {0x2206, 0x04F8}, {0x2206, 0xF9FA}, {0x2206, 0xFBEF},
{0x2206, 0x79E2}, {0x2206, 0x8AD2}, {0x2206, 0xAC19}, {0x2206, 0x2DE0},
{0x2206, 0xE036}, {0x2206, 0xE1E0}, {0x2206, 0x37EF}, {0x2206, 0x311F},
{0x2206, 0x325B}, {0x2206, 0x019E}, {0x2206, 0x1F7A}, {0x2206, 0x0159},
{0x2206, 0x019F}, {0x2206, 0x0ABF}, {0x2206, 0x348E}, {0x2206, 0x022C},
{0x2206, 0x31F6}, {0x2206, 0x06AE}, {0x2206, 0x0FF6}, {0x2206, 0x0302},
{0x2206, 0x0470}, {0x2206, 0xF703}, {0x2206, 0xF706}, {0x2206, 0xBF34},
{0x2206, 0x9302}, {0x2206, 0x2C31}, {0x2206, 0xAC1A}, {0x2206, 0x25E0},
{0x2206, 0xE022}, {0x2206, 0xE1E0}, {0x2206, 0x23EF}, {0x2206, 0x300D},
{0x2206, 0x311F}, {0x2206, 0x325B}, {0x2206, 0x029E}, {0x2206, 0x157A},
{0x2206, 0x0258}, {0x2206, 0xC4A0}, {0x2206, 0x0408}, {0x2206, 0xBF34},
{0x2206, 0x9E02}, {0x2206, 0x2C31}, {0x2206, 0xAE06}, {0x2206, 0xBF34},
{0x2206, 0x9C02}, {0x2206, 0x2C31}, {0x2206, 0xAC1B}, {0x2206, 0x4AE0},
{0x2206, 0xE012}, {0x2206, 0xE1E0}, {0x2206, 0x13EF}, {0x2206, 0x300D},
{0x2206, 0x331F}, {0x2206, 0x325B}, {0x2206, 0x1C9E}, {0x2206, 0x3AEF},
{0x2206, 0x325B}, {0x2206, 0x1C9F}, {0x2206, 0x09BF}, {0x2206, 0x3498},
{0x2206, 0x022C}, {0x2206, 0x3102}, {0x2206, 0x83C5}, {0x2206, 0x5A03},
{0x2206, 0x0D03}, {0x2206, 0x581C}, {0x2206, 0x1E20}, {0x2206, 0x0207},
{0x2206, 0xA0A0}, {0x2206, 0x000E}, {0x2206, 0x0284}, {0x2206, 0x17AD},
{0x2206, 0x1817}, {0x2206, 0xBF34}, {0x2206, 0x9A02}, {0x2206, 0x2C31},
{0x2206, 0xAE0F}, {0x2206, 0xBF34}, {0x2206, 0xC802}, {0x2206, 0x2C31},
{0x2206, 0xBF34}, {0x2206, 0xC502}, {0x2206, 0x2C31}, {0x2206, 0x0284},
{0x2206, 0x52E6}, {0x2206, 0x8AD2}, {0x2206, 0xEF97}, {0x2206, 0xFFFE},
{0x2206, 0xFDFC}, {0x2206, 0x04F8}, {0x2206, 0xBF34}, {0x2206, 0xDA02},
{0x2206, 0x2CE0}, {0x2206, 0xE58A}, {0x2206, 0xD3BF}, {0x2206, 0x34D4},
{0x2206, 0x022C}, {0x2206, 0xE00C}, {0x2206, 0x1159}, {0x2206, 0x02E0},
{0x2206, 0x8AD3}, {0x2206, 0x1E01}, {0x2206, 0xE48A}, {0x2206, 0xD3D1},
{0x2206, 0x00BF}, {0x2206, 0x34DA}, {0x2206, 0x022C}, {0x2206, 0xA2D1},
{0x2206, 0x01BF}, {0x2206, 0x34D4}, {0x2206, 0x022C}, {0x2206, 0xA2BF},
{0x2206, 0x34CB}, {0x2206, 0x022C}, {0x2206, 0xE0E5}, {0x2206, 0x8ACE},
{0x2206, 0xBF85}, {0x2206, 0x6702}, {0x2206, 0x2CE0}, {0x2206, 0xE58A},
{0x2206, 0xCFBF}, {0x2206, 0x8564}, {0x2206, 0x022C}, {0x2206, 0xE0E5},
{0x2206, 0x8AD0}, {0x2206, 0xBF85}, {0x2206, 0x6A02}, {0x2206, 0x2CE0},
{0x2206, 0xE58A}, {0x2206, 0xD1FC}, {0x2206, 0x04F8}, {0x2206, 0xE18A},
{0x2206, 0xD1BF}, {0x2206, 0x856A}, {0x2206, 0x022C}, {0x2206, 0xA2E1},
{0x2206, 0x8AD0}, {0x2206, 0xBF85}, {0x2206, 0x6402}, {0x2206, 0x2CA2},
{0x2206, 0xE18A}, {0x2206, 0xCFBF}, {0x2206, 0x8567}, {0x2206, 0x022C},
{0x2206, 0xA2E1}, {0x2206, 0x8ACE}, {0x2206, 0xBF34}, {0x2206, 0xCB02},
{0x2206, 0x2CA2}, {0x2206, 0xE18A}, {0x2206, 0xD3BF}, {0x2206, 0x34DA},
{0x2206, 0x022C}, {0x2206, 0xA2E1}, {0x2206, 0x8AD3}, {0x2206, 0x0D11},
{0x2206, 0xBF34}, {0x2206, 0xD402}, {0x2206, 0x2CA2}, {0x2206, 0xFC04},
{0x2206, 0xF9A0}, {0x2206, 0x0405}, {0x2206, 0xE38A}, {0x2206, 0xD4AE},
{0x2206, 0x13A0}, {0x2206, 0x0805}, {0x2206, 0xE38A}, {0x2206, 0xD5AE},
{0x2206, 0x0BA0}, {0x2206, 0x0C05}, {0x2206, 0xE38A}, {0x2206, 0xD6AE},
{0x2206, 0x03E3}, {0x2206, 0x8AD7}, {0x2206, 0xEF13}, {0x2206, 0xBF34},
{0x2206, 0xCB02}, {0x2206, 0x2CA2}, {0x2206, 0xEF13}, {0x2206, 0x0D11},
{0x2206, 0xBF85}, {0x2206, 0x6702}, {0x2206, 0x2CA2}, {0x2206, 0xEF13},
{0x2206, 0x0D14}, {0x2206, 0xBF85}, {0x2206, 0x6402}, {0x2206, 0x2CA2},
{0x2206, 0xEF13}, {0x2206, 0x0D17}, {0x2206, 0xBF85}, {0x2206, 0x6A02},
{0x2206, 0x2CA2}, {0x2206, 0xFD04}, {0x2206, 0xF8E0}, {0x2206, 0x8B85},
{0x2206, 0xAD27}, {0x2206, 0x2DE0}, {0x2206, 0xE036}, {0x2206, 0xE1E0},
{0x2206, 0x37E1}, {0x2206, 0x8B73}, {0x2206, 0x1F10}, {0x2206, 0x9E20},
{0x2206, 0xE48B}, {0x2206, 0x73AC}, {0x2206, 0x200B}, {0x2206, 0xAC21},
{0x2206, 0x0DAC}, {0x2206, 0x250F}, {0x2206, 0xAC27}, {0x2206, 0x0EAE},
{0x2206, 0x0F02}, {0x2206, 0x84CC}, {0x2206, 0xAE0A}, {0x2206, 0x0284},
{0x2206, 0xD1AE}, {0x2206, 0x05AE}, {0x2206, 0x0302}, {0x2206, 0x84D8},
{0x2206, 0xFC04}, {0x2206, 0xEE8B}, {0x2206, 0x6800}, {0x2206, 0x0402},
{0x2206, 0x84E5}, {0x2206, 0x0285}, {0x2206, 0x2804}, {0x2206, 0x0285},
{0x2206, 0x4904}, {0x2206, 0xEE8B}, {0x2206, 0x6800}, {0x2206, 0xEE8B},
{0x2206, 0x6902}, {0x2206, 0x04F8}, {0x2206, 0xF9E0}, {0x2206, 0x8B85},
{0x2206, 0xAD26}, {0x2206, 0x38D0}, {0x2206, 0x0B02}, {0x2206, 0x2B4D},
{0x2206, 0x5882}, {0x2206, 0x7882}, {0x2206, 0x9F2D}, {0x2206, 0xE08B},
{0x2206, 0x68E1}, {0x2206, 0x8B69}, {0x2206, 0x1F10}, {0x2206, 0x9EC8},
{0x2206, 0x10E4}, {0x2206, 0x8B68}, {0x2206, 0xE0E0}, {0x2206, 0x00E1},
{0x2206, 0xE001}, {0x2206, 0xF727}, {0x2206, 0xE4E0}, {0x2206, 0x00E5},
{0x2206, 0xE001}, {0x2206, 0xE2E0}, {0x2206, 0x20E3}, {0x2206, 0xE021},
{0x2206, 0xAD30}, {0x2206, 0xF7F6}, {0x2206, 0x27E4}, {0x2206, 0xE000},
{0x2206, 0xE5E0}, {0x2206, 0x01FD}, {0x2206, 0xFC04}, {0x2206, 0xF8FA},
{0x2206, 0xEF69}, {0x2206, 0xE08B}, {0x2206, 0x86AD}, {0x2206, 0x2212},
{0x2206, 0xE0E0}, {0x2206, 0x14E1}, {0x2206, 0xE015}, {0x2206, 0xAD26},
{0x2206, 0x9CE1}, {0x2206, 0x85E0}, {0x2206, 0xBF85}, {0x2206, 0x6D02},
{0x2206, 0x2CA2}, {0x2206, 0xEF96}, {0x2206, 0xFEFC}, {0x2206, 0x04F8},
{0x2206, 0xFAEF}, {0x2206, 0x69E0}, {0x2206, 0x8B86}, {0x2206, 0xAD22},
{0x2206, 0x09E1}, {0x2206, 0x85E1}, {0x2206, 0xBF85}, {0x2206, 0x6D02},
{0x2206, 0x2CA2}, {0x2206, 0xEF96}, {0x2206, 0xFEFC}, {0x2206, 0x0464},
{0x2206, 0xE48C}, {0x2206, 0xFDE4}, {0x2206, 0x80CA}, {0x2206, 0xE480},
{0x2206, 0x66E0}, {0x2206, 0x8E70}, {0x2206, 0xE076}, {0x2205, 0xE142},
{0x2206, 0x0701}, {0x2205, 0xE140}, {0x2206, 0x0405}, {0x220F, 0x0000},
{0x221F, 0x0000}, {0x2200, 0x1340}, {0x133E, 0x000E}, {0x133F, 0x0010},
{0x13EB, 0x11BB}, {0x13E0, 0x0010}
};
/*End of ChipData30[][2]*/

rtk_uint16 ChipData31[][2]= {
/*Code of Func*/
{0x1B03, 0x0876}, {0x1200, 0x7FC4}, {0x1305, 0xC000}, {0x121E, 0x03CA},
{0x1233, 0x0352}, {0x1234, 0x0064}, {0x1237, 0x0096}, {0x1238, 0x0078},
{0x1239, 0x0084}, {0x123A, 0x0030}, {0x205F, 0x0002}, {0x2059, 0x1A00},
{0x205F, 0x0000}, {0x207F, 0x0002}, {0x2077, 0x0000}, {0x2078, 0x0000},
{0x2079, 0x0000}, {0x207A, 0x0000}, {0x207B, 0x0000}, {0x207F, 0x0000},
{0x205F, 0x0002}, {0x2053, 0x0000}, {0x2054, 0x0000}, {0x2055, 0x0000},
{0x2056, 0x0000}, {0x2057, 0x0000}, {0x205F, 0x0000}, {0x133F, 0x0030},
{0x133E, 0x000E}, {0x221F, 0x0005}, {0x2205, 0x8B86}, {0x2206, 0x800E},
{0x221F, 0x0000}, {0x133F, 0x0010}, {0x12A3, 0x2200}, {0x6107, 0xE58B},
{0x6103, 0xA970}, {0x0018, 0x0F00}, {0x0038, 0x0F00}, {0x0058, 0x0F00},
{0x0078, 0x0F00}, {0x0098, 0x0F00}, {0x133F, 0x0030}, {0x133E, 0x000E},
{0x221F, 0x0005}, {0x2205, 0x8B6E}, {0x2206, 0x0000}, {0x220F, 0x0100},
{0x2205, 0xFFF6}, {0x2206, 0x0080}, {0x2205, 0x8000}, {0x2206, 0x0280},
{0x2206, 0x2BF7}, {0x2206, 0x00E0}, {0x2206, 0xFFF7}, {0x2206, 0xA080},
{0x2206, 0x02AE}, {0x2206, 0xF602}, {0x2206, 0x0153}, {0x2206, 0x0201},
{0x2206, 0x6602}, {0x2206, 0x8044}, {0x2206, 0x0201}, {0x2206, 0x7CE0},
{0x2206, 0x8B8C}, {0x2206, 0xE18B}, {0x2206, 0x8D1E}, {0x2206, 0x01E1},
{0x2206, 0x8B8E}, {0x2206, 0x1E01}, {0x2206, 0xA000}, {0x2206, 0xE4AE},
{0x2206, 0xD8EE}, {0x2206, 0x85C0}, {0x2206, 0x00EE}, {0x2206, 0x85C1},
{0x2206, 0x00EE}, {0x2206, 0x8AFC}, {0x2206, 0x07EE}, {0x2206, 0x8AFD},
{0x2206, 0x73EE}, {0x2206, 0xFFF6}, {0x2206, 0x00EE}, {0x2206, 0xFFF7},
{0x2206, 0xFC04}, {0x2206, 0xF8E0}, {0x2206, 0x8B8E}, {0x2206, 0xAD20},
{0x2206, 0x0302}, {0x2206, 0x8050}, {0x2206, 0xFC04}, {0x2206, 0xF8F9},
{0x2206, 0xE08B}, {0x2206, 0x85AD}, {0x2206, 0x2548}, {0x2206, 0xE08A},
{0x2206, 0xE4E1}, {0x2206, 0x8AE5}, {0x2206, 0x7C00}, {0x2206, 0x009E},
{0x2206, 0x35EE}, {0x2206, 0x8AE4}, {0x2206, 0x00EE}, {0x2206, 0x8AE5},
{0x2206, 0x00E0}, {0x2206, 0x8AFC}, {0x2206, 0xE18A}, {0x2206, 0xFDE2},
{0x2206, 0x85C0}, {0x2206, 0xE385}, {0x2206, 0xC102}, {0x2206, 0x2DAC},
{0x2206, 0xAD20}, {0x2206, 0x12EE}, {0x2206, 0x8AE4}, {0x2206, 0x03EE},
{0x2206, 0x8AE5}, {0x2206, 0xB7EE}, {0x2206, 0x85C0}, {0x2206, 0x00EE},
{0x2206, 0x85C1}, {0x2206, 0x00AE}, {0x2206, 0x1115}, {0x2206, 0xE685},
{0x2206, 0xC0E7}, {0x2206, 0x85C1}, {0x2206, 0xAE08}, {0x2206, 0xEE85},
{0x2206, 0xC000}, {0x2206, 0xEE85}, {0x2206, 0xC100}, {0x2206, 0xFDFC},
{0x2206, 0x0400}, {0x2205, 0xE142}, {0x2206, 0x0701}, {0x2205, 0xE140},
{0x2206, 0x0405}, {0x220F, 0x0000}, {0x221F, 0x0000}, {0x133E, 0x000E},
{0x133F, 0x0010}, {0x13E0, 0x0010}, {0x207F, 0x0002}, {0x2073, 0x1D22},
{0x207F, 0x0000}, {0x133F, 0x0030}, {0x133E, 0x000E}, {0x2200, 0x1340},
{0x133E, 0x000E}, {0x133F, 0x0010}, };
/*End of ChipData21[][2]*/

#endif

char *mibcnt_string[]={
    "STAT_IfInOctets",
    "STAT_Dot3StatsFCSErrors",
    "STAT_Dot3StatsSymbolErrors",
    "STAT_Dot3InPauseFrames",
    "STAT_Dot3ControlInUnknownOpcodes",
    "STAT_EtherStatsFragments",
    "STAT_EtherStatsJabbers",
    "STAT_IfInUcastPkts",
    "STAT_EtherStatsDropEvents",
    "STAT_EtherStatsOctets",
    "STAT_EtherStatsUnderSizePkts",
    "STAT_EtherOversizeStats",
    "STAT_EtherStatsPkts64Octets",
    "STAT_EtherStatsPkts65to127Octets",
    "STAT_EtherStatsPkts128to255Octets",
    "STAT_EtherStatsPkts256to511Octets",
    "STAT_EtherStatsPkts512to1023Octets",
    "STAT_EtherStatsPkts1024to1518Octets",
    "STAT_EtherStatsMulticastPkts",
    "STAT_EtherStatsBroadcastPkts",
    "STAT_IfOutOctets",
    "STAT_Dot3StatsSingleCollisionFrames",
    "STAT_Dot3StatsMultipleCollisionFrames",
    "STAT_Dot3StatsDeferredTransmissions",
    "STAT_Dot3StatsLateCollisions",
    "STAT_EtherStatsCollisions",
    "STAT_Dot3StatsExcessiveCollisions",
    "STAT_Dot3OutPauseFrames",
    "STAT_Dot1dBasePortDelayExceededDiscards",
    "STAT_Dot1dTpPortInDiscards",
    "STAT_IfOutUcastPkts",
    "STAT_IfOutMulticastPkts",
    "STAT_IfOutBroadcastPkts",
    "STAT_OutOampduPkts",
    "STAT_InOampduPkts",
    "STAT_PktgenPkts",
    "STAT_InMldChecksumError",
    "STAT_InIgmpChecksumError",
    "STAT_InMldSpecificQuery",
    "STAT_InMldGeneralQuery",
    "STAT_InIgmpSpecificQuery",
    "STAT_InIgmpGeneralQuery",
    "STAT_InMldLeaves",
    "STAT_InIgmpInterfaceLeaves",
    "STAT_InIgmpJoinsSuccess",
    "STAT_InIgmpJoinsFail",
    "STAT_InMldJoinsSuccess",
    "STAT_InMldJoinsFail",
    "STAT_InReportSuppressionDrop",
    "STAT_InLeaveSuppressionDrop",
    "STAT_OutIgmpReports",
    "STAT_OutIgmpLeaves",
    "STAT_OutIgmpGeneralQuery",
    "STAT_OutIgmpSpecificQuery",
    "STAT_OutMldReports",
    "STAT_OutMldLeaves",
    "STAT_OutMldGeneralQuery",
    "STAT_OutMldSpecificQuery",
    "STAT_InKnownMulticastPkts",
    "STAT_IfInMulticastPkts",
    "STAT_IfInBroadcastPkts",
    "STAT_PORT_CNTR_END"
};


static rtk_api_ret_t _rtk_switch_init_setreg(rtk_uint32 reg, rtk_uint32 data)
{
#ifndef MDC_MDIO_OPERATION
    rtk_uint32      busyFlag, cnt;
#endif
    rtk_api_ret_t   retVal;

#ifdef MDC_MDIO_OPERATION
    if((retVal = rtl8367b_setAsicReg(reg, data) != RT_ERR_OK))
            return retVal;
#else
    if ((reg & 0xF000) == 0x2000)
    {
        cnt = 0;
        busyFlag = 1;
        while (busyFlag&&cnt<5)
        {
            cnt++;
            if ((retVal = rtl8367b_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                return retVal;
        }
        if (5 == cnt)
            return RT_ERR_BUSYWAIT_TIMEOUT;

        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, data)) !=  RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, reg)) !=  RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
            return retVal;
    }
    else
    {
        if((retVal = rtl8367b_setAsicReg(reg, data)) != RT_ERR_OK)
            return retVal;
    }
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
rtk_api_ret_t rtk_switch_init(void)
{
    rtk_uint16      i;
    rtk_uint32      data;
    rtk_api_ret_t   retVal;

    if((retVal = rtl8367b_setAsicReg(0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_getAsicReg(0x1301, &data)) != RT_ERR_OK)
        return retVal;

#if defined(RTK_X86_ASICDRV)
    if(init_para == ChipData00)
    {
        if(data & 0xF000)
        {
            init_para = ChipData01;
            init_size = (sizeof(ChipData01) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData00) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData10)
    {
        if(data & 0xF000)
        {
            init_para = ChipData11;
            init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData10) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData20)
    {
        if(data & 0xF000)
        {
            init_para = ChipData21;
            init_size = (sizeof(ChipData21) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData20) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData30)
    {
        if(data & 0xF000)
        {
            init_para = ChipData31;
            init_size = (sizeof(ChipData31) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData30) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData40)
    {
        if(data & 0xF000)
        {
            init_para = ChipData41;
            init_size = (sizeof(ChipData41) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData40) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData50)
    {
        if(data & 0xF000)
        {
            init_para = ChipData51;
            init_size = (sizeof(ChipData51) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData50) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData60)
    {
        if(data & 0xF000)
        {
            init_para = ChipData61;
            init_size = (sizeof(ChipData61) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData60) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData70)
    {
        if(data & 0xF000)
        {
            init_para = ChipData71;
            init_size = (sizeof(ChipData71) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData70) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData80)
    {
        if(data & 0xF000)
        {
            init_para = ChipData81;
            init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData80) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData90)
    {
        if(data & 0xF000)
        {
            init_para = ChipData91;
            init_size = (sizeof(ChipData91) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData90) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8363SB)
    if(data & 0xF000)
    {
        init_para = ChipData01;
        init_size = (sizeof(ChipData01) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData00;
        init_size = (sizeof(ChipData00) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8365MB)
    if(data & 0xF000)
    {
        init_para = ChipData11;
        init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData10;
        init_size = (sizeof(ChipData10) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367_VB)
    if(data & 0xF000)
    {
        init_para = ChipData21;
        init_size = (sizeof(ChipData21) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData20;
        init_size = (sizeof(ChipData20) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367RB)
    if(data & 0xF000)
    {
        init_para = ChipData31;
        init_size = (sizeof(ChipData31) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData30;
        init_size = (sizeof(ChipData30) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367R_VB)
    if(data & 0xF000)
    {
        init_para = ChipData41;
        init_size = (sizeof(ChipData41) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData40;
        init_size = (sizeof(ChipData40) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367MB)
    if(data & 0xF000)
    {
        init_para = ChipData51;
        init_size = (sizeof(ChipData51) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData50;
        init_size = (sizeof(ChipData50) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367M_VB)
    if(data & 0xF000)
    {
        init_para = ChipData61;
        init_size = (sizeof(ChipData61) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData60;
        init_size = (sizeof(ChipData60) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8368MB)
    if(data & 0xF000)
    {
        init_para = ChipData71;
        init_size = (sizeof(ChipData71) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData70;
        init_size = (sizeof(ChipData70) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8305MB)
    if(data & 0xF000)
    {
        init_para = ChipData81;
        init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData80;
        init_size = (sizeof(ChipData80) / ((sizeof(rtk_uint16))*2));
    }
#elif defined(CHIP_RTL8307M_VB)
    if(data & 0xF000)
    {
        init_para = ChipData91;
        init_size = (sizeof(ChipData91) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData90;
        init_size = (sizeof(ChipData90) / ((sizeof(rtk_uint16))*2));
    }

#else
    /* Not define CHIP, Error */
    init_para = NULL;
#endif

    if(init_para == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    for(i = 0; i < init_size; i++)
    {
        if((retVal = _rtk_switch_init_setreg((rtk_uint32)init_para[i][0], (rtk_uint32)init_para[i][1])) != RT_ERR_OK)
            return retVal;
    }

    if((retVal = rtl8367b_setAsicRegBit(0x18e0, 0, 0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_setAsicReg(0x1303, 0x0778)) != RT_ERR_OK)
        return retVal;
    if((retVal = rtl8367b_setAsicReg(0x1304, 0x7777)) != RT_ERR_OK)
        return retVal;
    if((retVal = rtl8367b_setAsicReg(0x13E2, 0x01FE)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_init
 * Description:
 *      Initialize VLAN.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      VLAN is disabled by default. User has to call this API to enable VLAN before
 *      using it. And It will set a default VLAN(vid 1) including all ports and set
 *      all ports PVID to the default VLAN.
 */
rtk_api_ret_t rtk_vlan_init(void)
{
    rtk_api_ret_t retVal;
    rtk_uint32 i;
    rtl8367b_user_vlan4kentry vlan4K;
    rtl8367b_vlanconfiguser vlanMC;


    /* clean 32 VLAN member configuration */
    for (i = 0; i <= RTL8367B_CVIDXMAX; i++)
    {
        vlanMC.evid = 0;
        vlanMC.mbr = 0;
        vlanMC.fid_msti = 0;
        vlanMC.envlanpol = 0;
        vlanMC.meteridx = 0;
        vlanMC.vbpen = 0;
        vlanMC.vbpri = 0;
        if ((retVal = rtl8367b_setAsicVlanMemberConfig(i, &vlanMC)) != RT_ERR_OK)
            return retVal;
    }

    /* Set a default VLAN with vid 1 to 4K table for all ports */
    memset(&vlan4K, 0, sizeof(rtl8367b_user_vlan4kentry));
    vlan4K.vid = 1;
    vlan4K.mbr = RTK_MAX_PORT_MASK;
    vlan4K.untag = RTK_MAX_PORT_MASK;
    vlan4K.fid_msti = 0;
    if ((retVal = rtl8367b_setAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;


    /* Also set the default VLAN to 32 member configuration index 0 */
    memset(&vlanMC, 0, sizeof(rtl8367b_vlanconfiguser));
    vlanMC.evid = 1;
    vlanMC.mbr = RTK_MAX_PORT_MASK;
    vlanMC.fid_msti = 0;
    if ((retVal = rtl8367b_setAsicVlanMemberConfig(0, &vlanMC)) != RT_ERR_OK)
            return retVal;

    /* Set all ports PVID to default VLAN and tag-mode to original */
    for (i = 0; i < RTK_MAX_NUM_OF_PORT; i++)
    {
        if ((retVal = rtl8367b_setAsicVlanPortBasedVID(i, 0, 0)) != RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367b_setAsicVlanEgressTagMode(i, EG_TAG_MODE_ORI)) != RT_ERR_OK)
            return retVal;
    }

    /* enable VLAN */
    if ((retVal = rtl8367b_setAsicVlanFilter(TRUE)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_set
 * Description:
 *      Set a VLAN entry.
 * Input:
 *      vid - VLAN ID to configure.
 *      mbrmsk - VLAN member set portmask.
 *      untagmsk - VLAN untag set portmask.
 *      fid - filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT 		        - Invalid input parameters.
 *      RT_ERR_L2_FID               - Invalid FID.
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - Invalid member port mask.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *      There are 4K VLAN entry supported. User could configure the member set and untag set
 *      for specified vid through this API. The portmask's bit N means port N.
 *      For example, mbrmask 23=0x17=010111 means port 0,1,2,4 in the member set.
 *      FID is for SVL/IVL usage, specify 0~15, the VLAN would be configured as SVL mode.
 *      If FID is specified as RTK_IVL_MODE_FID, the VLAN would be configured as IVL mode.
 */
rtk_api_ret_t rtk_vlan_set(rtk_vlan_t vid, rtk_portmask_t mbrmsk, rtk_portmask_t untagmsk, rtk_fid_t fid)
{
    rtk_api_ret_t retVal;
    rtl8367b_user_vlan4kentry vlan4K;

    /* vid must be 0~4095 */
    if (vid > RTL8367B_VIDMAX)
        return RT_ERR_VLAN_VID;

    if (mbrmsk.bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_VLAN_PORT_MBR_EXIST;

    if (untagmsk.bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_VLAN_PORT_MBR_EXIST;

    /* fid must be 0~15 */
    if ( (fid != RTK_IVL_MODE_FID) && (fid > RTL8367B_FIDMAX) )
        return RT_ERR_L2_FID;

    /* update 4K table */
    memset(&vlan4K, 0, sizeof(rtl8367b_user_vlan4kentry));
    vlan4K.vid = vid;
    vlan4K.mbr = mbrmsk.bits[0];
    vlan4K.untag = untagmsk.bits[0];

    if(fid == RTK_IVL_MODE_FID)
    {
        vlan4K.ivl_svl  = 1;
        vlan4K.fid_msti = 0;
    }
    else
        vlan4K.fid_msti = fid;

    if ((retVal = rtl8367b_setAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
            return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port.
 * Input:
 *      port - Port id.
 * Output:
 *      pPvid - Specified VLAN ID.
 *      pPriority - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can get the PVID and 802.1p priority for the PVID of Port-based VLAN.
 */
rtk_api_ret_t rtk_vlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t *pPriority)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index, pri;
    rtl8367b_vlanconfiguser vlanMC;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367b_getAsicVlanPortBasedVID(port, &index, &pri)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367b_getAsicVlanMemberConfig(index, &vlanMC)) != RT_ERR_OK)
        return retVal;

    *pPvid = vlanMC.evid;
    *pPriority = pri;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port.
 * Input:
 *      port - Port id.
 *      igr_filter - VLAN ingress function enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The status of vlan ingress filter is as following:
 *      - DISABLED
 *      - ENABLED
 *      While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *      ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
rtk_api_ret_t rtk_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t igr_filter)
{
    rtk_api_ret_t retVal;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    if (igr_filter >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = rtl8367b_setAsicVlanIngressFilter(port, igr_filter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *     rtk_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID).
 * Input:
 *      port - Port id.
 *      pvid - Specified VLAN ID.
 *      priority - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_VLAN_PRIORITY        - Invalid priority.
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - VLAN entry not found.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *       The API is used for Port-based VLAN. The untagged frame received from the
 *       port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t rtk_vlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority)
{
    rtk_api_ret_t retVal;
    rtk_int32 i;
    rtk_uint32 j;
    rtk_uint32 k;
    rtk_uint32 index,empty_idx;
    rtk_uint32 gvidx, proc;
    rtk_uint32 bUsed, pri;
    rtl8367b_user_vlan4kentry vlan4K;
    rtl8367b_vlanconfiguser vlanMC;
    rtl8367b_protocolvlancfg ppb_vlan_cfg;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    /* vid must be 0~4095 */
    if (pvid > RTL8367B_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* priority must be 0~7 */
    if (priority > RTL8367B_PRIMAX)
        return RT_ERR_VLAN_PRIORITY;


      empty_idx = 0xFFFF;

    for (i = RTL8367B_CVIDXMAX; i >= 0; i--)
    {
        if ((retVal = rtl8367b_getAsicVlanMemberConfig(i, &vlanMC)) != RT_ERR_OK)
            return retVal;

        if (pvid == vlanMC.evid)
        {
            if ((retVal = rtl8367b_setAsicVlanPortBasedVID(port, i, priority)) != RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
        else if (vlanMC.evid == 0 && vlanMC.mbr == 0)
        {
            empty_idx = i;
        }
    }


    /*
        vid doesn't exist in 32 member configuration. Find an empty entry in
        32 member configuration, then copy entry from 4K. If 32 member configuration
        are all full, then find an entry which not used by Port-based VLAN and
        then replace it with 4K. Finally, assign the index to the port.
    */

    if (empty_idx != 0xFFFF)
    {
        vlan4K.vid = pvid;
        if ((retVal = rtl8367b_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
            return retVal;

        vlanMC.evid = pvid;
        vlanMC.mbr = vlan4K.mbr;
        vlanMC.fid_msti = vlan4K.fid_msti;
        vlanMC.meteridx= vlan4K.meteridx;
        vlanMC.envlanpol= vlan4K.envlanpol;
        vlanMC.vbpen = vlan4K.vbpen;
        vlanMC.vbpri = vlan4K.vbpri;

        if ((retVal = rtl8367b_setAsicVlanMemberConfig(empty_idx, &vlanMC)) != RT_ERR_OK)
            return retVal;

        if ((retVal = rtl8367b_setAsicVlanPortBasedVID(port,empty_idx, priority)) != RT_ERR_OK)
            return retVal;

        return RT_ERR_OK;
     }

    if ((retVal = rtl8367b_getAsic1xGuestVidx(&gvidx)) != RT_ERR_OK)
        return retVal;

    /* 32 member configuration is full, found a unused entry to replace */
    for (i = 0; i <= RTL8367B_CVIDXMAX; i++)
    {
        bUsed = FALSE;

        for (j = 0; j < RTK_MAX_NUM_OF_PORT; j++)
        {
            if ((retVal = rtl8367b_getAsicVlanPortBasedVID(j, &index, &pri)) != RT_ERR_OK)
                return retVal;

            if (i == index)/*index i is in use by port j*/
            {
                bUsed = TRUE;
                break;
            }

            if (i == gvidx)
            {
                if ((retVal = rtl8367b_getAsic1xProcConfig(j, &proc)) != RT_ERR_OK)
                    return retVal;
                if (DOT1X_UNAUTH_GVLAN == proc )
                {
                    bUsed = TRUE;
                    break;
                }
            }

            for (k = 0; k <= RTL8367B_PROTOVLAN_GIDX_MAX; k++)
            {
                if ((retVal = rtl8367b_getAsicVlanPortAndProtocolBased(port, k, &ppb_vlan_cfg)) != RT_ERR_OK)
                    return retVal;
                if (ppb_vlan_cfg.valid == TRUE && ppb_vlan_cfg.vlan_idx == i)
                {
                    bUsed = TRUE;
                    break;
                }
            }
        }

        if (FALSE == bUsed)/*found a unused index, replace it*/
        {
            vlan4K.vid = pvid;
            if ((retVal = rtl8367b_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
                return retVal;
            vlanMC.evid = pvid;
            vlanMC.mbr = vlan4K.mbr;
            vlanMC.fid_msti = vlan4K.fid_msti;
            vlanMC.meteridx= vlan4K.meteridx;
            vlanMC.envlanpol= vlan4K.envlanpol;
            vlanMC.vbpen = vlan4K.vbpen;
            vlanMC.vbpri = vlan4K.vbpri;
            if ((retVal = rtl8367b_setAsicVlanMemberConfig(i, &vlanMC)) != RT_ERR_OK)
                return retVal;

            if ((retVal = rtl8367b_setAsicVlanPortBasedVID(port, i, priority)) != RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      rtk_vlan_get
 * Description:
 *      Get a VLAN entry.
 * Input:
 *      vid - VLAN ID to configure.
 * Output:
 *      pMbrmsk - VLAN member set portmask.
 *      pUntagmsk - VLAN untag set portmask.
 *      pFid - filtering database.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *     The API can get the member set, untag set and fid settings for specified vid.
 */
rtk_api_ret_t rtk_vlan_get(rtk_vlan_t vid, rtk_portmask_t *pMbrmsk, rtk_portmask_t *pUntagmsk, rtk_fid_t *pFid)
{
    rtk_api_ret_t retVal;
    rtl8367b_user_vlan4kentry vlan4K;

    /* vid must be 0~4095 */
    if (vid > RTL8367B_VIDMAX)
        return RT_ERR_VLAN_VID;

    vlan4K.vid = vid;

    if ((retVal = rtl8367b_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    pMbrmsk->bits[0] = vlan4K.mbr;
    pUntagmsk->bits[0] = vlan4K.untag;

    if(vlan4K.ivl_svl == 1)
        *pFid = RTK_IVL_MODE_FID;
    else
        *pFid = vlan4K.fid_msti;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset MIB counter of ports. API will use global reset while port mask is all-ports.
 */
rtk_api_ret_t rtk_stat_global_reset(void)
{
    rtk_api_ret_t retVal;

    if ((retVal = rtl8367b_setAsicMIBsCounterReset(TRUE,FALSE, 0)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset per port MIB counter by port.
 * Input:
 *      port - port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset MIB counter of ports. API will use global reset while port mask is all-ports.
 */
rtk_api_ret_t rtk_stat_port_reset(rtk_port_t port)
{
    rtk_api_ret_t retVal;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367b_setAsicMIBsCounterReset(FALSE,FALSE,1<<port)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get global MIB counter
 * Input:
 *      cntr_idx - global counter index.
 * Output:
 *      pCntr - global counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      Get global MIB counter by index definition.
 */
rtk_api_ret_t rtk_stat_global_get(rtk_stat_global_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t retVal;

    if (cntr_idx!=DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX)
        return RT_ERR_STAT_INVALID_GLOBAL_CNTR;

    if ((retVal = rtl8367b_getAsicMIBsCounter(0, cntr_idx, pCntr)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global MIB counter
 * Input:
 *      None
 * Output:
 *      pGlobal_cntrs - global counter structure.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      Get all global MIB counter by index definition.
 */
rtk_api_ret_t rtk_stat_global_getAll(rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    rtk_api_ret_t retVal;

    if ((retVal = rtl8367b_getAsicMIBsCounter(0,DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX, &pGlobal_cntrs->dot1dTpLearnedEntryDiscards)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

#define MIB_NOT_SUPPORT     (0xFFFF)
static rtk_api_ret_t _get_asic_mib_idx(rtk_stat_port_type_t cnt_idx, RTL8367B_MIBCOUNTER *pMib_idx)
{
    RTL8367B_MIBCOUNTER mib_asic_idx[STAT_PORT_CNTR_END]=
    {
        ifInOctets,                     /* STAT_IfInOctets */
        dot3StatsFCSErrors,             /* STAT_Dot3StatsFCSErrors */
        dot3StatsSymbolErrors,          /* STAT_Dot3StatsSymbolErrors */
        dot3InPauseFrames,              /* STAT_Dot3InPauseFrames */
        dot3ControlInUnknownOpcodes,    /* STAT_Dot3ControlInUnknownOpcodes */
        etherStatsFragments,            /* STAT_EtherStatsFragments */
        etherStatsJabbers,              /* STAT_EtherStatsJabbers */
        ifInUcastPkts,                  /* STAT_IfInUcastPkts */
        etherStatsDropEvents,           /* STAT_EtherStatsDropEvents */
        etherStatsOctets,               /* STAT_EtherStatsOctets */
        etherStatsUnderSizePkts,        /* STAT_EtherStatsUnderSizePkts */
        etherOversizeStats,             /* STAT_EtherOversizeStats */
        etherStatsPkts64Octets,         /* STAT_EtherStatsPkts64Octets */
        etherStatsPkts65to127Octets,    /* STAT_EtherStatsPkts65to127Octets */
        etherStatsPkts128to255Octets,   /* STAT_EtherStatsPkts128to255Octets */
        etherStatsPkts256to511Octets,   /* STAT_EtherStatsPkts256to511Octets */
        etherStatsPkts512to1023Octets,  /* STAT_EtherStatsPkts512to1023Octets */
        etherStatsPkts1024to1518Octets, /* STAT_EtherStatsPkts1024to1518Octets */
        ifInMulticastPkts,              /* STAT_EtherStatsMulticastPkts */
        ifInBroadcastPkts,              /* STAT_EtherStatsBroadcastPkts */
        ifOutOctets,                    /* STAT_IfOutOctets */
        dot3StatsSingleCollisionFrames, /* STAT_Dot3StatsSingleCollisionFrames */
        dot3StatMultipleCollisionFrames,/* STAT_Dot3StatsMultipleCollisionFrames */
        dot3sDeferredTransmissions,     /* STAT_Dot3StatsDeferredTransmissions */
        dot3StatsLateCollisions,        /* STAT_Dot3StatsLateCollisions */
        etherStatsCollisions,           /* STAT_EtherStatsCollisions */
        dot3StatsExcessiveCollisions,   /* STAT_Dot3StatsExcessiveCollisions */
        dot3OutPauseFrames,             /* STAT_Dot3OutPauseFrames */
        MIB_NOT_SUPPORT,                /* STAT_Dot1dBasePortDelayExceededDiscards */
        dot1dTpPortInDiscards,          /* STAT_Dot1dTpPortInDiscards */
        ifOutUcastPkts,                 /* STAT_IfOutUcastPkts */
        ifOutMulticastPkts,             /* STAT_IfOutMulticastPkts */
        ifOutBroadcastPkts,             /* STAT_IfOutBroadcastPkts */
        outOampduPkts,                  /* STAT_OutOampduPkts */
        inOampduPkts,                   /* STAT_InOampduPkts */
        MIB_NOT_SUPPORT,                /* STAT_PktgenPkts */
        inMldChecksumError,             /* STAT_InMldChecksumError */
        inIgmpChecksumError,            /* STAT_InIgmpChecksumError */
        inMldSpecificQuery,             /* STAT_InMldSpecificQuery */
        inMldGeneralQuery,              /* STAT_InMldGeneralQuery */
        inIgmpSpecificQuery,            /* STAT_InIgmpSpecificQuery */
        inIgmpGeneralQuery,             /* STAT_InIgmpGeneralQuery */
        inMldLeaves,                    /* STAT_InMldLeaves */
        inIgmpLeaves,          			/* STAT_InIgmpInterfaceLeaves */
        inIgmpJoinsSuccess,             /* STAT_InIgmpJoinsSuccess */
        inIgmpJoinsFail,                /* STAT_InIgmpJoinsFail */
        inMldJoinsSuccess,              /* STAT_InMldJoinsSuccess */
        inMldJoinsFail,                 /* STAT_InMldJoinsFail */
        inReportSuppressionDrop,        /* STAT_InReportSuppressionDrop */
        inLeaveSuppressionDrop,         /* STAT_InLeaveSuppressionDrop */
        outIgmpReports,                 /* STAT_OutIgmpReports */
        outIgmpLeaves,                  /* STAT_OutIgmpLeaves */
        outIgmpGeneralQuery,            /* STAT_OutIgmpGeneralQuery */
        outIgmpSpecificQuery,           /* STAT_OutIgmpSpecificQuery */
        outMldReports,                  /* STAT_OutMldReports */
        outMldLeaves,                   /* STAT_OutMldLeaves */
        outMldGeneralQuery,             /* STAT_OutMldGeneralQuery */
        outMldSpecificQuery,            /* STAT_OutMldSpecificQuery */
        inKnownMulticastPkts,           /* STAT_InKnownMulticastPkts */
        ifInMulticastPkts,              /* STAT_IfInMulticastPkts */
        ifInBroadcastPkts,              /* STAT_IfInBroadcastPkts */
    };

    if(cnt_idx >= STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if(mib_asic_idx[cnt_idx] == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    *pMib_idx = mib_asic_idx[cnt_idx];
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get per port MIB counter by index
 * Input:
 *      port        - port id.
 *      cntr_idx    - port counter index.
 * Output:
 *      pCntr - MIB retrived counter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Get per port MIB counter by index definition.
 */
rtk_api_ret_t rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t       retVal;
    RTL8367B_MIBCOUNTER mib_idx;
    rtk_stat_counter_t  second_cnt;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    if (cntr_idx>=STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if((retVal = _get_asic_mib_idx(cntr_idx, &mib_idx)) != RT_ERR_OK)
        return retVal;

    if(mib_idx == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((retVal = rtl8367b_getAsicMIBsCounter(port, mib_idx, pCntr)) != RT_ERR_OK)
        return retVal;

    if(cntr_idx == STAT_EtherStatsMulticastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutMulticastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = rtl8367b_getAsicMIBsCounter(port, mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    if(cntr_idx == STAT_EtherStatsBroadcastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutBroadcastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = rtl8367b_getAsicMIBsCounter(port, mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port - port id.
 * Output:
 *      pPort_cntrs - buffer pointer of counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      Get all MIB counters of one port.
 */
rtk_api_ret_t rtk_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    rtk_api_ret_t retVal;
    rtk_uint32 mibIndex;
    rtk_uint64 mibCounter;
    rtk_uint32 *accessPtr;
    /* address offset to MIBs counter */
    CONST_T rtk_uint16 mibLength[STAT_PORT_CNTR_END]= {
        2,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_PORT_ID;

    accessPtr = (rtk_uint32*)pPort_cntrs;
    for (mibIndex=0;mibIndex<STAT_PORT_CNTR_END;mibIndex++)
    {
        if ((retVal = rtk_stat_port_get(port, mibIndex, &mibCounter)) != RT_ERR_OK)
        {
            if (retVal == RT_ERR_CHIP_NOT_SUPPORTED)
                mibCounter = 0;
            else
                return retVal;
        }

        if (2 == mibLength[mibIndex])
            *(rtk_uint64*)accessPtr = mibCounter;
        else if (1 == mibLength[mibIndex])
            *accessPtr = mibCounter;
        else
            return RT_ERR_FAILED;

        accessPtr+=mibLength[mibIndex];
    }

    return RT_ERR_OK;
}

#define FID9	9
#define FID8	8
int RL6000_vlan_set(void){
	rtk_portmask_t mbrmsk, untag;
	rtk_api_ret_t retVal;
	
	retVal=rtk_switch_init();
	if(retVal!=RT_ERR_OK)
		printk("rtk_switch_init failed!\n");
	
	retVal=rtk_vlan_init();
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_init failed!\n");

	/* LAN, Port 1~4 and port 6 in VLAN 9 */
	mbrmsk.bits[0] = 0x005e;
	untag.bits[0]  = 0x005f;
	retVal=rtk_vlan_set(9, mbrmsk, untag, FID9);
	if(retVal!=RT_ERR_OK)
		printk("vlan set failed!\n");


	/* WAN, Port 0 and port 6 in VLAN 8 */
	mbrmsk.bits[0] = 0x041;
	untag.bits[0]  = 0x05f;
	retVal=rtk_vlan_set(8, mbrmsk, untag, FID8);
	if(retVal!=RT_ERR_OK)
		printk("vlan set failed!\n");
	
	retVal=rtk_vlan_portPvid_set(0, 8, 0);
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_portPvid_set 0 failed!\n");
	retVal=rtk_vlan_portPvid_set(1, 9, 0);
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_portPvid_set 1 failed!\n");
	retVal=rtk_vlan_portPvid_set(2, 9, 0);
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_portPvid_set 2 failed!\n");
	retVal=rtk_vlan_portPvid_set(3, 9, 0);
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_portPvid_set 3 failed!\n");
	retVal=rtk_vlan_portPvid_set(4, 9, 0);
	if(retVal!=RT_ERR_OK)
		printk("rtk_vlan_portPvid_set 4 failed!\n");

	{
		rtk_portmask_t mbrmsk2, untag2;
		int i;
		rtk_fid_t pFid;
		rtk_vlan_t pPvid;
		rtk_pri_t pPriority;
		rtk_vlan_get(9, &mbrmsk2, &untag2, &pFid);
		printk("vid:%d  portmask:0x%x untagmask:0x%x  fid:%d\n",9, mbrmsk2.bits[0], untag2.bits[0],pFid);

		rtk_vlan_get(8, &mbrmsk2, &untag2, &pFid);
		printk("vid:%d  portmask:0x%x untagmask:0x%x  fid:%d\n",8, mbrmsk2.bits[0], untag2.bits[0],pFid);

		for(i=0 ;i<5; i++){
			rtk_vlan_portPvid_get(i,&pPvid,&pPriority);
			printk("port:%d  pvid:%d priority:%d\n", i, pPvid, pPriority);
		}
	}
	
	return 0;
}

#define RL6000_CPU_PORT 6
void RL6000_cpu_tag(int enable){
	if(enable){
		smi_write(RTL8367B_REG_CPU_PORT_MASK,1<<RL6000_CPU_PORT); //set CPU port
		smi_write(RTL8367B_REG_CPU_CTRL,0x281|(RL6000_CPU_PORT)<<RTL8367B_CPU_TRAP_PORT_OFFSET);
	}
	else{
		smi_write(RTL8367B_REG_CPU_CTRL,0x280|(RL6000_CPU_PORT)<<RTL8367B_CPU_TRAP_PORT_OFFSET);
	}
}

void RL6000_RGMII(void){
	unsigned int rdata;
	smi_write(RTL8367B_REG_DIGITAL_INTERFACE_SELECT, 0x10);
	smi_read(RTL8367B_REG_DIGITAL_INTERFACE_SELECT,&rdata);
	//printk("reg.0x%x:%x\n",RTL8367B_REG_DIGITAL_INTERFACE_SELECT , rdata);
	smi_write(RTL8367B_REG_DIGITAL_INTERFACE1_FORCE, 0x1076);
	smi_write(RTL8367B_REG_EXT1_RGMXF, 0xc);
}
void reset_rtl8367b_PortStat(void){
	rtk_stat_global_reset();
}

void show_rtl8367b_PortStat(unsigned int port){
	unsigned long long pCntr;
	int i;
	printk("dump mib counters for port:%d\n",port);
	
	for(i=STAT_IfInOctets;i<STAT_PORT_CNTR_END;i++){
		if((i!=STAT_Dot3StatsFCSErrors) && (i!=STAT_IfInUcastPkts) && (i!=STAT_IfOutMulticastPkts)
				&& (i!=STAT_IfInMulticastPkts) && (i!=STAT_IfOutUcastPkts))
			continue;
		if(rtk_stat_port_get(port, i, &pCntr)!=RT_ERR_OK)
			printk("get %s failed!\n",mibcnt_string[i]);
		else
			printk("%s:%llu\n",mibcnt_string[i], pCntr);	
	}
}

