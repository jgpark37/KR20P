/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems¡¯s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_EEPROM.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: EEPROM(AT25M02 of Microchip) control by SPI
                            A Page Write sequence allows up to 256 bytes to be written in the same write cycle,
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "ff.h"

/* Private define ------------------------------------------------------------*/
#define EEP_CS_L 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define EEP_CS_H 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define EEP_HOLD_L 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define EEP_HOLD_H 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define EEP_WP_L 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define EEP_WP_H 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)

#define EO_RDSR				0x05		//eeprom opcode
#define EO_LPWP				0x08
#define EO_WREN				0x06
#define EO_WRDI				0x04
#define EO_WRSR				0x01
#define EO_READ				0x03
#define EO_WRITE_BYTE		0x02
#define EO_WRITE_PAGE		0x07

#define EEP_PAGE_SIZE				256
#define EEP_MEM_MAX_SIZE			(EEP_PAGE_SIZE*1024)
#define EEP_BLOCK_SIZE				(EEP_PAGE_SIZE*2)

/* Private typedef -----------------------------------------------------------*/
typedef struct _tagSTATUS_REGISTER {
	unsigned char WPEN 		:1;
	unsigned char RFU			:3;
	unsigned char BP1_0		:2;
	unsigned char WEL			:1;
	unsigned char RDY_BSY	:1;
}StatusReg;



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *SPI_RxBuf;
SPI_HandleTypeDef *phSPI;

//FIL EEPFp; //file handle
//DIR EEPFatDir;				    /* Directory object */
DIR USERFatDir;				    /* Directory object */
//FILINFO EEPFno;			  /* File information object */
FILINFO USERFno;			  /* File information object */
//FATFS EEPFatFS; //structure with file system information
//uint8_t retEEP;    /* Return value for USBH */
//char EEPPath[4];   /* USBH logical drive path */
//char EEPText[]="12345678";//text which will be written into file
//char EEPName[]="12345678.dat";//name of the file
//char text2[100];//buffer for data read from file
extern uint8_t retUSER;    /* Return value for USER */
extern  char USERPath[4];   /* USER logical drive path */
extern  FATFS USERFatFS;    /* File system object for USER logical drive */
extern  FIL USERFile;       /* File object for USER */

struct {
	uint8_t linked;
	uint8_t mounted;
	uint8_t ready;  //pjg++180403: usb disk plug in check
	uint32_t freesize;
	uint32_t totalsize;
}EEPDiskInfo;	
//HAL_StatusTyypeDef hStateD;

DSTATUS EEPROM_initialize (BYTE);
DSTATUS EEPROM_status (BYTE);
DRESULT EEPROM_read (BYTE, BYTE*, DWORD, UINT);
DRESULT EEPROM_write (BYTE, const BYTE*, DWORD, UINT);
//#if _USE_IOCTL == 1
DRESULT EEPROM_ioctl (BYTE, BYTE, void*);
//#endif /* _USE_IOCTL == 1 */
const Diskio_drvTypeDef  EEPROM_Driver =
{
  EEPROM_initialize,
  EEPROM_status,
  EEPROM_read, 
  EEPROM_write,
//#if _USE_IOCTL == 1
  EEPROM_ioctl,
//#endif /* _USE_IOCTL == 1 */
};

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;


/* Private function prototypes -----------------------------------------------*/
/*
int EEPROMDrv_LowPowerWritePoll(uint8_t fd, uint8_t *buf) 
{
	//if (hStateD != HAL_OK) return 0;
	if (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) return 0;
	
	L_SPI1CS;
	HAL_SPI_Receive(&hspi1, buf, 1, 10000);
	H_SPI1CS;
	return 1;
}
*/

int EEPROMDrv_ReadStatusRegister(uint8_t fd, uint8_t *buf) 
{
	HAL_StatusTypeDef hstd;
	uint8_t txBuf;
	
	//if (hStateD != HAL_OK) return 0;
//	if (HAL_SPI_GetState(phSPI) != HAL_SPI_STATE_READY) return 0;
	
	txBuf = EO_RDSR;	//opcode

	EEP_CS_L;
	hstd = HAL_SPI_Transmit(phSPI, &txBuf, 1, 10);
	hstd = HAL_SPI_Receive(phSPI, buf, 1, 10);
	//td = HAL_SPI_TransmitReceive(phSPI, &txBuf, buf, 1, 10);
	EEP_CS_H;

	if (hstd != HAL_OK) return 0;
	return 1;
}

