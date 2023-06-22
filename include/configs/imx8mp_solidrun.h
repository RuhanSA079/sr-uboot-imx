/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __IMX8MP_SOLIDRUN_H
#define __IMX8MP_SOLIDRUN_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"

#define CONFIG_SYS_BOOTM_LEN		(64 * SZ_1M)

#define CONFIG_SPL_MAX_SIZE		(152 * 1024)
#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_SECTOR
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR	0x300
#define CONFIG_SYS_UBOOT_BASE	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_STACK		0x96dff0
#define CONFIG_SPL_BSS_START_ADDR      0x96e000
#define CONFIG_SPL_BSS_MAX_SIZE		SZ_8K	/* 8 KB */
#define CONFIG_SYS_SPL_MALLOC_START	0x42200000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_512K	/* 512 KB */

/* For RAW image gives a error info not panic */
#define CONFIG_SPL_ABORT_ON_RAW_IMAGE

#if defined(CONFIG_NAND_BOOT)
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_DMA
#define CONFIG_SPL_NAND_MXS
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SPL_NAND_IDENT
#define CONFIG_SYS_NAND_U_BOOT_OFFS 	0x4000000 /* Put the FIT out of first 64MB boot area */

/* Set a redundant offset in nand FIT mtdpart. The new uuu will burn full boot image (not only FIT part) to the mtdpart, so we check both two offsets */
#define CONFIG_SYS_NAND_U_BOOT_OFFS_REDUND \
	(CONFIG_SYS_NAND_U_BOOT_OFFS + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512 - 0x8400)
#endif

#endif

#define CONFIG_CMD_READ
#define CONFIG_SERIAL_TAG
#define CONFIG_FASTBOOT_USB_DEV 0

#define CONFIG_REMAKE_ELF
/* ENET Config */
/* ENET1 */

#if defined(CONFIG_CMD_NET)
#define CONFIG_ETHPRIME                 "eth1" /* Set eqos to primary since we use its MDIO */

#define CONFIG_FEC_XCV_TYPE             RGMII
#define CONFIG_FEC_MXC_PHYADDR          1
#define FEC_QUIRK_ENET_MAC

#define DWC_NET_PHYADDR			0

#define PHY_ANEG_TIMEOUT 20000

#endif


#if 1
// debug //

#define JAILHOUSE_ENV \
	"jh_clk= \0 " \
	"jh_mmcboot=setenv fdtfile imx8mp-evk-root.dtb;" \
		"setenv jh_clk clk_ignore_unused mem=2048MB; " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "else run jh_netboot; fi; \0" \
	"jh_netboot=setenv fdtfile imx8mp-evk-root.dtb; setenv jh_clk clk_ignore_unused mem=2048MB; run netboot; \0 "

#define CONFIG_MFG_ENV_SETTINGS \
	CONFIG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x43800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=2\0"\
	"sd_dev=1\0" \


#ifdef CONFIG_NAND_BOOT
#define MFG_NAND_PARTITION "mtdparts=gpmi-nand:64m(nandboot),16m(nandfit),32m(nandkernel),16m(nanddtb),8m(nandtee),-(nandrootfs)"
#endif

