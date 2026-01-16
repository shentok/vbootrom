/*
 * Copyright (c) 2018 Diego Sueiro <diego.sueiro@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Driver for UART on NXP IMX family processor.
 *
 * For full serial function, use the USART controller.
 *
 */

#include <stdint.h>
#include <uart.h>
#include <io.h>

#define Utxd 0x40
#define Ucr1 0x80
#define Ucr2 0x84
#define Ucr3 0x88
#define Ucr4 0x8c
#define Usr2 0x98
#define Uesc 0x9c
#define Utim 0xa0
#define Uts  0xb4

#define UCR1_ADEN	BIT(15) /* Auto dectect interrupt */
#define UCR1_ADBR	BIT(14) /* Auto detect baud rate */
#define UCR1_TRDYEN	BIT(13) /* Transmitter ready interrupt enable */
#define UCR1_IDEN	BIT(12) /* Idle condition interrupt */
#define UCR1_RRDYEN	BIT(9)	/* Recv ready interrupt enable */
#define UCR1_RDMAEN	BIT(8)	/* Recv ready DMA enable */
#define UCR1_IREN	BIT(7)	/* Infrared interface enable */
#define UCR1_TXMPTYEN	BIT(6)	/* Transimitter empty interrupt enable */
#define UCR1_RTSDEN	BIT(5)	/* RTS delta interrupt enable */
#define UCR1_SNDBRK	BIT(4)	/* Send break */
#define UCR1_TDMAEN	BIT(3)	/* Transmitter ready DMA enable */
#define UCR1_UARTCLKEN	BIT(2)	/* UART clock enabled */
#define UCR1_DOZE	BIT(1)	/* Doze */
#define UCR1_UARTEN	BIT(0)	/* UART enabled */

#define UCR2_TXEN	BIT(2)	/* Transmitter enabled */
#define UCR2_SRST	BIT(0)	/* SW reset */

#define UCR3_DSR	BIT(10) /* Data set ready */
#define UCR3_DCD	BIT(9)  /* Data carrier detect */
#define UCR3_RI		BIT(8)  /* Ring indicator */
#define UCR3_ADNIMP	BIT(7)  /* Autobaud Detection Not Improved */
#define UCR3_REF30	BIT(2)  /* Ref Freq 30 MHz */

#define UTS_TXFULL	BIT(4)	/* TxFIFO full */

struct imx_uart_config {
    uintptr_t base;
};

static struct imx_uart_config uart_dev_cfg;

void uart_imx_poll_out(unsigned char c)
{
    /* wait for transmitter to be ready */
    while (readl(uart_dev_cfg.base + Uts) & UTS_TXFULL);

    writel(c, uart_dev_cfg.base + Utxd);
}

/**
 * @brief Initialize UART channel
 *
 * This routine is called to reset the chip in a quiescent state.
 * It is assumed that this function is called only once per UART.
 *
 * @param dev UART device struct
 *
 * @return 0
 */
int uart_imx_init(const struct uart_config *uart_cfg)
{
    uintptr_t base;

    if (!uart_cfg) {
        return -1;
    }

    uart_dev_cfg.base = uart_cfg->base;
    base = uart_cfg->base;

    writel(0, base + Ucr1);
    writel(0, base + Ucr2);

    while (!(readl(base + Ucr2) & UCR2_SRST));

    writel(UCR2_TXEN, base + Ucr2);
    writel(UCR3_DSR | UCR3_DCD | UCR3_RI | UCR3_REF30 | UCR3_ADNIMP, base + Ucr3);
    writel(0x8000, base + Ucr4);
    writel(0x2b, base + Uesc);
    writel(0, base + Utim);

    writel(0, base + Uts);

    return 0;
}