int EEPROMDrv_WriteOperationCode(uint8_t fd, uint8_t buf) 
{
	HAL_StatusTypeDef hstd;

	//if (hStateD != HAL_OK) return 0;
//	if (HAL_SPI_GetState(phSPI) != HAL_SPI_STATE_READY) return 0;
	
	EEP_CS_L;
	hstd = HAL_SPI_Transmit(phSPI, &buf, 1, 10);
	EEP_CS_H;

	if (hstd != HAL_OK) return 0;
	return 1;
}


int EEPROMDrv_Read(uint8_t fd, uint32_t addr, uint8_t *buf, uint32_t len) 
{
	HAL_StatusTypeDef hstd;
	uint8_t txBuf[4];
	//uint8_t status;
	//uint32_t tickstart, Timeout;
	
	//if (hStateD != HAL_OK) return 0;
//	if (HAL_SPI_GetState(phSPI) != HAL_SPI_STATE_READY) return 0;
	if ((addr+len) >= EEP_MEM_MAX_SIZE) return 0; //pjg++180531
	
	txBuf[0] = EO_READ;	//opcode
	txBuf[1] = (uint8_t)((addr&0x00ff0000)>>16);
	txBuf[2] = (uint8_t)((addr&0x0000ff00)>>8);
	txBuf[3] = (uint8_t)(addr&0x000000ff);

	EEP_CS_L;
	hstd = HAL_SPI_Transmit(phSPI, txBuf, 4, 10);
	hstd = HAL_SPI_Receive(phSPI, buf, len, 10*len);
	//hstd = HAL_SPI_TransmitReceive(phSPI, txBuf, buf, len, 10*len);
	EEP_CS_H;

	if (hstd != HAL_OK) return 0;

#if 0
	status = 1;//.RDY_BSY = 1;
	//i = //10000*len;
	Timeout = 10+20*len;//10*len;
	tickstart = HAL_GetTick();
	while ((HAL_GetTick()-tickstart) <=  Timeout) {
		EEPROMDrv_ReadStatusRegister(0, (uint8_t *)&status);
		if ((status&0x01) == 0) {
                  break;
		}
		//i--;
	}
#endif
	return len;
}

//
// buf[0:3] : opcode and address
// buf[4]:~] : data

int EEPROMDrv_WriteByte(uint8_t fd, uint32_t addr, uint8_t data) 
{
	HAL_StatusTypeDef hstd;
	uint8_t status;
	uint32_t i;
	uint32_t tickstart, Timeout;
	uint8_t buf[5];
	//StatusReg status;
	
	//while (hStateD == HAL_OK);
//	if (HAL_SPI_GetState(phSPI) != HAL_SPI_STATE_READY) return 0;
	if ((addr+1) >= EEP_MEM_MAX_SIZE) return 0; //pjg++180531

	if (!EEPROMDrv_WriteOperationCode(fd, EO_WREN)) return 0;	//WREN
	//Timeout = 10;
	//tickstart = HAL_GetTick();
	//while ((HAL_GetTick()-tickstart) <=  Timeout) {}
	
	buf[0] = EO_WRITE_BYTE;	//opcode
	buf[1] = (uint8_t)((addr&0x00ff0000)>>16);
	buf[2] = (uint8_t)((addr&0x0000ff00)>>8);
	buf[3] = (uint8_t)(addr&0x000000ff);
	buf[4] = data;
	//EEP_WP_L;
	EEP_CS_L;
	hstd = HAL_SPI_Transmit(phSPI, buf, 1+4, 10*1);
	EEP_CS_H;
	//EEP_WP_H;

	if (hstd != HAL_OK) return 0;

	//if (!EEPROMDrv_WriteOperationCode(0, EO_WRDI)) return 0;	//WRDI
	HAL_Delay(10);
	status = 1;//.RDY_BSY = 1;
	i = 10000;//000;
	Timeout = 10;
	tickstart = HAL_GetTick();
	while ((HAL_GetTick()-tickstart) <=  Timeout) {
		EEPROMDrv_ReadStatusRegister(fd, (uint8_t *)&status);
		if ((status&0x01) == 0) {
           break;
		}
		i--;
    	if (!i) break;
		HAL_Delay(1);
	}
	//if (i == 0) return 0;
	return 1;
}

