/*
 * s2lp.c
 *
 *  Created on: 24 may. 2020
 *      Author: MAX PC
 */

//hardcoding STACK radio packet usage for Tx and Rx

#include <External_HW_Drivers/s2lp/s2lp.h>
#include <MCU_Drivers/spi/spi.h>

UINT8 radioBufferDataTxRx[DATA_RADIO_BUFFER_LENGTH];
UINT8 flag_radio_operation_finished = TRUE;
UINT8 s2lp_spi_header = 0;

s2lp_parameters radioPArams;

//external radio interrupt config
const port_pin_config_t portc16_pin71_config = {/* Internal pull-up resistor is disabled */
                                               kPORT_PullUp,
                                               /* Fast slew rate is configured */
                                               kPORT_FastSlewRate,
                                               /* Passive filter is disabled */
                                               kPORT_PassiveFilterDisable,
                                               /* Open drain is disabled */
                                               kPORT_OpenDrainDisable,
                                               /* Pin is configured as GPIO */
                                               kPORT_MuxAsGpio,
                                               /* Pin Control Register fields [15:0] are not locked */
                                               kPORT_UnlockRegister};

UINT32 packetsTx=0;

UINT8 rx_PacketReceived_Flag = 0;

//*****************************************************************************
void S2lp_SPI_Callback(volatile UINT8 *dataReceived)
//*****************************************************************************
// spi data received callback
//*****************************************************************************
{
  switch(s2lp_spi_header)
  {
    case DEVICE_INFO1: radioPArams.partNum = radioBufferDataTxRx[2]; break;
    case DEVICE_INFO0: radioPArams.version = radioBufferDataTxRx[2]; break;
    default: break;
  }

  s2lp_spi_header = 0;
  flag_radio_operation_finished = TRUE;
  Disable_CS(CS_S2LP_RADIO);
}

//*****************************************************************************
void S2lp_Config_Power_Management(void)
//*****************************************************************************
// configs the power of the s2lp chip, values selected for low power consumption
//*****************************************************************************
{
  UINT8 data=0;

  //SMPS output voltage 1.2V
  //sleep mode B (with FIFO retention)
  //retrieving d value
  //keep reserved bit 3:1 as 1

  //commented for test
  data = S2lp_Read_Register(PM_CONFIG0) & 0x1F;
  S2lp_Write_Register(PM_CONFIG0, data | 0x11);

  //BLD voltage 2.1V
  //disable battery detector circuit
  //bypass LDO for low power mode
  //SMPS output level depends on PM_CONFIG0 SMPS_LVL_MPODE
  data = S2lp_Read_Register(PM_CONFIG1);
  S2lp_Write_Register(PM_CONFIG1, data & 0xF1);

  data = S2lp_Read_Register(PM_CONFIG1);
  S2lp_Write_Register(PM_CONFIG1, data & 0xF1);

  data = S2lp_Read_Register(PM_CONFIG0);

  //test
  S2lp_Write_Register(PM_CONFIG0, 0x42);
  S2lp_Write_Register(PM_CONFIG1, 0x39);
  S2lp_Write_Register(PM_CONFIG2, 0xF4);
  S2lp_Write_Register(PM_CONFIG3, 0x9B);
  //end test
}

//****************************************************************************
float s2lp_Get_Base_Center_Freq(void)
//****************************************************************************
// Gets the center freq, according to register configuration
//****************************************************************************
{
  UINT8 b = 0;
  UINT8 d = 0;
  UINT32 synth_value = 0;

  float fc=0;

  //retrieving BS value
  b = S2lp_Read_Register(SYNTH3);

  if((b & B_MASK) == B_MASK)
  {
    b = 8;
  }
  else
  {
    b = 4;
  }

  //retrieving d value
  d = S2lp_Read_Register(XO_RCO_CONF0);

  if((d & D_MASK) == D_MASK)
  {
    d = 2;
  }
  else
  {
    d = 1;
  }

  //retrieving synth_value
  synth_value = (S2lp_Read_Register(SYNTH3)) & 0x0F;
  synth_value <<= 8;

  synth_value |=   S2lp_Read_Register(SYNTH2);
  synth_value <<= 8;


  synth_value |= S2lp_Read_Register(SYNTH1);
  synth_value <<= 8;

  synth_value |=  S2lp_Read_Register(SYNTH0);

  //andamiajes con la fb del datasheet s2lp
  fc = ((float)(XTAL_FREQ) / (float)((b >> 1)*d));
  fc = fc * synth_value; //1048576 = 2^20
  fc = (fc / 1.048576);

  return fc;
}

//*****************************************************************************
UINT8 S2lp_Set_Base_Center_Freq(float baseFreq)
//*****************************************************************************
// configs the rf synth module of s2lp
// baseFreq in MHz
// D is fixed, refdiv is disabled
// BS will be calculated according "basfreq" param
//*****************************************************************************
{
  float synthValue = 0;
  UINT8 data = 0;
  UINT8 bs = 8;
  UINT8 ds = 0;

  UINT8 synth0=0;
  UINT8 synth1=1;
  UINT8 synth2=2;
  UINT8 synth3=3;

  float numerator = 0;
  float denominator = 1;

  //check the base freq param middle band[413 - 527]Mhz - high band[826 - 1055]Mhz
  data = S2lp_Read_Register(SYNTH3);

  if((baseFreq >= MIN_MIDDLE_BAND_RFSYNTH_FREQ) && (baseFreq <= MAX_MIDDLE_BAND_RFSYNTH_FREQ))
  {
    //RF synth freq inside the middle band, setting the BS field to 1, setting charge pump value PLL_CP_ISEL = 011
    //cleaning the low part of SYNTH3, will be set later
    data = (data & 0xF0) | 0x60 | B_MASK;

    bs = BS_DEFAULT;

    S2lp_Write_Register(SYNTH3, data);
  }
  else
  {
    if((baseFreq >= MIN_HIGH_BAND_RFSYNTH_FREQ) && (baseFreq <= MAX_HIGH_BAND_RFSYNTH_FREQ))
    {
      //RF synth freq inside the high band, setting the BS field to 0, setting charge pump value PLL_CP_ISEL = 011
      //cleaning the low part of SYNTH3, will be set later
      data = ((data & 0xF0) | 0x60) & ~B_MASK;

      bs = BS_DEFAULT >> 1;

      S2lp_Write_Register(SYNTH3, data);
    }
    else
    {
      //failed to set up the freq into RF SYTNH
      return FAILED;
    }
  }

  data = S2lp_Read_Register(XO_RCO_CONF0);
  if((data & D_MASK) == D_MASK)
  {
    ds = 2;
  }
  else
  {
    ds = 1;
  }

  //through baseFreq as a parameter, process to extract the SYNTH data bytes equivalent to generate the base freq
  //synthValue = (baseFreq * 1048576 * (bs >>1)) / (XTAL_FREQ);
  numerator = (float)(baseFreq * (bs >> 1) * ds);
  denominator = (float)(XTAL_FREQ);

  synthValue = (float)(numerator/denominator);
  synthValue = synthValue * (1<<20);

  S2lp_Write_Register(SYNTH0, (UINT8)((UINT32)synthValue & 0x000000FF));
  S2lp_Write_Register(SYNTH1, (UINT8)(((UINT32)synthValue & 0x0000FF00) >> 8));
  S2lp_Write_Register(SYNTH2, (UINT8)(((UINT32)synthValue & 0x00FF0000) >> 16));

  //loading charge current
  data = S2lp_Read_Register(SYNTH3);
  S2lp_Write_Register(SYNTH3, data | ((UINT8)(((UINT32)synthValue & 0x0F000000) >> 24)));

  //test
  synth0 = S2lp_Read_Register(SYNTH0);
  synth1 = S2lp_Read_Register(SYNTH1);
  synth2 = S2lp_Read_Register(SYNTH2);
  synth3 = S2lp_Read_Register(SYNTH3);
  //end test

   return OK;
}

