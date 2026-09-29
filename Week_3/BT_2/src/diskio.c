/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "SD_card.h"

static DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv
)
{
    if (pdrv != 0)
    {
        return STA_NOINIT;
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv
)
{
    if (pdrv != 0)
    {
        return STA_NOINIT;
    }

    if (SD_Card_Init())
    {
        Stat &= (DSTATUS)~STA_NOINIT;
    }
    else
    {
        Stat |= STA_NOINIT;
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,
    BYTE *buff,
    LBA_t sector,
    UINT count
)
{
    if ((pdrv != 0) || (count == 0))
    {
        return RES_PARERR;
    }

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    for (UINT i = 0; i < count; i++)
    {
        if (!sd_read_a_block(sector + i,
                             &buff[i * 512]))
        {
            return RES_ERROR;
        }
    }

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,
    const BYTE *buff,
    LBA_t sector,
    UINT count
)
{
    if ((pdrv != 0) || (count == 0))
    {
        return RES_PARERR;
    }

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    for (UINT i = 0; i < count; i++)
    {
        if (!sd_write_a_block(sector + i,
                              &buff[i * 512]))
        {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/


DRESULT disk_ioctl (
    BYTE pdrv,
    BYTE cmd,
    void *buff
)
{
    if (pdrv != 0)
    {
        return RES_PARERR;
    }

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch (cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}