// buf[0:3] : opcode and address
// buf[4]:~] : data
//uint32_t test_addr;
int EEPROMDrv_WritePage(uint8_t fd, uint32_t addr, uint8_t *data) 
{
	HAL_StatusTypeDef hstd;
	uint8_t status;
	uint32_t i;
	uint32_t tickstart, Timeout;
	uint8_t buf[4];

	//while (hStateD == HAL_OK);
//	if (HAL_SPI_GetState(phSPI) != HAL_SPI_STATE_READY) return 0;

	if (!EEPROMDrv_WriteOperationCode(fd, EO_WREN)) {
		return 0;	//WREN
    }
	//addr &= 0xffffff00;
	buf[0] = EO_WRITE_PAGE;	//opcode
	buf[1] = (uint8_t)((addr&0x00ff0000)>>16);
	buf[2] = (uint8_t)((addr&0x0000ff00)>>8);
	buf[3] = (uint8_t)(addr&0x000000ff);
	
	//EEP_WP_L;
	EEP_CS_L;
	hstd = HAL_SPI_Transmit(phSPI, buf, 4, 10);
	hstd = HAL_SPI_Transmit(phSPI, data, EEP_PAGE_SIZE, EEP_PAGE_SIZE);///10);
	//for (i = 0; i < EEP_PAGE_SIZE; i++)
	//	hstd = HAL_SPI_Transmit(phSPI, &buf[4+i], 1, 10);
	EEP_CS_H;
	//EEP_WP_H;

	if (hstd != HAL_OK) return 0;
	//if (!EEPROMDrv_WriteOperationCode(0, EO_WRDI)) return 0;	//WRDI

	HAL_Delay(10);
	status = 1;
	i = 12;//000;//000;
	tickstart = HAL_GetTick();
	Timeout = 10+2;
	while ((HAL_GetTick()-tickstart) <=  Timeout) {
		EEPROMDrv_ReadStatusRegister(fd, (uint8_t *)&status);
		if ((status&0x01) == 0) {
            break;
		}
		i--;
        if (!i) break;
		HAL_Delay(1);
	}
	//if (i == 0) return 0;
	//test_addr = addr;
	return EEP_PAGE_SIZE;
}

void EEPROMDrv_Init(SPI_HandleTypeDef *p_hSPI)
{
	//uint8_t buf;
	
	EEP_CS_H;
	EEP_WP_H;
	EEP_HOLD_H;
	phSPI = p_hSPI;
	//if (!EEPROMDrv_WriteOperationCode(0, EO_WREN)) return;	//WREN
	//buf = 'a';
	//while (0) {
	//	EEP_CS_L;
	//	HAL_SPI_Transmit(phSPI, &buf, 1, 1000);
	//	EEP_CS_H;
	//}
}

///////////////////////////////////////////////////////////////////////////////
// EEPROM Disk
///////////////////////////////////////////////////////////////////////////////
DSTATUS EEPROM_initialize (BYTE lun)
{
	return RES_OK;
}

DSTATUS EEPROM_status(BYTE lun)
{
	return RES_OK;
}