//*****************************************************************************
void s2lp_Set_Modulation_Type(UINT8 modulation)
//*****************************************************************************
// Sets the RF modulation to use
//*****************************************************************************
{
  UINT8 data=0;
  UINT8 modulationFilter = 0;

  data = S2lp_Read_Register(MOD2);

  data &= ~MODULATION_MASK; //clear modulation data

  switch(modulation)
  {
    case TWO_FSK:
      data = data | 0x00;
    break;

    case FOUR_FSK:
      data = data | 0x10;
    break;

    case TWO_GFSK_BT_1:
      data = data | 0x20;
    break;

    case FOUR_GFSK_BT_1:
      data = data | 0x30;
      modulationFilter = 1;
    break;

    case ASK_OOK:
      data = data | 0x50;
    break;

    case POLAR_MODE:
      data = data | 0x60;
    break;

    case UNMODULATED:
      data = data | 0x70;
    break;

    case TWO_GFSK_BT_05:
      data = data | 0xA0;
    break;

    case FOUR_GFSK_BT_05:
      data = data | 0xB0;
    break;

    default: break;
  }

  S2lp_Write_Register(MOD2, data);

  //if modulation is 4gfsk, activating the dual pass equalizer
  if(modulationFilter)
  {
    data = S2lp_Read_Register(ANT_SELECT_CONF);
    data &= 0x9F;
    data |= 0x40;

    S2lp_Write_Register(ANT_SELECT_CONF, data);
  }
}

//*****************************************************************************
UINT8 s2lp_Get_Modulation_Type(void)
//*****************************************************************************
// Gets the modulation type
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(MOD2);
  data &= MODULATION_MASK;
  data >>= 4;

  return data;
}

//*****************************************************************************
void s2lp_Set_DataRate(UINT32 dataRate)
//*****************************************************************************
// Sets the data rate (kbits per second) forcing DATARATE_E = 15
//*****************************************************************************
{
  UINT8 data=0;
  UINT16 dataRate_m = 1;
  float dataRate_m_f = 1;
  UINT8 modulation = 0;
  UINT8 xMultiplier = 1;
  float dataRateRead=0;
  UINT8 datarate_e=0;
  UINT32 exp=1;
  float numerator = 0;
  float denominator = 1;

  UINT8 mod0=0;
  UINT8 mod1=0;
  UINT8 mod2=0;
  UINT8 mod3=0;
  UINT8 mod4=0;

  UINT8 afc2 = 0;
  UINT8 afc1 = 0;
  UINT8 afc0 = 0;

  //Get modulation to check if its 2 or 4 FSK or GFSK to produce the kbps from symbol per second former datarate
  modulation = s2lp_Get_Modulation_Type();

  //using 2 bytes to represent the four symbols
  if((modulation == FOUR_FSK) || (modulation == FOUR_GFSK_BT_1) || (modulation == FOUR_GFSK_BT_05))
  {
    xMultiplier = 2;
  }

  //Using equation 2 from 5.4.5 datasheet DATARATE_E > 0 [1-14] the algorithm will loop against DATARATE_E to found the first
  //valid datarate that match with the datarate input parameter
  for(datarate_e = 1; datarate_e < 15; datarate_e++)
  {
    //calculate datarate_m
    numerator = (float)((dataRate/xMultiplier) * (float)(1 << (33 - datarate_e)));
    denominator = (float)(XTAL_FREQ * 500);
    dataRate_m_f = (float)((numerator / denominator) - (1<<16));

    if((dataRate_m_f < 65535) && (dataRate_m_f > 0))
    {
      //valid datarate_m found, exit loop
      break;
    }
  }

  dataRate_m = (UINT16)(dataRate_m_f);

  if((dataRate_m_f / dataRate_m) > 1)
  {
    dataRate_m = (UINT16)(dataRate_m_f + 1);
  }

  S2lp_Write_Register(MOD3, (UINT8)(dataRate_m & 0x00FF));
  S2lp_Write_Register(MOD4, (UINT8)((dataRate_m & 0xFF00) >> 8));

  data = S2lp_Read_Register(MOD2);

  data &= MODULATION_MASK;
  data |= datarate_e;
  S2lp_Write_Register(MOD2, data);

  //test
  mod0 = S2lp_Read_Register(MOD0);
  mod1 = S2lp_Read_Register(MOD1);
  mod2 = S2lp_Read_Register(MOD2);
  mod3 = S2lp_Read_Register(MOD3);
  mod4 = S2lp_Read_Register(MOD4);

  S2lp_Write_Register(AFC2, 0xC8);
  S2lp_Write_Register(AFC1, 0x18);
  S2lp_Write_Register(AFC0, 0x25);

  afc2 = S2lp_Read_Register(AFC2);
  afc1 = S2lp_Read_Register(AFC1);
  afc0 = S2lp_Read_Register(AFC0);
  //end test

  dataRateRead = s2lp_Get_DataRate();
  dataRateRead=0;
}

//*****************************************************************************
float s2lp_Get_DataRate(void)
//*****************************************************************************
// Gets the data rate in kbps
//*****************************************************************************
{
  UINT16 dataRate_m = 0;
  UINT8 dataRate_e = 0;

  float dataRate = 0;
  UINT8 xMultiplier = 1;
  UINT8 modulation = 0;

  //read DATARATE_M
  dataRate_m = S2lp_Read_Register(MOD4);
  dataRate_m <<= 8;
  dataRate_m |= S2lp_Read_Register(MOD3);

  //read DATARATE_E
  dataRate_e = S2lp_Read_Register(MOD2);
  dataRate_e &= ~MODULATION_MASK;

  //Get modulation to check if its 2 or 4 FSK or GFSK to produce the kbps from symbol per second former datarate
  modulation = s2lp_Get_Modulation_Type();

  if((modulation == FOUR_FSK) || (modulation == FOUR_GFSK_BT_1) || (modulation == FOUR_GFSK_BT_05))
  {
    xMultiplier = 2;
  }


  //depending on datarate_e the equation to extract datarate changes according to datasheet p. 32 section 5.4.5
  if(dataRate_e == 0)
  {
    dataRate = (((XTAL_FREQ / 2) * (dataRate_m)) / (4294967296));
  }
  else
  {
    if(dataRate_e < 15)
    {
      dataRate = ((XTAL_FREQ * 500) * ((float)((dataRate_m + (1 << 16))) / (float)(1 << (33 - dataRate_e))));
    }
    else
    {
      if(dataRate_e == 15)
      {
        //125000 = 1000000 / 8, reducing the Mhz factor on equation
        dataRate = ((XTAL_FREQ / 2) * (31.25))/(dataRate_m);
      }
    }
  }

  //getting the kbps from symbols per second
  dataRate = dataRate  * xMultiplier;

  return dataRate;
}

