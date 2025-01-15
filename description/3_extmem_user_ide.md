# Pin toggle in application

1. Open STM32CubeIDE
2. Import Project to STM32CubeIDE or let STM32CubeMX to import the project for you.
3. You should have one main project and three sub projects in the workspace. 

4. Open Application project and its main.c
5. Add there the code for GPIO toggling

```c
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	  HAL_Delay(500);
```

Into the infinite loop:

```c-nc
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	  HAL_Delay(500);
  }
  /* USER CODE END 3 */
```


## Changle the linker

1. Open Application project properties (right click on application project and Properties)
2. Go to C/C++ Build > Settings > Tool settings > MCU/MPU GCC Linker > General
3. And change linker script to `STM32H7S3L8HX_ROMxspi1.ld`

![WARNING]
The CubeMX regeneration will return the LD to the default one

# Prepare variables for driver

we will add basic variable to manipulate with xspi and defines into our driver(`extmem_user_driver.c`)

1. Go to `Bootloader` project
2. Open file `extmem_user_driver.c`
3. Add variables

```c
extern XSPI_HandleTypeDef hxspi1;

#define hxspi hxspi1

#define HAL_TIMEOUT 0x1000
uint32_t u32RetVal = 0;

#define PAGE_SIZE 0x100
#define SECTOR_SIZE 4096

uint8_t dev_id[3];
```


Into the variable section:

```c-nc
/* Private variables ---------------------------------------------------------*/
extern XSPI_HandleTypeDef hxspi1;

#define hxspi hxspi1

#define HAL_TIMEOUT 0x1000
uint32_t u32RetVal = 0;

#define PAGE_SIZE 0x100
#define SECTOR_SIZE 4096

uint8_t dev_id[3];

/* Private functions ---------------------------------------------------------*/
```

The `extern XSPI_HandleTypeDef hxspi1` is xspi hadnle allow us use xspi1 with hal library
We use `#define hxspi hxspi1` to refer to it only as hxspi in case we need to move it to diferent xspi in the future

Out timeout time for HAL library `define HAL_TIMEOUT 0x1000`
And return variable to check what is status of HAL libraries `uint32_t u32RetVal = 0;`

We need to define also page size and sector size because we dont have acces to this values because they are only in external loader project. 

The page size is maximum size of programming
`#define PAGE_SIZE 0x100`
The sector size is size which is erased by erase
`#define SECTOR_SIZE 4096`

To check the correct reading we will read a ID of the memory:
`uint8_t dev_id[3];`


Define also all the commands for our memory and parameters for our memory

```c
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
```

Put it to private macro section

```c-nc
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
```


# Reading the memory ID

For this we send firs reset seqeunce to be sure that memory is in default state
Then we will send a read id command and we will receive the ID

Reset enable command
```c
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
```

Reset command
```c
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
```


Read ID command and ID reception

```c
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
```

Pu it to `EXTMEM_DRIVER_USER_Init` function in 

```c-nc
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
```

Now test the reading. Yiu can put the berakpoint to 
```c
	   u32RetVal = HAL_XSPI_Receive(&hxspi, dev_id, HAL_TIMEOUT);
```
 
and step over it. 
You should see values `0xc2`,`0x81`,`0x39`

If not as in oyur case is possible that SPI speed is to high. 

We can decrease the xspi clock

1. Open STM32CubeMX
2. Go to `XSPI1`
3. Set prescaller from `0` to `1`
4. Regenerate project
5. Try again

Now we should have a correct value `0xc2`,`0x81`,`0x39`

[!IMPORTANT]
If you still not see corect values increase again prescaller. If this not help best is to check it with scope if you are sending/receiving really correct values.


With this step we are sure that our memory is working and it is in default canfiguration. 


# Disable memory mapped mode implementation

To disable memory mapped mode we will use function   `HAL_XSPI_Abort`
Tha abor function can stop any xspi operation including memory mapped mode

1. Go to function `EXTMEM_DRIVER_USER_Disable_MemoryMappedMode`
2.  Add abort function
3.  Add correct return state
  

Into EXTMEM_DRIVER_USER_Disable_MemoryMappedMode put

```c
  HAL_XSPI_Abort(&hxspi);
  retr = EXTMEM_DRIVER_USER_OK;
```

it should looks like this:

```c-nc
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  HAL_XSPI_Abort(&hxspi);
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```


# Enable memory mapped mode

Enable memory mapped mode is more complex than disable it. 
We have function `HAL_XSPI_MemoryMapped` to switch the device to memory mapped mode. 
How ever we need firt to tell him which commands will be used to read/write in memory mapped mode. 