/**
  * @brief  Reads Sector(s) 
  * @param  lun : lun id
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT EEPROM_read (BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
#if 0
	int i, j;
	DWORD addr;
	uint8_t *ptr;
    extern char CommonBuf[];

	ptr = (uint8_t *)buff;
	addr = sector*EEP_BLOCK_SIZE;
	for (i = 0; i < count*2; i++) {
		if (!EEPROMDrv_Read(lun, addr, ptr, EEP_PAGE_SIZE)) return RES_ERROR;
		for (j = 0; j < EEP_PAGE_SIZE; j++) CommonBuf[j] = *(ptr+j);
		addr += EEP_PAGE_SIZE;
		ptr += EEP_PAGE_SIZE;
	}
#else
	//int j;
    //extern char CommonBuf[];

	//if (count > 1) {
	//	count = 2;
	//}
	if (!EEPROMDrv_Read(lun, sector*EEP_BLOCK_SIZE, buff, count*EEP_BLOCK_SIZE)) return RES_ERROR;
	//for (j = 0; j < EEP_BLOCK_SIZE; j++) CommonBuf[j] = *(buff+j);
#endif
	return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : lun id 
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT EEPROM_write (BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
	int i;
	DWORD addr;
	uint8_t *ptr;

	ptr = (uint8_t *)buff;
	addr = sector*EEP_BLOCK_SIZE;
	//EEPROMDrv_WriteByte(0, sector, (uint8_t *)buff, count);
	for (i = 0; i < count*2; i++) {
		if (!EEPROMDrv_WritePage(lun, addr, ptr)) return RES_ERROR;
		addr += EEP_PAGE_SIZE;
		ptr += EEP_PAGE_SIZE;
	}
	return RES_OK;
}

/**
  * @brief  I/O control operation
  * @param  lun : lun id
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
//#if _USE_IOCTL == 1
DRESULT EEPROM_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  //MSC_LUNTypeDef info;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC: 
  	//HAL_Delay(10);
    res = RES_OK;
    break;
    
  /* Get number of sectors on the disk (DWORD) */  
  case GET_SECTOR_COUNT : 
    *(DWORD*)buff = (DWORD)(EEP_MEM_MAX_SIZE/EEP_BLOCK_SIZE);//info.capacity.block_nbr;
    res = RES_OK;
    break;
    
  /* Get R/W sector size (WORD) */  
  case GET_SECTOR_SIZE :	
    *(DWORD*)buff = (DWORD)EEP_BLOCK_SIZE;//info.capacity.block_size;
    res = RES_OK;
    break;
    
    /* Get erase block size in unit of sector (DWORD) */ 
  case GET_BLOCK_SIZE : 
    *(DWORD*)buff = (DWORD)EEP_PAGE_SIZE;//info.capacity.block_size;
    res = RES_OK;
    break;
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}
//#endif /* _USE_IOCTL == 1 */

///////////////////////////////////////////////////////////////////////////////

int EEPROMDisk_Link(void)
{
	if (!EEPDiskInfo.linked) {
		retUSER = FATFS_LinkDriver(&EEPROM_Driver, USERPath);
		//retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);
		if (retUSER) return 0;
		EEPDiskInfo.linked = 1;
	}
	return 1;
}

void EEPROMDisk_UnLink(void)
{
	//FRESULT res;

	if (EEPDiskInfo.linked == 1) {
		retUSER = FATFS_UnLinkDriver(USERPath);
		//if (retEEP) return 0; //pjg<>190902
		EEPDiskInfo.linked = 0;
	}
}

void EEPROMDisk_UnMount(void)
{
	FRESULT res;

	if (EEPDiskInfo.mounted) {
		res = f_mount(0, (TCHAR const*)USERPath, 0);
		if(res != FR_OK){
			//EEPDiskInfo.mounted = 0;
			/* FatFs Initialization Error */
			return;
		}
	}
	EEPDiskInfo.mounted = 0;
}

int EEPROMDisk_Mount(void)
{
	FRESULT res;

	//Please note that FLASH disk must be formatted in FAT32 file system
	//res = f_mount(&EEPFp, (TCHAR const*)EEPPath, 0);
	res = f_mount(&USERFatFS, (TCHAR const*)USERPath, 0);
	if(res != FR_OK){
		EEPDiskInfo.mounted = 0;
		/* FatFs Initialization Error */
		return 0;
	}
	EEPDiskInfo.mounted = 1;

	return 1;
}

FRESULT EEPROMDisk_set_timestamp (
    char *obj,     /* Pointer to the file name */
    int year,
    int month,
    int mday,
    int hour,
    int min,
    int sec
)
{
    FILINFO fno;

    fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);

    return f_utime(obj, &fno);
}

uint32_t EEPROMDisk_OpenWrite(char *name)
{
	FRESULT res;
    FILINFO fno;
	int year, month, mday, hour, min, sec;

	//if (!EEPROMDisk_Mount()) return 0;
	
	/*open or create file for writing*/
	res = f_open(&USERFile, name, FA_CREATE_ALWAYS| FA_WRITE);
	if (res != FR_OK  ) {
		//while(1);
		return 0;
	}

	year = 2017;
	month = 6;
	mday = 15;
	hour = 10;
	min = 58;
	sec = 0;
    fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);
    f_utime(name, &fno);
	
	return 1;
}

