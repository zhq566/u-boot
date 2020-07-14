/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

/* Fout = 400MHz, Fin = 12MHz */
#define M_MDIV	0x5c
#define M_PDIV	0x1
#define M_SDIV	0x1

/* 48MHz USB */
#define U_M_MDIV	0x38
#define U_M_PDIV	0x2
#define U_M_SDIV	0x2


static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b" : "=r" (loops) : "0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
    writel(0x002556A9, &gpio->gpbcon);

	return 0;
}

int board_init(void)
{
	/* arch number of SMDK2440-Board */
	gd->bd->bi_arch_number = MACH_TYPE_S3C2440;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
}
