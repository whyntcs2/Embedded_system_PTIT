#ifndef __STM32F10X_H
#define __STM32F10X_H

#include <stdint.h>


#define __IO volatile

typedef struct
{
    __IO uint32_t CRL;      /* 0x00 */
    __IO uint32_t CRH;      /* 0x04 */
    __IO uint32_t IDR;      /* 0x08 */
    __IO uint32_t ODR;      /* 0x0C */
    __IO uint32_t BSRR;     /* 0x10 */
    __IO uint32_t BRR;      /* 0x14 */
    __IO uint32_t LCKR;     /* 0x18 */
} GPIO_TypeDef;

typedef struct
{
    __IO uint32_t EVCR;       /* 0x00 */
    __IO uint32_t MAPR;       /* 0x04 */
    __IO uint32_t EXTICR[4];  /* 0x08 - 0x14 */
    uint32_t RESERVED0;       /* 0x18 */
    __IO uint32_t MAPR2;      /* 0x1C */
} AFIO_TypeDef;

typedef struct
{
    __IO uint32_t CR;        /* 0x00 */
    __IO uint32_t CFGR;      /* 0x04 */
    __IO uint32_t CIR;       /* 0x08 */
    __IO uint32_t APB2RSTR;  /* 0x0C */
    __IO uint32_t APB1RSTR;  /* 0x10 */
    __IO uint32_t AHBENR;    /* 0x14 */
    __IO uint32_t APB2ENR;   /* 0x18 */
    __IO uint32_t APB1ENR;   /* 0x1C */
    __IO uint32_t BDCR;      /* 0x20 */
    __IO uint32_t CSR;       /* 0x24 */
} RCC_TypeDef;

#define PERIPH_BASE       ((uint32_t)0x40000000U)
#define APB2PERIPH_BASE   (PERIPH_BASE + 0x00010000U)
#define AHBPERIPH_BASE    (PERIPH_BASE + 0x00018000U)

#define AFIO_BASE         (APB2PERIPH_BASE + 0x00000000U)
#define GPIOA_BASE        (APB2PERIPH_BASE + 0x00000800U)
#define RCC_BASE          (AHBPERIPH_BASE  + 0x00001000U)

#define AFIO              ((AFIO_TypeDef *) AFIO_BASE)
#define GPIOA             ((GPIO_TypeDef *) GPIOA_BASE)
#define RCC               ((RCC_TypeDef  *) RCC_BASE)

#define RCC_APB2ENR_AFIOEN   (1U << 0)
#define RCC_APB2ENR_IOPAEN   (1U << 2)

#define AFIO_MAPR_SWJ_CFG_MASK        (7U << 24)
#define AFIO_MAPR_SWJ_CFG_DISABLE     (4U << 24) /* Disable JTAG-DP + SW-DP */

#endif