uint32_t EEPROMDisk_OpenRead(char *name)
{
	FRESULT res;

	//if (!EEPROMDisk_Mount()) return 0;
	
	/*open file for reading*/
	res = f_open(&USERFile, name, FA_READ);
	if (res != FR_OK) {
		//while(1);
		return 0;
	}

	return 1;
}

void EEPROMDisk_Close()
{
	f_close(&USERFile);

	//EEPROMDisk_UnMount();
}

uint32_t EEPROMDisk_Write(uint8_t *buf, uint32_t len)
{
	FRESULT rv;
    UINT ByteWritten;
    
	/*write data into flashdisk*/   
	rv = f_write(&USERFile, buf, len, &ByteWritten);
	if(rv != FR_OK) {
		//while(1);
		return 0;
	}
	
	return ByteWritten;
}

int EEPROMDisk_IsReady(uint16_t wait)
{
	//uint32_t i;

	EEPDiskInfo.ready = 1;
	return 1;
}

uint32_t EEPROMDisk_Read(uint8_t *buf, uint32_t len)
{
	FRESULT res;
    UINT ByteRead;

	/*read data from flash*/
	res = f_read(&USERFile, buf, len, &ByteRead);
	if (res != FR_OK) {
		//while(1);
		return 0;
	}
	return ByteRead;
}

int EEPROMDisk_ReadDirectory(void)
{
	FRESULT res;

 /*  directory display test*/ 
	//Delay(50);	 
	//printf("Open root directory\n\r"); 
	res = f_opendir(&USERFatDir, "");
	
	if (res) { 
		//printf("Open root directory error\n\r");	 
	} 
	else { 
		//printf("Directory listing...\n\r"); 
		for (;;) { 
			res = f_readdir(&USERFatDir, &USERFno);		/* Read a directory item */ 
			if (res || !USERFno.fname[0]) { 
				break;	/* Error or end of dir */ 
			} 
			
			if (USERFno.fattrib & AM_DIR) { 
				//printf("  <dir>  %s\n\r", fno.fname); 
			} 
			else { 
				//printf("%8lu  %s\n\r", fno.fsize, fno.fname); 
			} 
		} 
	
		if (res) { 
			//printf("Read a directory error\n\r"); 
			//fault_err(ret); 
			return 0;
		} 
	} 
	//Delay(50);	 
	//printf("Test completed\n\r"); 


	/* Infinite loop */ 
	//while (1) { 
	//	STM_EVAL_LEDToggle(LED3); 
	//	Delay(100);	 
	//}

	return 1;
}

int EEPROMDisk_GetReadyFlag(void)
{
	return EEPDiskInfo.ready;
}

int EEPROMDisk_Format(void)
{
	FRESULT res;
	BYTE work[512];
	//DWORD plist[] = {100, 0, 0, 0};  /* Divide drive into two partitions */

	//f_fdisk(0, plist, work);  /* Divide physical drive 0 */
	//FRESULT f_mkfs (
	//  const TCHAR* path,  /* [IN] Logical drive number */
	//  BYTE  opt,          /* [IN] Format options */
	//  DWORD au,           /* [IN] Size of the allocation unit */
	//  void* work,         /* [-]  Working buffer */
	//  UINT len            /* [IN] Size of working buffer */
	//);
	res = f_mkfs((TCHAR const*)USERPath, FM_FAT, 512, work, sizeof(work)); 
	if (res != FR_OK)
	{
		//printf("Format error...\r\n");
		return 0;
	}
	return 1;
}

int EEPROMDisk_MkDir(char *dir)
{
	FRESULT res;

	res = f_mkdir(dir);
	if (res != FR_OK) 
	  return 0;
	return 1;
}

int EEPROMDisk_OpenDir(char *path)
{
	FRESULT res;
	//DIR dir;

	res = f_opendir(&USERFatDir, path);
	if (res != FR_OK) return 0;
	return 1;
}

FSIZE_t EEPROMDisk_GetSize(void)
{
	return f_size(&USERFile);
}

int EEPROMDisk_lseek(DWORD ofs)
{
	FRESULT res;

	res = f_lseek(&USERFile, ofs);
	if (res != FR_OK) return 0;
	return 1;
}

