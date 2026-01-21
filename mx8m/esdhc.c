/*
 * Copyright (c) 2018 Diego Sueiro <diego.sueiro@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Driver for eSDHC on NXP IMX family processor.
 */

#include <stdint.h>
#include <esdhc.h>
#include <io.h>

#define SDHC_BLKSIZE                   0x04

#define SDHC_ARGUMENT                  0x08

#define SDHC_TRNMOD                    0x0C
#define SDHC_CMD_RESPONSE              (3 << 16)
#define SDHC_CMD_DATA_PRESENT          BIT(21)

#define SDHC_RSPREG0                   0x10
/* ROC Response Register 1 0x0 */
#define SDHC_RSPREG1                   0x14
/* ROC Response Register 2 0x0 */
#define SDHC_RSPREG2                   0x18
/* ROC Response Register 3 0x0 */
#define SDHC_RSPREG3                   0x1C

#define SDHC_HOSTCTL                   0x28

#define SDHC_NORINTSTS                 0x30

#define ESDHC_MIX_CTRL                 0x48
#define ESDHC_MIX_CTRL_DMA_EN          BIT(0)

#define SDHC_ADMASYSADDR               0x58

struct imx_esdhc_config {
    uintptr_t base;
    uint16_t rca;
};

static struct imx_esdhc_config esdhc_dev_cfg;

/**
 * @brief Initialize esdhc channel
 *
 * This routine is called to reset the chip in a quiescent state.
 * It is assumed that this function is called only once per esdhc.
 *
 * @param dev esdhc device struct
 *
 * @return 0
 */
int esdhc_init(const struct sdhci_config *esdhc_cfg)
{
    uintptr_t base;

    if (!esdhc_cfg) {
        return -1;
    }

    esdhc_dev_cfg.base = esdhc_cfg->base;
    base = esdhc_cfg->base;

    /* CMD0 */
    writel(0, base + SDHC_ARGUMENT);
    writel(0, base + SDHC_TRNMOD);

    /* CMD8 */
    writel(0x000001aa, base + SDHC_ARGUMENT);
    writel(8 << 24, base + SDHC_TRNMOD);

    /* CMD55 */
    writel(0, base + SDHC_ARGUMENT);
    writel(55 << 24, base + SDHC_TRNMOD);

    /* CMD41 */
    writel(0x50ff8000, base + SDHC_ARGUMENT);
    writel(41 << 24, base + SDHC_TRNMOD);

    /* CMD2 */
    writel(0, base + SDHC_ARGUMENT);
    writel(2 << 24, base + SDHC_TRNMOD);

    /* CMD3 */
    writel(0, base + SDHC_ARGUMENT);
    writel(3 << 24 | SDHC_CMD_RESPONSE, base + SDHC_TRNMOD);
    esdhc_dev_cfg.rca = readl(base + SDHC_RSPREG0) >> 16;

    /* CMD7 */
    writel(esdhc_dev_cfg.rca << 16, base + SDHC_ARGUMENT);
    writel(7 << 24, base + SDHC_TRNMOD);

    return 0;
}

struct desc {
    uint32_t addr;
    uint16_t size;
    uint8_t reserved;
    uint8_t attr;
};

void esdhc_read_block(uint8_t *buf, uint16_t size)
{
    uintptr_t base = esdhc_dev_cfg.base;

    /* CMD16 */
    writel(0x00000200, base + SDHC_ARGUMENT);
    writel(16 << 24, base + SDHC_TRNMOD);

    /* CMD18 */
    struct desc desc = {
        .addr = (uint32_t)buf,
        .size = size,
        .attr = 0x23,
    };
    writeq(&desc, SDHC_ADMASYSADDR);
    writel(0x00087200, base + SDHC_BLKSIZE);
    writel(ESDHC_MIX_CTRL_DMA_EN, base + ESDHC_MIX_CTRL);
    writel(0x00000012, base + SDHC_HOSTCTL);
    writel(0x00008000, base + SDHC_ARGUMENT);
    writel(18 << 24 | SDHC_CMD_DATA_PRESENT, base + SDHC_TRNMOD);
}
