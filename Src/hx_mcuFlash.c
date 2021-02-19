/**
  ******************************************************************************
  * @file    FLASH_Program/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "hx_mcuFlash.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
			
/* Private define ------------------------------------------------------------*/
/**
  * @}
  */ 
/** @defgroup FLASH_Sectors
  * @{
  */

//#define FLASH_PI_NUM_PER_PAGE			(FLASH_PAGE_SIZE/FLASH_PI_DATA_SIZE)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char *FlashTmpBuf;
FLASH_REGION flashInfo;
FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t SectorError;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_SECTOR_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_SECTOR_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_SECTOR_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_SECTOR_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_SECTOR_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_SECTOR_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_SECTOR_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_SECTOR_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_SECTOR_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_SECTOR_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_SECTOR_10;  
	}
	else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
	{
		sector = FLASH_SECTOR_11;
	}

	return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
/*
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
     (Sector == FLASH_SECTOR_3) )
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}
*/

void FlashDrv_SetTempBuf(char *buf)
{
	FlashTmpBuf = buf;
}

void FlashDrv_SetParam(uint32_t page_size)
{
	flashInfo.block_size = page_size;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.NbSectors = 1;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	
}

int FlashDrv_Read(uint32_t addr, char *buf, uint32_t size)
{
	//uint32_t Address;
	uint32_t i;//, j;
	//uint32_t data;
	//union {
	//	uint32_t all;
	//	uint8_t ch[4];
	//}data;

//	__disable_irq();
	//MemoryProgramStatus = PASSED;
	//j = 0;
#if 0
	for (i = addr; i < (addr+size); i += 4)
	{
		data.all = *(__IO uint32_t *)i;
		if (size >= j+4) {
			buf[j++] = data.ch[0];
			buf[j++] = data.ch[1];
			buf[j++] = data.ch[2];
			buf[j++] = data.ch[3];
			if (j > size) break;
		}
		else {
			buf[j++] = data.ch[0];
			if (j > size) break;
			buf[j++] = data.ch[1];
			if (j > size) break;
			buf[j++] = data.ch[2];
			if (j > size) break;
			buf[j++] = data.ch[3];
			if (j > size) break;
		}
	}
#else
	for (i = 0; i < size; i++)
	{
		buf[i] = *((__IO uint8_t *)(addr+i));
	}

#endif
//	__enable_irq();

	return 1;
}

int FlashDrv_Write(uint32_t addr, char *data, uint32_t size)
{
	//uint32_t nextPage_addr;
	uint32_t backup_start_addr;
	uint32_t overwrite_size;
	char *pbuf;
	uint32_t i, temp;
	//uint32_t *pbuf32;
	//uint32_t Timeout;

//	__disable_irq();

	/* Unlock the Flash to enable the flash control register access *************/ 
	//HAL_FLASH_Unlock();

	/* Clear pending flags (if any) */  
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	//			FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

	while (size > 0)
	{
		if (addr < FLASH_USER_START_ADDR || addr >= FLASH_USER_END_ADDR) {
			break;
		}
 
		temp = addr/flashInfo.block_size;
		//erasePage_addr = GetSector(addr);//temp;
		backup_start_addr = temp*flashInfo.block_size;
		temp = backup_start_addr + flashInfo.block_size;

		if (addr + size > temp) overwrite_size = temp - addr;
		else overwrite_size = size;
		//backup data
//		FlashDrv_Read(backup_start_addr, FlashTmpBuf, flashInfo.block_size);
		pbuf = &FlashTmpBuf[addr - backup_start_addr];
		for (i = 0; i < overwrite_size; i += 1) {
			pbuf[i] = data[i];
		}

		EraseInitStruct.Sector = GetSector(addr);
		HAL_FLASH_Unlock();
		if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
		{
			//uSysStatus.all |= FLASH_ERASE_FAIL;
			HAL_FLASH_Lock();
			return 0;
		}
		HAL_FLASH_Lock();

		temp = 0;
		//pbuf32 = (uint32_t *)FlashTmpBuf;
		HAL_FLASH_Unlock();
		//for (i = backup_start_addr; i < backup_start_addr+flashInfo.block_size; i += 4) {
		for (i = 0; i < flashInfo.block_size; i++) {
			//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, i, pbuf32[temp]) != HAL_OK){
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, backup_start_addr+i, FlashTmpBuf[i]) != HAL_OK){
				//uSysStatus.all |= FLASH_WRITE_FAIL;
				HAL_FLASH_Lock();
				return 0;
			}
			//temp += 1;
			//Timeout = 5000;
			//while (Timeout > 0) {
			//	Timeout--;
			//}
		}
		HAL_FLASH_Lock();
		addr += overwrite_size;
		if (size >= overwrite_size) size -= overwrite_size;
		else size = 0;
		data = &data[overwrite_size];
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	//HAL_FLASH_Lock();
//	__enable_irq();

	return 1;
}

int FlashDrv_Write2(uint32_t addr, char *data, uint32_t size)
{
	//uint32_t nextPage_addr;
	//uint32_t backup_start_addr;
	//uint32_t overwrite_size;
	//char *pbuf;
	uint32_t i;//, temp;
	//uint32_t *pbuf32;
	//uint32_t Timeout;

	if (addr < FLASH_USER_START_ADDR || addr >= FLASH_USER_END_ADDR) {
		return 0;
	}

	//__disable_irq();
	//__HAL_FLASH_DATA_CACHE_RESET();
	/* Unlock the Flash to enable the flash control register access *************/ 
	HAL_FLASH_Unlock();
	//HAL_FLASH_OB_Unlock();
	//HAL_Delay(10);

	/* Clear pending flags (if any) */  
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	//			FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);	

	//EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	//EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	//EraseInitStruct.NbSectors = 1;
	EraseInitStruct.Sector = GetSector(addr);
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		//uSysStatus.all |= FLASH_ERASE_FAIL;
		//HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		//__enable_irq();
		return 0;
	}
	//HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();

	//temp = 0;
	HAL_FLASH_Unlock();
	//HAL_FLASH_OB_Unlock();
	//for (i = backup_start_addr; i < backup_start_addr+flashInfo.block_size; i += 4) {
	for (i = 0; i < size; i++) {
		//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, i, pbuf32[temp]) != HAL_OK){
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr+i, data[i]) != HAL_OK){
			//uSysStatus.all |= FLASH_WRITE_FAIL;
			//HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
			//__enable_irq();
			return 0;
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	//HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();
	//__enable_irq();

	return 1;
}