/* Initial environment variables */
#define CONFIG_EXTRA_ENV_SETTINGS		\
	"bootcmd=run load_uc\0" \
	"fitloadaddr=0x45000000\0" \
	"loadfiles=load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${kernel_prefix}/${kernel_filename}; bootm ${fitloadaddr}#conf-0\0" \
	"kernel_filename=kernel.img\0" \
	"initrd_filename=initrd.img\0" \
	"core_state=/uboot/ubuntu/boot.sel\0" \
	"kernel_vars=snap_kernel snap_try_kernel kernel_status\0" \
	"recovery_vars=snapd_recovery_mode snapd_recovery_system snapd_recovery_kernel\0" \
	"snapd_recovery_mode=install\0" \
	"snapd_standard_params=panic=-1 systemd.gpt_auto=0 rd.systemd.unit=basic.target\0" \
	"boot_uc=run load_uc; bootm ${fitloadaddr}#conf-0\0" \
	"mmc_seed_part=1\0" \
	"mmc_boot_part=2\0" \
	"devtype=mmc\0" \
	"mmcdev=2\0" \
	"mmcpart=1\0" \
	"load_uc=" \
      		"setenv kernel_bootpart ${mmc_seed_part};"\
      		"load ${devtype} ${mmcdev}:${kernel_bootpart} 0x40480000 ${core_state};" \
      		"env import -v -c 0x40480000 ${filesize} ${recovery_vars};" \
      		"if test \"${snapd_recovery_mode}\" = \"run\"; then " \
		  "setenv bootargs \"console=${console} snapd_recovery_mode=${snapd_recovery_mode} ${snapd_standard_params}\";" \
		  "setenv kernel_bootpart ${mmc_boot_part}; " \
		  "load ${devtype} ${mmcdev}:${kernel_bootpart} 0x40480000 ${core_state}; " \
		  "env import -v -c 0x40480000 ${filesize} ${kernel_vars}; " \
		  "setenv kernel_name ${snap_kernel}; " \
		  "if test -n \"${kernel_status}\"; then " \
		    "if test \"${kernel_status}\" = \"try\"; then " \
		      "if test -n \"${snap_try_kernel}\"; then " \
		        "setenv kernel_status trying; " \
			"setenv kernel_name \"${snap_try_kernel}\"; " \
            	      "fi; " \
                    "elif test \"${kernel_status}\" = \"trying\"; then " \
                      "setenv kernel_status \"\"; " \
                    "fi;" \
          	    "env export -c 0x40480000 ${kernel_vars}; " \
		    "save ${devtype} ${mmcdev}:${kernel_bootpart} 0x40480000 ${core_state} ${filesize}; " \
		  "fi; " \
		  "setenv kernel_prefix \"/uboot/ubuntu/${kernel_name}/\"; " \
		"else " \
		  "setenv bootargs \"fde_helper=enabled console=${console} snapd_recovery_mode=${snapd_recovery_mode} snapd_recovery_system=${snapd_recovery_system} ${snapd_standard_params}\";" \
		  "setenv kernel_prefix \"/systems/${snapd_recovery_system}/kernel/\"; " \
		"fi; " \
	"run loadfiles;\0"

/* Link Definitions */
#define CONFIG_LOADADDR			0x40480000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_INIT_RAM_ADDR	0x40000000
#define CONFIG_SYS_INIT_RAM_SIZE	0x80000
#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_ENV_SPI_BUS		CONFIG_SF_DEFAULT_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SF_DEFAULT_CS
#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED

#define CONFIG_MMCROOT			"/dev/mmcblk1p2"  /* USDHC2 */

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		SZ_32M

/* Totally 6GB DDR */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define PHYS_SDRAM			0x40000000
#define PHYS_SDRAM_SIZE			0xC0000000	/* 3 GB */
#define PHYS_SDRAM_2			0x100000000
#ifdef CONFIG_TARGET_IMX8MP_DDR4_EVK
#define PHYS_SDRAM_2_SIZE		0x40000000	/* 1 GB */
#else
#define PHYS_SDRAM_2_SIZE		0xC0000000	/* 3 GB */
#endif

#define CONFIG_MXC_UART_BASE		UART2_BASE_ADDR

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_IMX_BOOTAUX
#define CONFIG_FSL_USDHC

#ifdef CONFIG_TARGET_IMX8MP_DDR4_EVK
#define CONFIG_SYS_FSL_USDHC_NUM	1
#else
#define CONFIG_SYS_FSL_USDHC_NUM	2
#endif
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#ifdef CONFIG_NAND_MXS
#define CONFIG_CMD_NAND_TRIMFFS

/* NAND stuff */
#define CONFIG_SYS_MAX_NAND_DEVICE     1
#define CONFIG_SYS_NAND_BASE           0x20000000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_USE_FLASH_BBT
#endif /* CONFIG_NAND_MXS */

#define CONFIG_SYS_I2C_SPEED		100000

/* USB configs */
#ifndef CONFIG_SPL_BUILD

#define CONFIG_CMD_USB_MASS_STORAGE
#define CONFIG_USB_GADGET_MASS_STORAGE
#define CONFIG_USB_FUNCTION_MASS_STORAGE
#endif

#define CONFIG_USB_MAX_CONTROLLER_COUNT         2
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_VBUS_DRAW 2

#ifdef CONFIG_DM_VIDEO
#define CONFIG_VIDEO_LOGO
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_VIDEO_BMP_LOGO
#endif

#define BOOT_TARGET_DEVICES(func) \
        func(MMC, mmc, 1) \
        func(MMC, mmc, 2) \
        func(USB, usb, 0) \
        func(PXE, pxe, na) \
        func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx8mp_evk_android.h"
#endif

#endif
// debug //

#endif