//*****************************************************************************
void s2lp_Set_Channel_Num(UINT8 channelNum)
//*****************************************************************************
// Description: Sets the data channel
//*****************************************************************************
{
  //write channel number
  S2lp_Write_Register(CHNUM, channelNum);
}

//*****************************************************************************
UINT8 s2lp_Get_Channel_Num(void)
//*****************************************************************************
// Description: Gets the data channel
//*****************************************************************************
{
  return S2lp_Read_Register(CHNUM);
}

//*****************************************************************************
void s2lp_Set_Channel_Space(UINT8 channelSpace)
//*****************************************************************************
// Description: Sets the data channel
//*****************************************************************************
{
  //write channel number
  S2lp_Write_Register(CHSPACE, channelSpace);
}

//*****************************************************************************
UINT8 s2lp_Get_Channel_Space(void)
//*****************************************************************************
// Description: Gets the data channel
//*****************************************************************************
{
  return S2lp_Read_Register(CHSPACE);
}

//*****************************************************************************
void s2lp_Set_Tx_Power_Config(UINT8 txPowerData)
//*****************************************************************************
// Description: Sets the data channel
//*****************************************************************************
{
  //set Tx power according to power configuration
  //0xEF limiting to 7 bits
  S2lp_Write_Register(PA_POWER8, txPowerData & 0xEF);
}

//*****************************************************************************
SINT8 s2lp_Get_Tx_Power_Config(void)
//*****************************************************************************
// Description: Sets the data channel
//*****************************************************************************
{
  return S2lp_Read_Register(PA_POWER8);
}

//*****************************************************************************
void s2lp_Set_Packet_Length(UINT16 dataPacketLength)
//*****************************************************************************
// Sets the TX packet length
//*****************************************************************************
{
  UINT8 regData=0;

  regData = S2lp_Read_Register(PCKTCTRL4);

  //check if address is included in the radio packet
  if((regData & 0x08) == 0x08)
  {
    //check the address bytes
    if((regData & 0x80) == 0x80)
    {
      //need to add 2 bytes of address to payloadlenght
      dataPacketLength = dataPacketLength + 2;
    }
    else
    {
      //need to add 1 bytes of address to payloadlenght
      dataPacketLength = dataPacketLength + 1;
    }
  }

  S2lp_Write_Register(PCKTLEN1, (UINT8)((dataPacketLength & 0xFF00) >> 8));
  S2lp_Write_Register(PCKTLEN0, (UINT8)(dataPacketLength & 0x00FF));
}

//*****************************************************************************
UINT16 s2lp_Get_Packet_Length(void)
//*****************************************************************************
// Gets the Tx packetLength in bytes per packet
//*****************************************************************************
{
  UINT16 packetDataLength = 0;

  packetDataLength = S2lp_Read_Register(PCKTLEN1);
  packetDataLength <<= 8;
  packetDataLength |= S2lp_Read_Register(PCKTLEN0);

  return(packetDataLength);
}

//*****************************************************************************
void s2lp_Set_Src_Addr_Filt_Addr(UINT8 scrAddrMask)
//*****************************************************************************
// Sets the Rx destination address for filtering purposes
//*****************************************************************************
{
  S2lp_Write_Register(PCKT_FLT_GOALS4, scrAddrMask);
}

//*****************************************************************************
UINT8 s2lp_Get_Src_Addr_Filt_Addr(void)
//*****************************************************************************
// Gets the Rx destination address for filtering purposes
//*****************************************************************************
{
  S2lp_Read_Register(PCKT_FLT_GOALS4);

  return S2lp_Get_Register_Data_Read();
}

//*****************************************************************************
void s2lp_Set_Address_Type_Packet_Filter(UINT8 addrFilterType)
//*****************************************************************************
// Sets the filter addr type
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(PROTOCOL1);
  data |= 0x01;

  S2lp_Write_Register(PCKT_FLT_OPTIONS, addrFilterType);
}

//*****************************************************************************
void s2lp_Enable_Ack_For_Tx_Packet(void)
//*****************************************************************************
// Enables the need for an ack to the Tx packet
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(PROTOCOL0);
  data |= TX_ACK_MASK;

  S2lp_Write_Register(PROTOCOL0, data);
}

//*****************************************************************************
void s2lp_Disable_Ack_For_Tx_Packet(void)
//*****************************************************************************
// Disables the need for an ack to the Tx packet
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(PROTOCOL0);
  data &= ~TX_ACK_MASK;

  S2lp_Write_Register(PROTOCOL0, data);
}

//*****************************************************************************
UINT8 s2lp_Get_CRC_Mode(void)
//*****************************************************************************
// Sets the CRCMODE for data packet
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(PCKTCTRL1);
  return ((data & CRC_MODE_MASK) >> 5);
}

//*****************************************************************************
UINT8 s2lp_Get_Tx_Source_Data_Mode(void)
//*****************************************************************************
// Sets the CRCMODE for data packet
//*****************************************************************************
{
  UINT8 data = 0;

  data = S2lp_Read_Register(PCKTCTRL1);

  return ((data & DATA_MODE_MASK) >> 2);
}

//*****************************************************************************
void s2lp_Load_Tx_FIFO(UINT8 *dataBuffer, UINT8 byteCount)
//*****************************************************************************
// Fill the s2lp Tx fifo
//*****************************************************************************
{
  UINT8 i=0;
  UINT8 data[10];
  UINT8 elements = 0;
  UINT8 bytesToRead=10;

  //check current data in the fifo
  /*for(i=0; i < bytesToRead; i++)
  {
    data[i] = S2lp_Read_Register(REG_FIFO);
  }

  elements = s2lp_Get_Tx_FIFO_Elements();*/

  //fill fifo
  for(i=0; i < byteCount; i++)
  {
    S2lp_Write_Register(REG_FIFO, dataBuffer[i]);
  }

  /*elements = s2lp_Get_Tx_FIFO_Elements();

  //check current data in the fifo
  for(i=0; i < bytesToRead; i++)
  {
    data[i] = S2lp_Read_Register(REG_FIFO);
  }
  i=0;*/
}

//*****************************************************************************
void s2lp_Retrieve_Rx_FIFO_Data(UINT8 bytesToRead, UINT8 *dataBuffer)
//*****************************************************************************
// description: retrieves data from Rx FIFO
//*****************************************************************************
{
  UINT8 i=0;

  for(i=0; i < bytesToRead; i++)
  {
    dataBuffer[i] = S2lp_Read_Register(REG_FIFO);
  }
}

//*****************************************************************************
UINT8 s2lp_Get_Tx_FIFO_Elements()
//*****************************************************************************
// description: gets the number of bytes loaded into TX FIFO
//*****************************************************************************
{
  return S2lp_Read_Register(TX_FIFO_STATUS);
}

//*****************************************************************************
UINT8 s2lp_Get_Rx_FIFO_Elements()
//*****************************************************************************
// description: gets the number of bytes loaded into RX FIFO
//*****************************************************************************
{
  return S2lp_Read_Register(RX_FIFO_STATUS);
}