So first we set set dommands for read a and write and then we can enable memory mapped mode.

We can now implement the `EXTMEM_DRIVER_USER_Enable_MemoryMappedMode` function

Cmd for read operation type must be `HAL_XSPI_OPTYPE_READ_CFG`

```c
//read
  {
	   XSPI_RegularCmdTypeDef cmd;
	   cmd.OperationType = HAL_XSPI_OPTYPE_READ_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
	   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
	   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	   cmd.Instruction = CMD_SPI_FAST_READ4B;
	   cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
	   cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
	   cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;//HAL_XSPI_ADDRESS_1_LINE
	   cmd.Address = 0;
	   cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	   cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
	   cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
	   cmd.AlternateBytes = 0;
	   cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
	   cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
	   cmd.DummyCycles = DUMMY_OSPI_FAST_READ4B;
	   cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
	   cmd.DataLength =0;
	   cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
	   cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

	   u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
	}
```

Cmd for write operation type must be `HAL_XSPI_OPTYPE_WRITE_CFG`

```c
  //write
  {
	   XSPI_RegularCmdTypeDef cmd;
	   cmd.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
	   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
	   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	   cmd.Instruction = CMD_SPI_FAST_READ4B;
	   cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
	   cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
	   cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;//HAL_XSPI_ADDRESS_1_LINE
	   cmd.Address = 0;
	   cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	   cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
	   cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
	   cmd.AlternateBytes = 0;
	   cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
	   cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
	   cmd.DummyCycles = DUMMY_OSPI_FAST_READ4B;
	   cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
	   cmd.DataLength =0;
	   cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
	   cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

	   u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
	}
```

And at the end we enable memory mapped mode:

```c
  XSPI_MemoryMappedTypeDef memMapCfg;
  memMapCfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  memMapCfg.TimeoutPeriodClock = HAL_TIMEOUT;
  HAL_XSPI_MemoryMapped(&hxspi, &memMapCfg);
```

The `EXTMEM_DRIVER_USER_Enable_MemoryMappedMode` will then looks like:

```c-nc
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;

  XSPI_MemoryMappedTypeDef memMapCfg;
  //read
  {
	   XSPI_RegularCmdTypeDef cmd;
	   cmd.OperationType = HAL_XSPI_OPTYPE_READ_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
	   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
	   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	   cmd.Instruction = CMD_SPI_FAST_READ4B;
	   cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
	   cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
	   cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;//HAL_XSPI_ADDRESS_1_LINE
	   cmd.Address = 0;
	   cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	   cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
	   cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
	   cmd.AlternateBytes = 0;
	   cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
	   cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
	   cmd.DummyCycles = DUMMY_OSPI_FAST_READ4B;
	   cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
	   cmd.DataLength =0;
	   cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
	   cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

	   u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
	}
  //write
  {
	   XSPI_RegularCmdTypeDef cmd;
	   cmd.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
	   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
	   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	   cmd.Instruction = CMD_SPI_FAST_READ4B;
	   cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
	   cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
	   cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;//HAL_XSPI_ADDRESS_1_LINE
	   cmd.Address = 0;
	   cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	   cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
	   cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
	   cmd.AlternateBytes = 0;
	   cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
	   cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
	   cmd.DummyCycles = DUMMY_OSPI_FAST_READ4B;
	   cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
	   cmd.DataLength =0;
	   cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
	   cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

	   u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
	}

  memMapCfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  memMapCfg.TimeoutPeriodClock = HAL_TIMEOUT;
  HAL_XSPI_MemoryMapped(&hxspi, &memMapCfg);
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

[!IMPORTANT]
The Write is not really used so still a read command is send. It is because the HAL_XSPI_MemoryMapped reqeuire that both operation commands are defined


Now we can test the memory mapped mode the bootloader with memory_manager by default enable the memory mapped mode. 

1. Put breakpoint after `HAL_XSPI_MemoryMapped`
2. Run debug
3. Open memory window
4. Look into 0x90000000

If you tried the first handson then you should have already application data here. 
The jump to the application is still not working we are missing content for function `EXTMEM_DRIVER_USER_GetMapAddress`




# Get map Address

Now we will imoplement `EXTMEM_DRIVER_USER_GetMapAddress` function
Only what need to do is return a address where your memory is located
And set  correct return value.

```c
  *BaseAddress=0x90000000;
  retr = EXTMEM_DRIVER_USER_OK;