int EEPROMDisk_stat(char *path, FILINFO *fno)
{
	FRESULT fr;

	//printf("Test for 'file.txt'...\n");

	fr = f_stat(path, fno);
	switch (fr) {
	case FR_OK:
		//printf("Size: %lu\n", fno.fsize);
		//printf("Timestamp: %u/%02u/%02u, %02u:%02u\n",
		//(fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
		//fno.ftime >> 11, fno.ftime >> 5 & 63);
		//printf("Attributes: %c%c%c%c%c\n",
		//(fno.fattrib & AM_DIR) ? 'D' : '-',
		//(fno.fattrib & AM_RDO) ? 'R' : '-',
		//(fno.fattrib & AM_HID) ? 'H' : '-',
		//(fno.fattrib & AM_SYS) ? 'S' : '-',
		//(fno.fattrib & AM_ARC) ? 'A' : '-');
		break;
	case FR_NO_FILE:
		//printf("It is not exist.\n");
		return 0;
		//break;
	default:
		//printf("An error occured. (%d)\n", fr);
		return -1;
	}
	return 1;
}

uint32_t EEPROMDisk_GetFree(char *path)
{
	FRESULT res;
	FATFS *pfs;
	DWORD fre_clust, fre_sect, tot_sect;

	//fs = &USERFatFS;
	/* Get volume information and free clusters of drive 1 */
	res = f_getfree(path, &fre_clust, &pfs);
	if (res != FR_OK) return 0;//die(res);

	/* Get total sectors and free sectors */
	//tot_sect = (pfs->n_fatent - 2) * pfs->csize;
	//fre_sect = fre_clust * pfs->csize;
	tot_sect = (EEP_MEM_MAX_SIZE/EEP_BLOCK_SIZE) * EEP_BLOCK_SIZE;
	fre_sect = fre_clust * EEP_BLOCK_SIZE * 2;

	/* Print the free space (assuming 512 bytes/sector) */
	//printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
	EEPDiskInfo.freesize = fre_sect / 2;
	EEPDiskInfo.totalsize = tot_sect / 2;
	return EEPDiskInfo.freesize;//fre_sect / 2;
}

uint32_t EEPROMDisk_GetFreeFromMem(void)
{
	return EEPDiskInfo.freesize;
}

uint32_t EEPROMDisk_GetTotalFromMem(void)
{
	return EEPDiskInfo.totalsize;
}

int EEPROMDisk_Delete(char *path)
{
	FRESULT res;

	res = f_unlink(path);
	if (res != FR_OK) return 0;
	return 1;
}

int EEPROMDisk_IsPattern(char *pattern, char* filename)
{
	int i, j;
	uint8_t flag;//, f_filename, f_ext;
	uint8_t ext_pattern_buf[4];
	uint8_t extbuf[4];
	uint8_t pattern_buf[10];
	uint8_t pattern_len;
	uint8_t exe_pattern_len;

	//save ext data
	i = 0;
	j = 0;
	flag = 0;
	exe_pattern_len = 0;
	while (j < 3 && i < 100) {
		if (pattern[i] == 0) break;
		if (flag == 1) { //after find '.' text
			if (pattern[i] != '*') {
				ext_pattern_buf[exe_pattern_len] = pattern[i];
				exe_pattern_len++;
			}
		}
		if (pattern[i] == '.') {
			flag = 1;
		}
		i++;
	}	
	
	//save ext2 data
	i = 0;
	j = 0;
	flag = 0;
	while (j < 3 && i < 100) {
		if (filename[i] == 0) break;
		if (flag == 1) {
			extbuf[j] = filename[i];
			j++;
		}
		if (filename[i] == '.') {
			flag = 1;
		}
		i++;
	}
	//get pattern text from filename
	i = 0;
	pattern_len = 0;
	while (pattern_len < 10 && i < 100) {
		if (pattern[i] == 0) break;
		if (pattern[i] == '.') break;
		if (pattern[i] != '*') {
			pattern_buf[pattern_len] = pattern[i];
			pattern_len++;
		}
		i++;
	}
	//compare filename to pattern
	i = 0;
	j = 0;
	flag = 0;
	if (pattern_len == 0) {
		flag = 1;
	}
	else {
		while (i < 100) {
			if (filename[i] == 0) break;
			if (pattern_buf[j] == filename[i]) {
				j++;
				if (j == pattern_len) {
					flag = 1;
					break;
				}
			}
			i++;
		}
	}
	//compare file ext to exe pattern
	i = 0;
	j = 0;
	if (exe_pattern_len == 0) {
		flag |= 2;
	}
	else {
		while (i < 3) {
			if (ext_pattern_buf[j] == extbuf[i]) {
				j++;
				if (j == exe_pattern_len) {
					flag |= 2;
					break;
				}
			}
			i++;
		}
	}

	return flag;
}