//*****************************************************************************
UINT8 s2lp_Set_Operating_State(UINT8 newStateCommand)
//*****************************************************************************
// Send the new state to be programmed in s2lp
//*****************************************************************************
{
  UINT8 retVal = FAILED;
  UINT8 currentState = 0;

  currentState = s2lp_Get_Operating_State();

  switch(newStateCommand)
  {
    case TX:
    case RX:
    case STANDBY:
    case SLEEP:
    case LOCKRX:
    case LOCKTX:

      switch(currentState)
      {
        case STATE_READY:

          S2lp_Send_Command(newStateCommand);
          retVal = OK;
          break;

        default:
          retVal = FAILED;
          break;
      }

      break;

    case READY:

      switch(currentState)
      {
        case STATE_STANDBY:
        case STATE_SLEEP_A:
        case STATE_SLEEP_B:
        case STATE_LOCK:
          S2lp_Send_Command(newStateCommand);
          retVal = OK;

          break;

        default:
          retVal = FAILED;
          break;
      }

      break;

    case SABORT:
      switch(currentState)
      {
        case STATE_TX:
        case STATE_RX:

          S2lp_Send_Command(newStateCommand);
          retVal = OK;

          break;

        default:
          retVal = FAILED;
          break;
      }
      break;

    case LDC_RELOAD:
    case SRES:
    case FLUSHRXFIFO:
    case FLUSHTXFIFO:
    case SEQUENCE_UPDATE:

      S2lp_Send_Command(newStateCommand);
      retVal = OK;

      break;
  }

  return retVal;
}

//*****************************************************************************
UINT8 s2lp_Get_Operating_State(void)
//*****************************************************************************
// Send the new state to be programmed in s2lp
//*****************************************************************************
{
  //get current state
  return (S2lp_Read_Register(MC_STATE0) >> 1);
}

//*****************************************************************************
void s2lp_Clear_IrqStatus(void)
//*****************************************************************************
// description: read the interrupt status bits
//*****************************************************************************
{
  S2lp_Write_Register(IRQ_STATUS3,0x00);
  S2lp_Write_Register(IRQ_STATUS2,0x00);
  S2lp_Write_Register(IRQ_STATUS1,0x00);
  S2lp_Write_Register(IRQ_STATUS0,0x00);
}

//*****************************************************************************
UINT32 s2lp_Check_IrqStatus(void)
//*****************************************************************************
// description: read the interrupt status bits
//*****************************************************************************
{
  UINT32 data = 0;

  data = S2lp_Read_Register(IRQ_STATUS3);
  data <<=8;

  data |= S2lp_Read_Register(IRQ_STATUS2);
  data <<=8;

  data |= S2lp_Read_Register(IRQ_STATUS1);
  data <<=8;

  data |=  S2lp_Read_Register(IRQ_STATUS0);

  return data;
}

//*****************************************************************************
void s2lp_Config_InterruptPin(void)
//*****************************************************************************
{
  gpio_pin_config_t io_config_Intinput = {kGPIO_DigitalInput, 0};

  //configuring PORTC16 (pin 71) for external interrupt
  PORT_SetPinConfig(PORTC, 16U, &portc16_pin71_config);
  PORT_SetPinInterruptConfig(PORTC, 16U, kPORT_InterruptFallingEdge);
  EnableIRQ(PORTC_IRQn);
  GPIO_PinInit(GPIOC, 16, &io_config_Intinput);
}

//*****************************************************************************
void S2lp_Init_Pinout(void)
//*****************************************************************************
// start the s2lp module pinout
//*****************************************************************************
{
  UINT32 d=0;

  gpio_pin_config_t io_config_output = {kGPIO_DigitalOutput, 0};

  /* PTC Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortC);

  /* PTD Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortD);

  /* PORTC12 (pin 69) is configured as GPIO Output for RF Chip shutdown*/
  PORT_SetPinMux(PORTC, 12, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOC, 12, &io_config_output);

  /* PORTD4 (pin 77) is configured as GPIO  for RF SPI Chip select*/
  PORT_SetPinMux(PORTD, 4, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOD, 4, &io_config_output);
}

//*****************************************************************************
void S2lp_Enable_ShutDown_Mode(void)
//*****************************************************************************
// Set the s2lp in shutown modde
//*****************************************************************************
{
  GPIO_PortSet(GPIOC, 1u << 12);
}

//*****************************************************************************
void S2lp_Disable_ShutDown_Mode(void)
//*****************************************************************************
// Set the s2lp in shutown modde
//*****************************************************************************
{
  GPIO_PortClear(GPIOC, 1u << 12);
}

//*****************************************************************************
void s2lp_default_settings(void)
//*****************************************************************************
// Default internal s2lp radio settings
//*****************************************************************************
{
  //configuring IF_FREQUENCY (around 300Khz, recommended pag 34 from s2lp reference manual datasheet)
  S2lp_Write_Register(IF_OFFSET_ANA, 0x2F);
  S2lp_Write_Register(IF_OFFSET_DIG, 0xC2);

  //set default channel space(should be arounf 100Hz -> CHSPACE[7:0]*(Fxo/2^15))
  s2lp_Set_Channel_Space(0x3F);

  //set default channel number, eq:6 pag 27 of s2lp reference manual datasheet
  s2lp_Set_Channel_Num(0x00);

  //set default channel Rx filter M=1 E=3
  S2lp_Write_Register(CHFLT, 0x13);

  //RSSI filter config
  S2lp_Write_Register(RSSI_FLT, 0xE0);

  //dual pass equalization
  S2lp_Write_Register(ANT_SELECT_CONF, 0x55);

  //packet settings, include address field in packet
  S2lp_Write_Register(PCKTCTRL4, 0x08);

  //almost emptu/full control for TX FIFO
  S2lp_Write_Register(PROTOCOL2,0x44);

  //default interrupt masks
  S2lp_Write_Register(IRQ_MASK3,0x00);
  S2lp_Write_Register(IRQ_MASK2,0x00);
  S2lp_Write_Register(IRQ_MASK1,0x00);
  S2lp_Write_Register(IRQ_MASK0,0x00);

  //PA_POWER registers init by default
  S2lp_Write_Register(PA_POWER8,0x18);
  S2lp_Write_Register(PA_POWER7,0x0C);
  S2lp_Write_Register(PA_POWER6,0x18);
  S2lp_Write_Register(PA_POWER5,0x24);
  S2lp_Write_Register(PA_POWER4,0x30);
  S2lp_Write_Register(PA_POWER3,0x48);
  S2lp_Write_Register(PA_POWER2,0x60);
  S2lp_Write_Register(PA_POWER1,0x00);
  S2lp_Write_Register(PA_POWER0,0x87);

  S2lp_Write_Register(0x63,0x01);     //PA_CONFIG1
  S2lp_Write_Register(0x64,0x88);     //PA_CONFIG0
}