```

It will look like this:


```c-nc
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_GetMapAddress(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject, uint32_t* BaseAddress)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  *BaseAddress=0x90000000;
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

Now our bootloader will be able to jump into application. 

# Write enabled function

Here we will read a memory status register and check if the memory is unlocked for write
The function we can name `driver_spi_WriteEnabled`

We will use get status register command and we will read back the status register with `HAL_XSPI_Receive`

command setup
```c
    XSPI_RegularCmdTypeDef cmd;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_RDSR;
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
    cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
    cmd.DataLength =1;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;
```

The unlock can take some time so we pu it into loop

```c
    uint32_t u32Wait;
    uint32_t u32StatusValue;
    u32Wait =0;
    while(0 == u32Wait){
      u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
      u32RetVal = HAL_XSPI_Receive(&hxspi, &u32StatusValue, HAL_TIMEOUT);
      if((u32StatusValue & OSPI_SR_WEL) != 0){
        u32Wait=1;
      }
    }
  }
```

Here we read the status into `u32StatusValue` and check if the `OSPI_SR_WEL` is set

Complete function will looks like this:

```c-nc
uint32_t driver_spi_WriteEnabled(void){
  {
    XSPI_RegularCmdTypeDef cmd;
    uint32_t u32Wait;
    uint32_t u32StatusValue;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_RDSR;
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
    cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
    cmd.DataLength =1;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;
    u32Wait =0;
    while(0 == u32Wait){
      u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
      u32RetVal = HAL_XSPI_Receive(&hxspi, &u32StatusValue, HAL_TIMEOUT);
      if((u32StatusValue & OSPI_SR_WEL) != 0){
        u32Wait=1;
      }
    }
  }
  return 0;
}
```

# Write enable function

This is helping function to allow to enable write to memory
We can name it `driver_spi_WriteEnable`


For this we will create a function prototype

```c
uint32_t driver_spi_WriteEnable(void);
```

And we can start implement the function

For unlocking the memory we will use:

```c
	  {
		   XSPI_RegularCmdTypeDef cmd;
		   cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
		   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
		   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		   cmd.Instruction = CMD_SPI_WREN;
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
```

But we need to check if memory is really unclocked. So we will call `driver_spi_WriteEnabled`


```c
  return driver_spi_WriteEnabled();
```

It will look like this:

```c-nc
uint32_t driver_spi_WriteEnable(void){
	  {
		   XSPI_RegularCmdTypeDef cmd;
		   cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
		   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
		   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		   cmd.Instruction = CMD_SPI_WREN;
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

	  return driver_spi_WriteEnabled();
}
```


# Write wait function

This funtion will wait until program/erase/masserase is done 

We create function `driver_spi_WriteEnable`

The approach is completly the same as in `driver_spi_WriteEnabled` only we check ifferent flag which is `OSPI_SR_WIP`

```c
  if((u32StatusValue & OSPI_SR_WIP) == 0){
    u32Wait=1;
  }
```

It will looks like this:

```c-nc
uint32_t driver_spi_WriteWait(void){
  { 
    XSPI_RegularCmdTypeDef cmd;
    uint32_t u32Wait;
    uint32_t u32StatusValue;
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_SPI_RDSR;
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
    cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
    cmd.DataLength =1;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;
    u32Wait =0;
    while(0 == u32Wait){
    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
    u32RetVal = HAL_XSPI_Receive(&hxspi, &u32StatusValue, HAL_TIMEOUT);
        if((u32StatusValue & OSPI_SR_WIP) == 0){
          u32Wait=1;
        }
    }
  }
  return 0;
}
```



# Mass erase function

With memory unlocked the simples functin to use it with is a mass erase
Which is simple command

We need to add a comand define

```c
// chip erase
#define CMD_SPI_CE 0x60
```

We will not implement the `EXTMEM_DRIVER_USER_MassErase` function 

furst we unclok the memory

```c
  driver_spi_WriteEnable();
```

Then we send command to mass erase the memory

```c
{
  XSPI_RegularCmdTypeDef cmd;
  cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
  cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
  cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
  cmd.Instruction = CMD_SPI_CE;
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
```

At the enad we check if the erase was done with 

```c
  driver_spi_WriteWait();
```

And we set correct return state to `EXTMEM_DRIVER_USER_OK`

It will looks like this:

```c-nc
// chip erase
#define CMD_SPI_CE 0x60

EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_MassErase(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  driver_spi_WriteEnable();
  {
	   XSPI_RegularCmdTypeDef cmd;
	   cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
	   cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
	   cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	   cmd.Instruction = CMD_SPI_CE;
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
  driver_spi_WriteWait();
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

Now we can test the masserase (mainly if we have something in memory if the memory is empty is hard to discover and it must wait until we implement programming).

We can test it by adding it to end of Init function(dont forget to reove it later)

```c
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);

    u32RetVal = HAL_XSPI_Receive(&hxspi, dev_id, HAL_TIMEOUT);
  }

  EXTMEM_DRIVER_USER_MassErase(null);//test the masserase, remove later

  retr = EXTMEM_DRIVER_USER_OK;
  return retr;

```

You can now run debug. In inti it will masserase the memory and when memory mapped mode is enabled we can check the memory.

![WARNING]
If you are using H7S NUCLEO the masserase is queite long like 10s so the code will be in `driver_spi_WriteWait` very long time

The erased memory will be full of `0xFF`


# Write function

We can now unlock the memory and mass erase it and check write/erase is complete. Now we can implent the programming. 

The programing will implemented into `EXTMEM_DRIVER_USER_Write` function

It we can use most of the code from mass erase. 
We need to unlock memory. We need to check the write. 
Only difference will be the command and we need to transmit data. 

Also depends on size we can write pultiple pages. Between each write we need to unlock memory and after progamming check if programming is in progress.


```c-nc
// program
#define CMD_SPI_PP4B 0x12
#define MEMORY_MASK 0x0FFFFFFF
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_Write(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject,
                                                          uint32_t Address, const uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Data;
  (void)Size;

  uint32_t u32ProgramAddrOffet = 0;
  uint32_t u32ProgramAddr=0;
  uint32_t u32ProgramSize=0;
  while(u32ProgramAddrOffet< Size){
	  u32ProgramAddr = (Address & MEMORY_MASK) + u32ProgramAddrOffet;
	  if(Size < u32ProgramAddrOffet + PAGE_SIZE){
		  u32ProgramSize = Size - u32ProgramAddrOffet;
	  }else{
		  u32ProgramSize = PAGE_SIZE;
	  }
    driver_spi_WriteEnable();
    {
      XSPI_RegularCmdTypeDef cmd;
      cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_READ_CFG,HAL_XSPI_OPTYPE_WRITE_CFG
      cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;
      cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
      cmd.Instruction = CMD_SPI_PP4B;
      cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;//HAL_XSPI_ADDRESS_1_LINE
      cmd.Address = u32ProgramAddr;
      cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
      cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
      cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
      cmd.AlternateBytes = 0;
      cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;
      cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
      cmd.DummyCycles = 0;
      cmd.DataMode = HAL_XSPI_DATA_1_LINE; //HAL_XSPI_DATA_1_LINE
      cmd.DataLength =u32ProgramSize;
      cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
      cmd.DQSMode = HAL_XSPI_DQS_DISABLE;

      u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
      u32RetVal = HAL_XSPI_Transmit(&hxspi,(uint8_t*)(Data+u32ProgramAddrOffet) , HAL_TIMEOUT);
		}
    driver_spi_WriteWait();
    u32ProgramAddrOffet +=u32ProgramSize;
  }
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

Again for testing we can put it into `EXTMEM_DRIVER_USER_Init` function just after our testin Maserase.

```c
  EXTMEM_DRIVER_USER_Write(NULL,0,(uint8_t*)0x08000000,256);
```

We will write a 256B from the flash memory. Basically we copy begining of our bootloader into external memory. 
You can use different buffer if you want. 

```c-nc
  EXTMEM_DRIVER_USER_MassErase(NULL);
  EXTMEM_DRIVER_USER_Write(NULL,0,(uint8_t*)0x08000000,256);

  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
```

When we run the debug and after programming when the memory mapped mode is enabled we should see different data compared to previous case where we had only `0xFF`
You can compare it to `0x08000000` to confirm that it is working. 

# Sector erase

For sector erase we can again use the MassErase part and update it similart way like programming. 

Again we need a while loop in case we need to erase multiple sectors. Before each erase we must unlock the memory and after programming check when the programming finishes. 

```c-nc
EXTMEM_DRIVER_USER_StatusTypeDef EXTMEM_DRIVER_USER_EraseSector(EXTMEM_DRIVER_USER_ObjectTypeDef* UserObject,
                                                                uint32_t Address, uint32_t Size)
{
  // sector erase
  #define CMD_SPI_SE4B 0x21
  EXTMEM_DRIVER_USER_StatusTypeDef retr = EXTMEM_DRIVER_USER_NOTSUPPORTED;
  (void)*UserObject;
  (void)Address;
  (void)Size;
    uint32_t u32EraseAddr = Address;
  XSPI_RegularCmdTypeDef cmd;
  while (u32EraseAddr <= (Address+Size))
  {
    driver_spi_WriteEnable();
    cmd.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG; // HAL_XSPI_OPTYPE_COMMON_CFG,HAL_XSPI_OPTYPE_READ_CFG
    cmd.IOSelect = HAL_XSPI_SELECT_IO_7_0;          // HAL_XSPI_SELECT_IO_7_0
    cmd.Instruction = CMD_SPI_SE4B;
    cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;         // HAL_XSPI_INSTRUCTION_1_LINE, HAL_XSPI_INSTRUCTION_8_LINES
    cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;        // HAL_XSPI_INSTRUCTION_8_BITS,HAL_XSPI_INSTRUCTION_16_BITS
    cmd.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE; // HAL_XSPI_INSTRUCTION_DTR_DISABLE,HAL_XSPI_INSTRUCTION_DTR_ENABLE
    cmd.Address = u32EraseAddr & MEMORY_MASK;
    cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;         // HAL_XSPI_ADDRESS_NONE,HAL_XSPI_ADDRESS_1_LINE,HAL_XSPI_ADDRESS_8_LINES
    cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;       // HAL_XSPI_ADDRESS_32_BITS
    cmd.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE; // HAL_XSPI_ADDRESS_DTR_DISABLE,HAL_XSPI_ADDRESS_DTR_ENABLE
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;           // HAL_XSPI_ALT_BYTES_NONE
    cmd.AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS;        // HAL_XSPI_ALT_BYTES_8_BITS
    cmd.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE; // HAL_XSPI_ALT_BYTES_DTR_DISABLE
    cmd.DataMode = HAL_XSPI_DATA_NONE;                          // HAL_XSPI_DATA_NONE, HAL_XSPI_DATA_1_LINE,HAL_XSPI_DATA_8_LINES
    cmd.DataLength = 0;
    cmd.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE; // HAL_XSPI_DATA_DTR_DISABLE,HAL_XSPI_DATA_DTR_ENABLE
    cmd.DummyCycles = 0;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE; // HAL_XSPI_DQS_DISABLE,HAL_XSPI_DQS_ENABLE
    u32RetVal = HAL_XSPI_Command(&hxspi, &cmd, HAL_TIMEOUT);
    u32RetVal = driver_spi_WriteWait();
    u32EraseAddr += SECTOR_SIZE;
  }
  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

![IMPORTANT]
Be careful about the memory erase address. Is possible it reqeuire block instead of address. 

To thest the the sector erase we will again use `EXTMEM_DRIVER_USER_Init`, hnow we comment the masserase and use sector erase instead

```c
  EXTMEM_DRIVER_USER_EraseSector(NULL,0,4096);
```


```c-nc
  // EXTMEM_DRIVER_USER_MassErase(NULL);
  EXTMEM_DRIVER_USER_EraseSector(NULL,0,4096);
  EXTMEM_DRIVER_USER_Write(NULL,0,(uint8_t*)0x08000000,256);

  retr = EXTMEM_DRIVER_USER_OK;
  return retr;
}
```

The result should be similar like with MassErase function but much faster. You can test it by changing the addresses to be sure we can erase the memory.
The erase aligment is defined by BLOCK size from memory DS in my case 4096.

Now we have a complete driver file. Please comment the MassErase, SectorErase, Prgoram functions in Init function. 
The boodlated can not work directly with this file. Also in case you will create your own IAP project you can use this file too. 

# External loader copy driver 

Now we will use this file also for External loader to not create it 2x. 

We have two options. 

1. Copy content of `extmem_user_driver.c` from Bootloader project to ExtMemLoader project `extmem_user_driver.c`
2. Copy `extmem_user_driver.c`from Bootloader to ExtMemLoader project and rewrite it.

# Update ExternaLOader details

Because we use USER driver we need to fill the memory details by ouselfs

1. Got to ExtMemLoader project
2. Open `Core/Inc/stm32_extmemloader_config.h`

3. Set device address to `0x90000000`

```c
#define	STM32EXTLOADER_DEVICE_ADDR            0x90000000
```

# Build and debug

Now we can build all three projects
You can Debug the application project

You can add also Bootloader alf to application that you can debug both in same time. 


If you get error during loading the issue will be most probalby in external loader.
You can check in that time this wtih CubeProgrammer on maximum verbosity and check in which step we have the issue. 