// find first file name pattern
// return
// 1  : find
// -1 : not found
// 0  : no exist file 
int EEPROMDisk_FindFirst(char *path, char *pattern, char* filename)
{
	FRESULT res;
	//DIR dj; 		/* Directory search object */
	//FILINFO fno;	/* File information */
	int i, j;
	//uint8_t flag;
	
#if 0
	res = f_findfirst(dj, fno, "", path);  /* Start to search for photo files */
	if (res == FR_OK && fno->fname[0]) {
		filename = fno->fname;
		return 1;
	}
#else
	res = f_opendir(&USERFatDir, path);					   /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&USERFatDir, &USERFno);				   /* Read a directory item */
			if (res != FR_OK || USERFno.fname[0] == 0) break;  /* Break on error or end of dir */
			if (USERFno.fattrib & AM_DIR) { 				   /* It is a directory */
				//i = strlen(path);
				//sprintf(&path[i], "/%s", fno.fname);
				//res = scan_files(path); 				   /* Enter the directory */
				//if (res != FR_OK) break;
				//path[i] = 0;
			} else {									   /* It is a file. */
				//printf("%s/%s\n", path, fno.fname);
				j = 0;
				for (i = 0; i < 12; i++) {
					if (USERFno.fname[i] == '.') {
						filename[j] = '.';
						j++;
						break;
					}
					filename[j] = USERFno.fname[i];
					j++;
				}
				//flag = 0;
				for (i = j; i < 12; i++) {
					filename[j] = USERFno.fname[i];
					j++;
				}
				if (EEPROMDisk_IsPattern(pattern, filename) > 2) return 1;
				return -1;
			}
		}
	}
#endif
	return 0;
}

// find next file name pattern
// return
// 1  : find
// -1 : not found
// 0  : no exist file 
int EEPROMDisk_FindNext(char *pattern, char *filename)
{
	FRESULT res;
	int i, j;
	//uint8_t flag;

#if 0
    res = f_findnext(dj, fno);               /* Search for next item */
	if (res == FR_OK && fno->fname[0]) {
		filename = fno->fname;
		return 1;
	}
#else
	for (;;) {
		res = f_readdir(&USERFatDir, &USERFno);				   /* Read a directory item */
		if (res != FR_OK || USERFno.fname[0] == 0) break;  /* Break on error or end of dir */
		if (USERFno.fattrib & AM_DIR) { 				   /* It is a directory */
			//i = strlen(path);
			//sprintf(&path[i], "/%s", fno.fname);
			//res = scan_files(path); 				   /* Enter the directory */
			//if (res != FR_OK) break;
			//path[i] = 0;
		} else {									   /* It is a file. */
			//printf("%s/%s\n", path, fno.fname);
			j = 0;
			for (i = 0; i < 12; i++) {
				if (USERFno.fname[i] == '.') {
					filename[j] = '.';
					j++;
					break;
				}
				filename[j] = USERFno.fname[i];
				j++;
			}
			//flag = 0;
			for (i = j; i < 12; i++) {
				filename[j] = USERFno.fname[i];
				j++;
			}
			if (EEPROMDisk_IsPattern(pattern, filename) > 2) return 1;
			return -1;
		}
	}
#endif
	return 0;
}

int EEPROMDisk_CloseDir(void)
{
    f_closedir(&USERFatDir);
	return 1;
}

void EEPROMDisk_Init(void)
{
	EEPDiskInfo.linked = 0;
	EEPDiskInfo.mounted = 0;
	if (!retUSER) EEPDiskInfo.linked = 1;
	EEPDiskInfo.freesize = 0;
	EEPDiskInfo.totalsize = 1;
}