//*****************************************************************************
void S2lp_Init(void)
//*****************************************************************************
// Inits the s2lp radio module HW
//*****************************************************************************
{
  UINT32 d=0;

  UINT8 state = 0;
  UINT8 dataRead = 0;
  UINT8 dataRead2 = 0;

  S2lp_Init_Pinout();

  S2lp_Enable_ShutDown_Mode();

  //11ms to wait for s2lp writing registers
  d=0;
  while( d < 0x000fffff)
  {
   d++;
  }

  S2lp_Disable_ShutDown_Mode();
  d=0;

  //11ms to wait for s2lp writing registers
  while( d < 0x000fffff)
  {
    d++;
  }

  s2lp_Config_InterruptPin();

  //TODO: <<<<Problema al ponerlo en standby o en sleep>>>>
  /*state = s2lp_Get_Operating_State();
  if(state != STATE_STANDBY)
  {
    s2lp_Set_Operating_State(STANDBY);

    //wait to set the s2lp in STANDBY state
    while(1)
    {
      state = s2lp_Get_Operating_State();
      if(state == STATE_STANDBY)
      {
        break;
      }
    }
  }*/

  dataRead = S2lp_Read_Register(XO_RCO_CONF1);

  //setting up the divider for s2lp digital HW
  if((XTAL_FREQ >= 48) && (XTAL_FREQ <= 52))
  {
    //PD_CLKDIV = 0
    //digital clock dividers enabled
    dataRead &= 0xEF;

    //REFDIV = 1
    //dataRead2 |= 0x08;
  }
  else
  {
    if((XTAL_FREQ >= 24) && (XTAL_FREQ <= 26))
    {
      //PD_CLKDIV = 1
      //digital clock dividers disabled
      dataRead |= 0x10;

      //REFDIV = 0
      //dataRead2 &= 0xF7;
    }
    else
    {
      //forced stop!
      while(1);
    }
  }

  S2lp_Write_Register(XO_RCO_CONF1, dataRead);

  //by default internal radio settings
  s2lp_default_settings();

  //test
  s2lp_Clear_IrqStatus();
  s2lp_Check_IrqStatus();
  S2lp_Config_Interrupt(RX_DATA_READY /*VALID_PREAMBLE_DETECTED*/ /*SYNC_WORD_DETECTED*/);
  S2lp_Config_Power_Management();

  //config BASIC packet format
  s2lp_Set_Packet_Format_BASIC();

  //config STACK packet type by default
  //s2lp_Set_Packet_Format_StAck();
}

//*****************************************************************************
UINT8 S2lp_Get_Register_Data_Read(void)
//*****************************************************************************
// must be executed when read register operation finishes
//*****************************************************************************
{
  return radioBufferDataTxRx[2];
}

//*****************************************************************************
void S2lp_Write_Register(UINT8 registerToWrite, UINT8 dataToWrite)
//*****************************************************************************
// Ask for module identification
//*****************************************************************************
{
  flag_radio_operation_finished = FALSE;

  s2lp_spi_header = registerToWrite;

  radioBufferDataTxRx[0] = ADDRESS_WRITE_HEADER;
  radioBufferDataTxRx[1] = registerToWrite;
  radioBufferDataTxRx[2] = dataToWrite;

  //send data via SPI and wait for it to be sent
  Enable_CS(CS_S2LP_RADIO);
  SPI_Send_NonBlocking(SPI0, radioBufferDataTxRx, 3);
  while(S2lp_Operation_Finished() == FALSE);
}


//*****************************************************************************
UINT8 S2lp_Read_Register(UINT8 registerToRead)
//*****************************************************************************
// Ask for module identification
//*****************************************************************************
{
  flag_radio_operation_finished = FALSE;

  s2lp_spi_header = registerToRead;

  radioBufferDataTxRx[0] = ADDRESS_READ_HEADER;
  radioBufferDataTxRx[1] = registerToRead;
  radioBufferDataTxRx[2] = 0x00;

  //send data via SPI and wait for response
  Enable_CS(CS_S2LP_RADIO);
  SPI_Send_NonBlocking(SPI0, radioBufferDataTxRx, 3);
  while(S2lp_Operation_Finished() == FALSE);

  return (S2lp_Get_Register_Data_Read());
}

//*****************************************************************************
void S2lp_Send_Command(UINT8 commandToSend)
//*****************************************************************************
//
//*****************************************************************************
{
  flag_radio_operation_finished = FALSE;

  s2lp_spi_header = commandToSend;

  radioBufferDataTxRx[0] = COMMAND_SEND_HEADER;
  radioBufferDataTxRx[1] = commandToSend;

  Enable_CS(CS_S2LP_RADIO);
  SPI_Send_NonBlocking(SPI0, radioBufferDataTxRx, 2);
  while(S2lp_Operation_Finished() == FALSE);
}

//*****************************************************************************
void S2lp_Config_Interrupt(UINT32 intBitsMask)
//*****************************************************************************
// configs the s2lp interrupts, user can or-ing the masks in s2lp.h
//*****************************************************************************
{
  UINT8 dataRead=0;

  UINT8 irqmask0=0;
  UINT8 irqmask1=0;
  UINT8 irqmask2=0;
  UINT8 irqmask3=0;

  //gipo0 routed as interrupt high power, routed gpio -> GPIO0
  S2lp_Write_Register(GPIO0_CONF, 0x03);
  dataRead = S2lp_Read_Register(GPIO0_CONF);

  S2lp_Write_Register(IRQ_MASK3, (UINT8)(intBitsMask >> 24));
  S2lp_Write_Register(IRQ_MASK2, (UINT8)(intBitsMask >> 16));
  S2lp_Write_Register(IRQ_MASK1, (UINT8)(intBitsMask >> 8));
  S2lp_Write_Register(IRQ_MASK0, (UINT8)(intBitsMask));

  irqmask3 = S2lp_Read_Register(IRQ_MASK3);
  irqmask2 = S2lp_Read_Register(IRQ_MASK2);
  irqmask1 = S2lp_Read_Register(IRQ_MASK1);
  irqmask0 = S2lp_Read_Register(IRQ_MASK0);

  dataRead=0;

  //enable routing interrupt 0 [Rx data ready] to GPIO,
  //read the irq_status is useful to know if the interrupt has been triggered also
  //irq_status registers should be clear to clear the interrupt flag

  //test rx preamble detected
  /*S2lp_Write_Register(IRQ_MASK0, 0x00);
  S2lp_Write_Register(IRQ_MASK1, 0x10); //int 12, valid preamble detected
  S2lp_Write_Register(IRQ_MASK2, 0x00);
  S2lp_Write_Register(IRQ_MASK3, 0x00);*/

  //test rx preamble detected
  /*S2lp_Write_Register(IRQ_MASK0, 0x01); //Rx data ready
  S2lp_Write_Register(IRQ_MASK1, 0x00);
  S2lp_Write_Register(IRQ_MASK2, 0x00);
  S2lp_Write_Register(IRQ_MASK3, 0x00);*/

  //test rx preamble detected
  /*S2lp_Write_Register(IRQ_MASK0, 0x00);
  S2lp_Write_Register(IRQ_MASK1, 0x20); //int 12, valid sync word
  S2lp_Write_Register(IRQ_MASK2, 0x00);
  S2lp_Write_Register(IRQ_MASK3, 0x00);*/

  //test Tx packet sent
  /*S2lp_Write_Register(IRQ_MASK0, 0x04); //tx data sent
  S2lp_Write_Register(IRQ_MASK1, 0x00);
  S2lp_Write_Register(IRQ_MASK2, 0x00);
  S2lp_Write_Register(IRQ_MASK3, 0x00);*/

  //test Tx packet sent
  /*S2lp_Write_Register(IRQ_MASK0, 0x00);
  S2lp_Write_Register(IRQ_MASK1, 0x00);
  S2lp_Write_Register(IRQ_MASK2, 0x00);
  S2lp_Write_Register(IRQ_MASK3, 0x10); //rx timer timeout
  */

}

