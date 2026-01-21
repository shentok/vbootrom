/*
 * Copyright (c) 2026 Bernhard Beschow <shentey@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for eSDHC drivers
 */

#ifndef __MX8M_INCLUDE_ESDHC_H__
#define __MX8M_INCLUDE_ESDHC_H__

#include <stdint.h>

/**
 * @brief eSDHC Interface
 * @defgroup uart_interface eSDHC Interface
 * @since 1.0
 * @version 1.0.0
 * @ingroup io_interfaces
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

struct sdhci_config
{
    uintptr_t base;
};

int esdhc_init(const struct sdhci_config *esdhc_cfg);
void esdhc_read_block(uint8_t *buf, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* __MX8M_INCLUDE_ESDHC_H__ */
