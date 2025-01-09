/**
  ******************************************************************************
  * @file           : extmem_user_driver.c
  * @version        : 1.0.0
  * @brief          : This file implements the extmem configuration
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"/**
  ******************************************************************************
  * @file    stm32_user_driver.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for user support
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_DRIVER_USER == 1
#include "stm32_user_driver_api.h"
#include "stm32_user_driver_type.h"

/** @defgroup USER USER driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup USER_Private_Macro Private Macro
  * @{
  */

// reset
#define CMD_SPI_RSTEN 0x66
#define CMD_SPI_RST 0x99

#define CMD_SPI_READID 0x9F

// read
#define CMD_SPI_FAST_READ4B 0x0C
#define DUMMY_OSPI_FAST_READ4B 8


// write enable
#define CMD_SPI_WREN 0x6


// read status register
#define CMD_SPI_RDSR 0x05

#define OSPI_SR_WEL 0x2
#define OSPI_SR_WIP 0x1

/**
  * @}
  */
/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern XSPI_HandleTypeDef hxspi1;

#define hxspi hxspi1

#define HAL_TIMEOUT 0x1000
uint32_t u32RetVal = 0;

#define PAGE_SIZE 0x100
#define SECTOR_SIZE 4096

uint8_t dev_id[3];

/* Private functions ---------------------------------------------------------*/

/** @defgroup USER_Exported_Functions Exported Functions
  * @{
  */

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Init(uint32_t MemoryId,
                                                         EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  UserObject->MemID = MemoryId;     /* Keep the memory id, could be used to control more than one user memory */
  UserObject->PtrUserDriver = NULL; /* could be used to link data with the memory id */
  {
    XSPI_RegularCmdTypeDef cmd;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_RSTEN;
    cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
    cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;//HAL_XSPI_ADDRESS_1_LINE
    cmd.Address = 0;
    cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
    cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
    cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
    cmd.DummyCycles = 0;
    cmd.DataMode = HAL_XSPI_DATA_NONE; //HAL_XSPI_DATA_1_LINE
    cmd.DataLength =0;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
	}
  {
    XSPI_RegularCmdTypeDef cmd;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_RST;
    cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
    cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;//HAL_XSPI_ADDRESS_1_LINE
    cmd.Address = 0;
    cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
    cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
    cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
    cmd.DummyCycles = 0;
    cmd.DataMode = HAL_XSPI_DATA_NONE; //HAL_XSPI_DATA_1_LINE
    cmd.DataLength =0;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
  }

  {
    XSPI_RegularCmdTypeDef cmd;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_READID;
    cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
    cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;//HAL_XSPI_ADDRESS_1_LINE
    cmd.Address = 0;
    cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
    cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
    cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
    cmd.DummyCycles = 0;
    cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_NONE
    cmd.DataLength =3;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);

    u32RetVal = HAL_XSPI_Receive(&hxspi, dev_id, HAL_TIMEOUT);
  }
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_DeInit(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)UserObject;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Read(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject,
                                                         uint32_t Address, uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Data;
  (void)Size;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Write(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject,
                                                          uint32_t Address, const uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Data;
  (void)Size;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_EraseSector(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject,
                                                                uint32_t Address, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Size;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_MassErase(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetMapAddress(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject, uint32_t* BaseAddress)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)BaseAddress;
  return retr;
}

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetInfo(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject, EXTMEM_USER_MemInfoTypeDef* MemInfo)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)MemInfo;
  return retr;
}

/**
  * @}
  */

/** @addtogroup USER_Private_Functions
  * @{
  */

/**
  * @}
  */


#endif /* EXTMEM_DRIVER_USER == 1 */