//*****************************************************************************
UINT32 s2lp_Get_IRQ_Mask(void)
//*****************************************************************************
// Gets the s2lp irq_mask from MSB[31..0]LSB
//*****************************************************************************
{
  UINT32 retVal=0;

  UINT8 msk0;
  UINT8 msk1;
  UINT8 msk2;
  UINT8 msk3;

  msk3 = (S2lp_Read_Register(IRQ_MASK3));

  msk2 = (S2lp_Read_Register(IRQ_MASK2));

  msk1 = (S2lp_Read_Register(IRQ_MASK1));

  msk0 = (S2lp_Read_Register(IRQ_MASK0));

  return (UINT32)( (msk3<<24) || (msk3<<16) || (msk3<<8) || msk0 );
}

//*****************************************************************************
UINT8 S2lp_Operation_Finished(void)
//*****************************************************************************
// check for operation finished
//*****************************************************************************
{
  return flag_radio_operation_finished;
}

//*****************************************************************************
void s2lp_Set_Packet_Format_BASIC(void)
//*****************************************************************************
// description: sets the radio data packet format BASIC
//*****************************************************************************
{
  //BASIC packet selection
  //preamble 01 selection
  S2lp_Write_Register(PCKTCTRL3,0x01);

  //variable packet length
  S2lp_Write_Register(PCKTCTRL2,0x01);

  //primary sync word selected
  //using 0x8005 poly
  S2lp_Write_Register(PCKTCTRL1,0x40);

  //SYNC word
  S2lp_Write_Register(SYNC_3_REG,SYNC_3_DATA);
  S2lp_Write_Register(SYNC_2_REG,SYNC_2_DATA);
  S2lp_Write_Register(SYNC_1_REG,SYNC_2_DATA);
  S2lp_Write_Register(SYNC_0_REG,SYNC_1_DATA);

  //packet filtering ENABLED
  S2lp_Write_Register(PROTOCOL1,0x01);

  //automatic ack DISABLED
  //NO_ACK=1 in Tx packet (the Tx packet do not need for an ACK response from the receiver)
  S2lp_Write_Register(PROTOCOL0,0x08);

  //PCKT_FLTR_OPTIONS //filter rx packet accepted id crc is ok
  S2lp_Write_Register(PCKT_FLT_OPTIONS,0x41);

  //TODO:

}

//*****************************************************************************
void s2lp_Set_Packet_Format_StAck(void)
//*****************************************************************************
// description: sets the radio data packet format STack
//*****************************************************************************
{
  UINT8 dataRead = 0;
  UINT16 preambleBits = 0;
  UINT8 AFCData = 0;

  UINT8 pckcrtl1=0;
  UINT8 pckcrtl2=0;
  UINT8 pckcrtl3=0;
  UINT8 pckcrtl4=0;
  UINT8 pckcrtl5=0;
  UINT8 pckcrtl6=0;

  //CrC poly 0x8005
  //TXSource: normal mode
  //disable whitening (for test)
  S2lp_Write_Register(PCKTCTRL1, 0x40);
  dataRead = S2lp_Read_Register(PCKTCTRL1);

  //variable packet length
  S2lp_Write_Register(PCKTCTRL2, 0x01);
  dataRead = S2lp_Read_Register(PCKTCTRL2);

  //using STACK packet for radio communication
  //PCKT_FORMAT:    3 -> STACK packet format
  //RX_MODE:        0 -> NORMAL_MODE
  //FSK4_SYM_SWAP:  0 -> S0 = b7b6, S1 = b5b4, S2 = b3b2, S3 = b1b0
  //BYTE_SWAP:      0 -> MSB first
  //Preamble selection pattern [1] -> 1010 for ASK_OOK o 2GFSK, 0010 for 4GFSK
  S2lp_Write_Register(PCKTCTRL3, 0xC1);
  dataRead = S2lp_Read_Register(PCKTCTRL3);

  //add address info data into packet (Rxaddr = 1byte + TxAddr = 1 byte + payload = x bytes)
  //LEND_WID = 0, 1byte for length packet
  //ADDRESS_LEN = 1, including address field in the packet
  S2lp_Write_Register(PCKTCTRL4, 0x08);

  //configuring SYNC bits and PREAMBLE MSB bit pairs in radio packet
  dataRead = 0;
  dataRead = ((STACK_SYNC_BITS << STACK_SYNC_BITS_SHIFT) | ((STACK_PREAMBLE_BIT_PAIRS & STACK_PREAMBLE_BYTE_PAIRS_MSB_MASK) >> STACK_PREAMBLE_BYTE_PAIRS_MSB_SHIFT ));
  S2lp_Write_Register(PCKTCTRL6, dataRead);

  //configuring PREAMBLE LSB bit pairs in radio packet
  dataRead = (UINT8)(STACK_PREAMBLE_BIT_PAIRS & 0x00FF);
  S2lp_Write_Register(PCKTCTRL5, dataRead);

  //configuring the AFC fast period, according to datasheet, its value should be double of the preamble symbols
  preambleBits = STACK_PREAMBLE_BIT_PAIRS << 1;
  dataRead = s2lp_Get_Modulation_Type();

  if((dataRead == TWO_FSK) || (dataRead == TWO_GFSK_BT_1) || (dataRead == TWO_GFSK_BT_05) )
  {
    //2 bits per symbol
    AFCData = (UINT8)(preambleBits >> 1);
    AFCData <<= 1;
    S2lp_Write_Register(AFC1, AFCData);
  }
  else
  {
    if((dataRead == FOUR_FSK) || (dataRead == FOUR_GFSK_BT_1) || (dataRead == FOUR_GFSK_BT_05))
    {
      //4 bits per symbol
      AFCData = (UINT8)(preambleBits >> 2);
      AFCData <<= 1;
      S2lp_Write_Register(AFC1, AFCData);
    }
    else
    {
      //1 bits for symbol
      AFCData = (UINT8)preambleBits;
      AFCData <<= 1;
      S2lp_Write_Register(AFC1, AFCData);
    }
  }

  //configuring SYNCx data registers
  S2lp_Write_Register(SYNC_0_REG, SYNC_0_DATA);
  S2lp_Write_Register(SYNC_1_REG, SYNC_1_DATA);
  S2lp_Write_Register(SYNC_2_REG, SYNC_2_DATA);
  S2lp_Write_Register(SYNC_3_REG, SYNC_3_DATA);

  //test
  pckcrtl6 = S2lp_Read_Register(PCKTCTRL6);
  pckcrtl5 = S2lp_Read_Register(PCKTCTRL5);
  pckcrtl4 = S2lp_Read_Register(PCKTCTRL4);
  pckcrtl3 = S2lp_Read_Register(PCKTCTRL3);
  pckcrtl2 = S2lp_Read_Register(PCKTCTRL2);
  pckcrtl1 = S2lp_Read_Register(PCKTCTRL1);
  dataRead=0;
  //end test
}

