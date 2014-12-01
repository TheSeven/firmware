#pragma once

#define STM32_GPIO_CONFIG \
{ \
    GPIO_CONFIG_PORT( /* PORTA */ \
        /* PA 0 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 1 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_REF_CLK */ \
        /* PA 2 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_2MHZ, PULL_UP, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_MDIO */ \
        /* PA 3 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 4 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 5 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 6 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 7 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_CRS_DV */ \
        /* PA 8 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA 9 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA10 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PA11 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_OTG_FS, /* OTG_FS_DM */ \
        /* PA12 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_OTG_FS, /* OTG_FS_DP */ \
        /* PA13 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_100MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* SWDIO */ \
        /* PA14 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* SWCLK */ \
        /* PA15 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* IRQ */ \
    ), \
    GPIO_CONFIG_PORT( /* PORTB */ \
        /* PB 0 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB 1 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB 2 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB 3 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SPI1, /* SPI1_SCK */ \
        /* PB 4 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SPI1, /* SPI1_MISO */ \
        /* PB 5 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SPI1, /* SPI1_MOSI */ \
        /* PB 6 */ MODE_SPECIAL, TYPE_OPENDRAIN, SPEED_2MHZ, PULL_UP, INITCTL_SET, INITVAL_LOW, SPECIAL_I2C1, /* I2C1_SCL */ \
        /* PB 7 */ MODE_SPECIAL, TYPE_OPENDRAIN, SPEED_2MHZ, PULL_UP, INITCTL_SET, INITVAL_LOW, SPECIAL_I2C1, /* I2C1_SDA */ \
        /* PB 8 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB 9 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB10 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PB11 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_TX_EN */ \
        /* PB12 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_TXD0 */ \
        /* PB13 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_TXD1 */ \
        /* PB14 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_OTG_HS, /* OTG_HS_DM */ \
        /* PB15 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_OTG_HS, /* OTG_HS_DP */ \
    ), \
    GPIO_CONFIG_PORT( /* PORTC */ \
        /* PC 0 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* User button */ \
        /* PC 1 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_MDC */ \
        /* PC 2 */ MODE_OUTPUT, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_HIGH, SPECIAL_SYS, /* SPI flash CS */ \
        /* PC 3 */ MODE_OUTPUT, TYPE_PUSHPULL, SPEED_25MHZ, PULL_NONE, INITCTL_SET, INITVAL_HIGH, SPECIAL_SYS, /* microSD card CS */ \
        /* PC 4 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_RXD0 */ \
        /* PC 5 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_50MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_ETH, /* ETH_RMII_RXD1 */ \
        /* PC 6 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PC 7 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PC 8 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD D0 */ \
        /* PC 9 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD D1 */ \
        /* PC10 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD D2 */ \
        /* PC11 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD D3 */ \
        /* PC12 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD CLK */ \
        /* PC13 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* unmapped */ \
        /* PC14 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* OSC32_IN */ \
        /* PC15 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* OSC32_OUT */ \
    ), \
    GPIO_CONFIG_PORT( /* PORTD */ \
        /* PD 0 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 1 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 2 */ MODE_SPECIAL, TYPE_PUSHPULL, SPEED_25MHZ, PULL_UP, INITCTL_SET, INITVAL_HIGH, SPECIAL_SDIO, /* SD CMD */ \
        /* PD 3 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 4 */ MODE_OUTPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_HIGH, SPECIAL_SYS, /* nonpresent */ \
        /* PD 5 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 6 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 7 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 8 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD 9 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD10 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD11 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD12 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD13 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD14 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
        /* PD15 */ MODE_INPUT, TYPE_PUSHPULL, SPEED_2MHZ, PULL_NONE, INITCTL_SET, INITVAL_LOW, SPECIAL_SYS, /* nonpresent */ \
    ), \
}

#define STM32_ENABLE_SPI
#include "board/flexperiment/mini/f205/target.h"