//*****************************************************************************
void s2lp_Set_Source_Address(UINT8 sourceAddr)
//*****************************************************************************
// Sets the current configured source addr (my address)
//*****************************************************************************
{
  S2lp_Write_Register(PCKT_FLT_GOALS0, sourceAddr);
}

//*****************************************************************************
UINT8 s2lp_Get_Source_Address(void)
//*****************************************************************************
// Gets the current source address (my address)
//*****************************************************************************
{
  return S2lp_Read_Register(PCKT_FLT_GOALS0);
}

//*****************************************************************************
void s2lp_Set_Destination_Address(UINT8 destinationAddr)
//*****************************************************************************
// Sets the current destination address
//*****************************************************************************

{
  S2lp_Write_Register(PCKT_FLT_GOALS3, destinationAddr);
}

//*****************************************************************************
UINT8 s2lp_Get_Destination_Address(void)
//*****************************************************************************
// Gets the current destination address
//*****************************************************************************
{
  return S2lp_Read_Register(PCKT_FLT_GOALS3);
}

//*****************************************************************************
void s2_lp_TestDataRate_RC(void)
//*****************************************************************************
// description: programs the datarate registers for test
//*****************************************************************************
{
  S2lp_Write_Register(MOD4, 0x3f); //0x92
  S2lp_Write_Register(MOD3, 0x10); //0xA7
  S2lp_Write_Register(MOD2, 0x39); //0x27
}

//*****************************************************************************
UINT8 s2lp_Get_Rx_Timer_Stop_Condition(void)
//*****************************************************************************
// description: gets the Rx timer stop condition according to
// s2lp datasheet table 45
//*****************************************************************************
{
  UINT8 retVal=0;

  retVal = ((S2lp_Read_Register(PCKT_FLT_OPTIONS) >> 3) & 0x08);
  retVal |= ((S2lp_Read_Register(PROTOCOL2) >> 5) & 0x04);
  retVal |= ((S2lp_Read_Register(PROTOCOL2) >> 5) & 0x02);
  retVal |= ((S2lp_Read_Register(PROTOCOL2) >> 5) & 0x01);

  return retVal;
}

//*****************************************************************************
void s2lp_Set_Rx_Timer_Stop_Condition(UINT8 rxTimerStopCondition)
//*****************************************************************************
// description: Sets the Rx timer stop condition according to
// s2lp datasheet table 45
//*****************************************************************************
{
  UINT8 dataRead=0;

  rxTimerStopCondition &= 0x0F;

  //setting up RX_TIMEOUT_AND_OR_SEL
  dataRead = S2lp_Read_Register(PCKT_FLT_OPTIONS);
  (((rxTimerStopCondition & 0x08) != 0) ? (dataRead |= 0x40) : (dataRead &= 0xBF));
  S2lp_Write_Register(PCKT_FLT_OPTIONS, dataRead);

  dataRead = S2lp_Read_Register(PROTOCOL2);

  //setting up CS_TIMEOUT_MASK
  (((rxTimerStopCondition & 0x04) != 0) ? (dataRead |= 0x80) : (dataRead &= 0x7F));

  //setting up SQI_TIMEOUT_MASK
  (((rxTimerStopCondition & 0x02) != 0) ? (dataRead |= 0x40) : (dataRead &= 0xBF));

  //setting up PQI_TIMEOUT_MASK
  (((rxTimerStopCondition & 0x02) != 0) ? (dataRead |= 0x20) : (dataRead &= 0xDF));

  S2lp_Write_Register(PROTOCOL2, dataRead);
}

//*****************************************************************************
void s2lp_Start_Tx(void)
//*****************************************************************************
// Sets the transceiver in Tx mode
//*****************************************************************************
{
  UINT8 opState=0;

  opState = s2lp_Get_Operating_State();

  s2lp_Set_Operating_State(TX);

  opState = s2lp_Get_Operating_State();
}

UINT32 s2lp_GetPacketsTx(void)
{
  return packetsTx;
}

void s2lp_ResetPacketsTx(void)
{
  packetsTx = 0;
}

void s2lp_Config_Test_Registers(void)
{
  /*S2lp_Write_Register(0x00,0x0A); //GPIO0_CONF
  S2lp_Write_Register(0x01,0xA2);   //GPIO1_CONF
  S2lp_Write_Register(0x02,0xA2);   //GPIO2_CONF
  S2lp_Write_Register(0x03,0xA2);*/ //GPIO3_CONF

  /*S2lp_Write_Register(0x05,0x62); //SYNTH3
  S2lp_Write_Register(0x06,0x2B);   //SYNTH2
  S2lp_Write_Register(0x07,0x84);   //SYNTH1
  S2lp_Write_Register(0x08,0x99);*/ //SYNTH0

  /*S2lp_Write_Register(0x09,0x2F);
  S2lp_Write_Register(0x0A,0xC2);*/

  //S2lp_Write_Register(0x0C,0x3F);   //CHSPACE
  //S2lp_Write_Register(0x0D,0x00);   //CHNUM

  /*S2lp_Write_Register(0x0E,0x4F); //MOD4
  S2lp_Write_Register(0x0F,0x8B);   //MOD3
  S2lp_Write_Register(0x10,0x53);   //MOD2
  S2lp_Write_Register(0x11,0x03);   //MOD1
  S2lp_Write_Register(0x12,0xA3);*/ //MOD0

  //S2lp_Write_Register(0x13,0x13); //CHFLT

  //S2lp_Write_Register(0x14,0xC8); //AFC2
  //S2lp_Write_Register(0x15,0x18); //AFC1
  //S2lp_Write_Register(0x16,0x25); //AFC0

  //S2lp_Write_Register(0x17,0xE0); //RSSI filter
  //S2lp_Write_Register(0x18,0x28); //RSSI TH

  /*S2lp_Write_Register(0x19,0x80); //AGCTRL5
  S2lp_Write_Register(0x1A,0x54);   //AGCTRL4
  S2lp_Write_Register(0x1B,0x10);   //AGCTRL3
  S2lp_Write_Register(0x1C,0x22);   //AGCTRL2
  S2lp_Write_Register(0x1D,0x59);   //AGCTRL1
  S2lp_Write_Register(0x1E,0x8C);*/ //AGCTRL0

  //S2lp_Write_Register(0x1F,0x55); //ANT_SELECT_CONF

  /*S2lp_Write_Register(0x20,0xC0); //CLOCKREC2
  S2lp_Write_Register(0x21,0x58);*/ //CLOCKREC1

  /*S2lp_Write_Register(0x2B,0x80);   //PCKTCTRL6
  S2lp_Write_Register(0x2C,0x10);*/   //PCKTCTRL6

  //S2lp_Write_Register(0x2D,0x00); //PCKTCTRL4
  //S2lp_Write_Register(0x2D,0x08); //PCKTCTRL4 including Rx address in the radio packet

  //S2lp_Write_Register(0x2E,0x01); //PCKTCTRL3 determines the packet format used
  //S2lp_Write_Register(0x2F,0x01); //PCKTCTRL2 variable packet length
  //S2lp_Write_Register(0x30,0x20); //PCKTCTRL1 dual sync word is disabled

  //OJO CUIDAO
  /*S2lp_Write_Register(0x31,0x00);   //PCKTLEN1
  S2lp_Write_Register(0x32,0x14);   //PCKTLEN0*/

  /*S2lp_Write_Register(0x33,0xF0);   //SYNC3
  S2lp_Write_Register(0x34,0xF0);   //SYNC2
  S2lp_Write_Register(0x35,0xF0);   //SYNC1
  S2lp_Write_Register(0x36,0xF0);*/   //SYNC0

  /*S2lp_Write_Register(0x37,0x01);   //QI
  S2lp_Write_Register(0x38,0x00);*/   //PCKT_PSTMBL

  /*S2lp_Write_Register(0x39,0x44);*/ //PROTOCOL2

  //S2lp_Write_Register(0x3A,0x01); //PROTOCOL1, AUTO_PCKT_FLT -> autopacket filtering control enabled
  //S2lp_Write_Register(0x3B,0x08); //PROTOCOL0, No ack,

  /*S2lp_Write_Register(0x3C,0x40);
  S2lp_Write_Register(0x3D,0x40);
  S2lp_Write_Register(0x3E,0x40);
  S2lp_Write_Register(0x3F,0x40);*/

  //S2lp_Write_Register(0x40,0x41); //PCKT_FLTR_OPTIONS //filter rx packet accepted id crc is ok
  //S2lp_Write_Register(0x40,0x43); //PCKT_FLTR_OPTIONS //filter rx packet accepted id crc is ok and if rx address matches

  /*S2lp_Write_Register(0x41,0x00); //PCKT_FLT_GOALS4, saving Rx address mask
  S2lp_Write_Register(0x42,0x00);
  S2lp_Write_Register(0x43,0x00);
  S2lp_Write_Register(0x44,0x00);
  S2lp_Write_Register(0x45,0x00);*/

  /*S2lp_Write_Register(0x46,0x01); //TIMERS5
  S2lp_Write_Register(0x47,0x00);
  S2lp_Write_Register(0x48,0x01);
  S2lp_Write_Register(0x49,0x00);
  S2lp_Write_Register(0x4A,0x01);
  S2lp_Write_Register(0x4B,0x00);*/

  /*S2lp_Write_Register(0x4C,0xFF);   //CSMA CONFIG
  S2lp_Write_Register(0x4D,0x00);
  S2lp_Write_Register(0x4E,0x04);
  S2lp_Write_Register(0x4F,0x00);*/

  /*S2lp_Write_Register(0x50,0x00); //IRQMASK
  S2lp_Write_Register(0x51,0x00);
  S2lp_Write_Register(0x52,0x00);
  S2lp_Write_Register(0x53,0x00);*/

  //S2lp_Write_Register(0x54,0x28); //FAST_RX_TIMER

  /*S2lp_Write_Register(0x5A,0x18);
  S2lp_Write_Register(0x5B,0x0C);
  S2lp_Write_Register(0x5C,0x18);
  S2lp_Write_Register(0x5D,0x24);
  S2lp_Write_Register(0x5E,0x30);
  S2lp_Write_Register(0x5F,0x48);
  S2lp_Write_Register(0x60,0x60);
  S2lp_Write_Register(0x61,0x00); //PA_POWER1
  S2lp_Write_Register(0x62,0x87); //PA_POWER0

  S2lp_Write_Register(0x63,0x01);     //PA_CONFIG1
  S2lp_Write_Register(0x64,0x88);     //PA_CONFIG0*/

  /*S2lp_Write_Register(0x65,0xD0);     //SYNTH_CONF2

  S2lp_Write_Register(0x68,0x03);*/       //VCO_CONFIG

  /*S2lp_Write_Register(0x69,0x88);
  S2lp_Write_Register(0x6A,0x40);
  S2lp_Write_Register(0x6B,0x40);

  S2lp_Write_Register(0x6C,0x45);
  S2lp_Write_Register(0x6D,0x30);

  S2lp_Write_Register(0x6E,0x70);
  S2lp_Write_Register(0x6F,0x4D);

  S2lp_Write_Register(0x75,0x17);
  S2lp_Write_Register(0x76,0x20);
  S2lp_Write_Register(0x77,0x00);
  S2lp_Write_Register(0x78,0x39);
  S2lp_Write_Register(0x79,0x42);

  S2lp_Write_Register(0x8D,0x52);
  S2lp_Write_Register(0x8E,0x01);

  S2lp_Write_Register(0x8F,0x00);
  S2lp_Write_Register(0x90,0x00);
  S2lp_Write_Register(0x94,0x70);
  S2lp_Write_Register(0x95,0x00);
  S2lp_Write_Register(0x99,0x00);
  S2lp_Write_Register(0x9A,0x00);
  S2lp_Write_Register(0x9C,0x00);
  S2lp_Write_Register(0x9D,0x00);
  S2lp_Write_Register(0x9E,0x00);
  S2lp_Write_Register(0x9F,0x00);
  S2lp_Write_Register(0xA0,0x00);
  S2lp_Write_Register(0xA2,0x00);
  S2lp_Write_Register(0xA4,0x00);
  S2lp_Write_Register(0xA5,0x00);
  S2lp_Write_Register(0xA6,0x00);
  S2lp_Write_Register(0xA7,0x00);
  S2lp_Write_Register(0xA8,0x00);
  S2lp_Write_Register(0xA9,0x00);
  S2lp_Write_Register(0xAA,0x00);
  S2lp_Write_Register(0xAB,0x00);
  S2lp_Write_Register(0xEF,0x00);
  S2lp_Write_Register(0xF0,0x03);
  S2lp_Write_Register(0xF1,0x91);*/
}

//*****************************************************************************
void S2lp_Test(void)
//*****************************************************************************
// default config for the s2-lp, to start another data transfer system should
// wait for the last to end
//*****************************************************************************
{
  S2lp_Read_Register(DEVICE_INFO1);
  S2lp_Read_Register(DEVICE_INFO0);
}

//*****************************************************************************
UINT8 s2lp_Get_PacketReceivedFlag()
//*****************************************************************************
// Gets the received packet flag value which indicates if a radio packet
// has been received or not
//*****************************************************************************
{
  return rx_PacketReceived_Flag;
}

//*****************************************************************************
void s2lp_Clear_PacketReceivedFlag(void)
//*****************************************************************************
// Sets the received packet flag value which indicates if a radio packet
// has been received or not
//*****************************************************************************
{
  rx_PacketReceived_Flag = FALSE;
}

//*****************************************************************************
void PORTC_IRQHandler(void)
//*****************************************************************************
// External s2lp interrupt pin
//*****************************************************************************
{
  UINT8 i=0;
  UINT32 intTriggered = 0;

  /* Clear external interrupt flag. */
  GPIO_PortClearInterruptFlags(GPIOC, 1U << 16U);
  /* Change state of button. */

  //must check which interrupt has been triggered
  /*intTriggered = s2lp_Check_IrqStatus();
  if(intTriggered != 0)
  {
    //process interrupts
    switch(intTriggered)
    {
      case VALID_PREAMBLE_DETECTED:
        i=2;
        break;
    }
  }
  else
  {
    //no interrupts to process
  }*/

  packetsTx++;
  rx_PacketReceived_Flag = TRUE;

  //s2lp clear interrupts

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
