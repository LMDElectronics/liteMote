/*
 * spi.c
 *
 *  Created on: 4 oct. 2019
 *      Author: MAX PC
 */
#include <External_HW_Drivers/mx25l128/mx25l128.h>
#include <External_HW_Drivers/s2lp/s2lp.h>
#include <MCU_Drivers/spi/spi.h>

volatile uint32_t bytes_to_Tx = 0;
volatile uint32_t bytes_transmitted = 0;
volatile uint8_t device_enabled = 0;
volatile uint8_t header = 0;

//pointers to acces to external data buffers independently
volatile uint8_t *txrx_Buffer_SPI1;
volatile uint8_t *txrx_Buffer_SPI0;

//*****************************************************************************
void SPI_Init_PinOut(SPI_Type *spiPeripheralNumber)
//*****************************************************************************
//
//*****************************************************************************
{
  //using SPI1
  if(spiPeripheralNumber == SPI1)
  {
    /* PTE Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE1 (pin 2) is configured as SP1_CLK */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt2);

    /* PORTE2 (pin 3) is configured as SPI1_SOUT */
    PORT_SetPinMux(PORTE, 2U, kPORT_MuxAlt2);

    /* PORTE4 (pin 7) is configured as SPI1_SIN */
    PORT_SetPinMux(PORTE, 4U, kPORT_MuxAlt2);
  }

  //using SPI0
  if(spiPeripheralNumber == SPI0)
  {
    /* PTC Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTC5 (pin 62) is configured as SP0_CLK */
    PORT_SetPinMux(PORTC, 5U, kPORT_MuxAlt2);

    /* PORTC6 (pin 63) is configured as SPI1_SOUT */
    PORT_SetPinMux(PORTC, 6U, kPORT_MuxAlt2);

    /* PORTC (pin 64) is configured as SPI1_SIN */
    PORT_SetPinMux(PORTC, 7U, kPORT_MuxAlt2);
  }
}

//*****************************************************************************
void SPI_ConfigHW_SPI1(void)
//*****************************************************************************
// Inits the spi1 module
//*****************************************************************************
{
  //uint32_t spi_clk_freq = 0;

  //SPI1 interrupt is not part of the 32 formerly interrupts routed directly in arm m0 core, its needed to route it through
  //the imux peripheral
  //init the IMUX peripheral
  CLOCK_EnableClock(kCLOCK_Intmux0);
  INTMUX0->CHANNEL[0].CHn_CSR |= INTMUX_CHn_CSR_RST_MASK;
  //NVIC_SetPriority(INTMUX0_0_IRQn, 0U);
  NVIC_EnableIRQ(INTMUX0_0_IRQn);

  //set the logic of the interupts attached to the imux0 peripheral, OR logic selected
  INTMUX0->CHANNEL[0].CHn_CSR = INTMUX_CHn_CSR_AND(0);

  //wire interrupt
  INTMUX0->CHANNEL[0].CHn_IER_31_0 |= (1U << ((uint32_t)SPI1_IRQn - FSL_FEATURE_INTMUX_IRQ_START_INDEX));

  //open clock for SPI1 peripheral
  CLOCK_EnableClock(kCLOCK_Spi1);

  DSPI1->MCR = 0;

  DSPI1->MCR |= SPI_MCR_DIS_TXF(1);         // Disable Tx FIFO
  DSPI1->MCR |= SPI_MCR_DIS_RXF(1);         // Disable Rx FIFO

  DSPI1->MCR |= SPI_MCR_CLR_TXF(1);         // Clears Tx FIFO
  DSPI1->MCR |= SPI_MCR_CLR_RXF(1);         // Clears Rx FIFO

  //spi master module configuration
  DSPI1->MCR |= SPI_MCR_MSTR(TRUE);         // set master mode
  DSPI1->MCR |= SPI_MCR_CONT_SCKE(FALSE);   // continous clock off
  DSPI1->MCR |= SPI_MCR_DCONF(0x00);        // SPI mode
  DSPI1->MCR |= SPI_MCR_FRZ(FALSE);         // Do not halt spi transfers in debug mode
  DSPI1->MCR |= SPI_MCR_MTFE(0);            // Modified transfer format disable
  DSPI1->MCR |= SPI_MCR_PCSSE(0);           // Used as peripheral chip select
  DSPI1->MCR |= SPI_MCR_ROOE(1);            // Incoming data shifted to shift register
  DSPI1->MCR |= SPI_MCR_PCSIS(0xfff);       // Inactive PCS signal is 1
  DSPI1->MCR |= SPI_MCR_DOZE(0);            // Doze has no effect on the module
  DSPI1->MCR |= SPI_MCR_MDIS(0);            // Enable module clocks

  DSPI1->MCR |= SPI_MCR_SMPL_PT(0x00);      // Sample point for slave -> master sent data
  DSPI1->MCR |= SPI_MCR_HALT(1);            // Stop transfers

  //clock and tranfer module configuration, using CTAR0
  DSPI1->CTAR[CTAR_0] = 0;

  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_DBR(0);      // clk duty 50/50
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_FMSZ(7);     // 8 bit frame size
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_CPOL(0);     // sclk inactive signal value "1" logic
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_CPHA(0);     // data captured in _/- edge and changed in -\_ edge

  //all configurable prescaler delays configured to minimum
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_PCSSCK(0);
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_PASC(0);
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_PDT(0);

  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_PBR(3);

  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_CSSCK(0);

  // baud rate clock prescaler SCK baud rate = (fP /PBR) x [(1+DBR)/BR]
  DSPI1->CTAR[CTAR_0] |= SPI_CTAR_BR(0);

  //Tx and Rx operations are enabled
  DSPI1->SR |= SPI_SR_TXRXS(1);

  //clear Rx interrupt flag
  DSPI1->SR |= SPI_SR_RFDF(1);

  //enabling interrupts
  NVIC_SetPriority(SPI1_IRQn, 0U);
  NVIC_EnableIRQ(SPI1_IRQn);
}

//*****************************************************************************
void SPI_ConfigHW_SPI0(void)
//*****************************************************************************
// Inits the spi1 module
//*****************************************************************************
{
  //open clock for SPI1 peripheral
  CLOCK_EnableClock(kCLOCK_Spi0);

  DSPI0->MCR = 0;

  DSPI0->MCR |= SPI_MCR_DIS_TXF(1);         // Disable Tx FIFO
  DSPI0->MCR |= SPI_MCR_DIS_RXF(1);         // Disable Rx FIFO

  DSPI0->MCR |= SPI_MCR_CLR_TXF(1);         // Clears Tx FIFO
  DSPI0->MCR |= SPI_MCR_CLR_RXF(1);         // Clears Rx FIFO

  //spi master module configuration
  DSPI0->MCR |= SPI_MCR_MSTR(TRUE);         // set master mode
  DSPI0->MCR |= SPI_MCR_CONT_SCKE(FALSE);   // continous clock off
  DSPI0->MCR |= SPI_MCR_DCONF(0x00);        // SPI mode
  DSPI0->MCR |= SPI_MCR_FRZ(FALSE);         // Do not halt spi transfers in debug mode
  DSPI0->MCR |= SPI_MCR_MTFE(0);            // Modified transfer format disable
  DSPI0->MCR |= SPI_MCR_PCSSE(0);           // Used as peripheral chip select
  DSPI0->MCR |= SPI_MCR_ROOE(1);            // Incoming data shifted to shift register
  DSPI0->MCR |= SPI_MCR_PCSIS(0xfff);       // Inactive PCS signal is 1
  DSPI0->MCR |= SPI_MCR_DOZE(0);            // Doze has no effect on the module
  DSPI0->MCR |= SPI_MCR_MDIS(0);            // Enable module clocks

  DSPI0->MCR |= SPI_MCR_SMPL_PT(0x00);      // Sample point for slave -> master sent data
  DSPI0->MCR |= SPI_MCR_HALT(1);            // Stop transfers

  //clock and tranfer module configuration, using CTAR0
  DSPI0->CTAR[CTAR_0] = 0;

  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_DBR(0);      // clk duty 50/50
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_FMSZ(7);     // 8 bit frame size
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_CPOL(0);     // sclk inactive signal value "1" logic
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_CPHA(0);     // data captured in _/- edge and changed in -\_ edge

  //all configurable prescaler delays configured to minimum
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_PCSSCK(0);
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_PASC(0);
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_PDT(0);

  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_PBR(3);

  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_CSSCK(0);

  // baud rate clock prescaler SCK baud rate = (fP /PBR) x [(1+DBR)/BR]
  DSPI0->CTAR[CTAR_0] |= SPI_CTAR_BR(0);

  //Tx and Rx operations are enabled
  DSPI0->SR |= SPI_SR_TXRXS(1);

  //clear Rx interrupt flag
  DSPI0->SR |= SPI_SR_RFDF(1);

  //enabling interrupts
  NVIC_SetPriority(SPI0_IRQn, 1U);
  NVIC_EnableIRQ(SPI0_IRQn);
}


//*****************************************************************************
UINT32 SPI_GetClockFrequency(SPI_Type *spiPeripheralNumber)
//*****************************************************************************
// description: Gets the SPI freq according to configured system freq
// extracted from: kl82 reference manual, chapter 48, page 1395
//
// returns 0 if clock cannot be calculated
//*****************************************************************************
{
  uint8_t pbr_value = 0;
  uint8_t br_value  = 0;
  uint8_t dbr_value = 0;

  uint32_t spi_clk_value = 0;

  pbr_value = 2 << ((spiPeripheralNumber->CTAR[CTAR_0] & SPI_CTAR_PBR_MASK) >> (SPI_CTAR_PBR_SHIFT));
  br_value  = 2 << ((spiPeripheralNumber->CTAR[CTAR_0] & SPI_CTAR_BR_MASK) >> (SPI_CTAR_BR_SHIFT));
  dbr_value = ((spiPeripheralNumber->CTAR[CTAR_0] & SPI_CTAR_DBR_MASK) >> (SPI_CTAR_DBR_SHIFT));

  if((pbr_value > 0) && (br_value > 0))
  {
    spi_clk_value = ((CLOCK_GetFreq(SYS_CLK)/(pbr_value*br_value))*(1+dbr_value));
  }
  else
  {
    spi_clk_value = 0;
  }

  return spi_clk_value;
}

//*****************************************************************************
UINT32 SPI_GetStatusFlag(SPI_Type *spiPeripheralNumber)
//*****************************************************************************
// returns the spi flag indicated in "uint8_t Flag"
// SPI1 flags from spi module:
//*****************************************************************************
{
  return spiPeripheralNumber->SR;
}

//*****************************************************************************
void SPI_Enable_Module(SPI_Type *spiPeripheralNumber)
//*****************************************************************************
//
//*****************************************************************************
{
  spiPeripheralNumber->MCR &= ~SPI_MCR_HALT_MASK;
}

//*****************************************************************************
void SPI_System_Init(void)
//*****************************************************************************
//
//*****************************************************************************
{
  //set pinout
  SPI_Init_PinOut(SPI1);
  SPI_Init_PinOut(SPI0);

  //configuring SPI peripherals
  SPI_ConfigHW_SPI1();
  SPI_ConfigHW_SPI0();

  SPI_GetStatusFlag(SPI1);

  //enable module
  SPI_Enable_Module(SPI1);
  SPI_Enable_Module(SPI0);
  //DSPI1->MCR &= ~SPI_MCR_HALT_MASK;

}

//*****************************************************************************
uint8_t SPI_Send_And_Receive_Byte_Blocking(SPI_Type *spiPeripheralNumber, uint8_t byte)
//*****************************************************************************
//
//*****************************************************************************
{
  ENTER_ATOMIC();

  //send data
  spiPeripheralNumber->PUSHR |= SPI_PUSHR_TXDATA((UINT16)byte);

  while(((spiPeripheralNumber->SR & SPI_SR_TCF_MASK) >> SPI_SR_TCF_SHIFT) == 0);
  spiPeripheralNumber->SR = SPI_SR_TCF(1);

  LEAVE_ATOMIC();

  return spiPeripheralNumber->POPR;
}

//*****************************************************************************
void SPI_Send_NonBlocking(SPI_Type *spiPeripheralNumber, uint8_t *dataBytes, uint32_t dataLength)
//*****************************************************************************
//
//*****************************************************************************
{
  //enable Rx interrupts  until next transfer
  spiPeripheralNumber->RSER |= SPI_RSER_RFDF_RE_MASK;

  //disable Tx interrupt
  spiPeripheralNumber->RSER &= ~SPI_RSER_TCF_RE_MASK;

  //setting spi data transfer parameters
  bytes_transmitted = 0;
  bytes_to_Tx = dataLength;

  if(spiPeripheralNumber == SPI1) txrx_Buffer_SPI1 = dataBytes;
  if(spiPeripheralNumber == SPI0) txrx_Buffer_SPI0 = dataBytes;

  header = dataBytes[0];

  //sending first byte
  if(spiPeripheralNumber == SPI1) spiPeripheralNumber->PUSHR = SPI_PUSHR_TXDATA((UINT16)txrx_Buffer_SPI1[bytes_transmitted]);
  if(spiPeripheralNumber == SPI0) spiPeripheralNumber->PUSHR = SPI_PUSHR_TXDATA((UINT16)txrx_Buffer_SPI0[bytes_transmitted]);

  bytes_to_Tx--;
  bytes_transmitted++;
}

//*****************************************************************************
void INTMUX0_0_IRQHandler(void)
//*****************************************************************************
// IntMUX0 must call SP1 isr
//*****************************************************************************
{
  uint32_t pendingIrqOffset;

  pendingIrqOffset = INTMUX0->CHANNEL[0].CHn_VEC;
  if (pendingIrqOffset)
  {
      uint32_t isr = *(uint32_t *)(SCB->VTOR + pendingIrqOffset);
      ((void (*)(void))isr)();
  }
}

//*****************************************************************************
void SPI1_IRQHandler(void)
//*****************************************************************************
//
//*****************************************************************************
{
  uint8_t byteRecv = 0;

  //check if byte received
  if(SPI_GetStatusFlag(SPI1) & SPI_SR_RFDF_MASK)
  {
    //loading data received onto data buffer
    byteRecv = (uint8_t)DSPI1->POPR;
    txrx_Buffer_SPI1[bytes_transmitted - 1] = byteRecv;

    //clear Rx interrupt flag
    DSPI1->SR = SPI_SR_RFDF(1);

    if(bytes_to_Tx == 0)
    {

      DSPI1->RSER &= ~SPI_RSER_RFDF_RE_MASK;

      //flush rx fifo
      DSPI1->MCR = (DSPI1->MCR & (~(SPI_MCR_CLR_TXF_MASK | SPI_MCR_CLR_RXF_MASK))) | SPI_MCR_CLR_TXF(1) | SPI_MCR_CLR_RXF(1);

      //call driver callback
      switch(device_enabled)
      {
        case CS_EXTERNAL_FLASH: mx25l128_SPI_Callback(txrx_Buffer_SPI1); break;
        default: break;
      }


    }
    else
    {
      ///check if there are bytes left to be transmitted
      if( bytes_to_Tx > 0 )
      {
        DSPI1->PUSHR = SPI_PUSHR_TXDATA((UINT16)txrx_Buffer_SPI1[bytes_transmitted]);
        bytes_transmitted++;
        bytes_to_Tx--;
      }
    }
  }
}

//*****************************************************************************
void SPI0_IRQHandler(void)
//*****************************************************************************
//
//*****************************************************************************
{
  uint8_t byteRecv = 0;

  //check if byte received
  if(SPI_GetStatusFlag(SPI0) & SPI_SR_RFDF_MASK)
  {
    //loading data received onto data buffer
    byteRecv = (uint8_t)DSPI0->POPR;
    txrx_Buffer_SPI0[bytes_transmitted - 1] = byteRecv;

    //clear Rx interrupt flag
    DSPI0->SR = SPI_SR_RFDF(1);

    if(bytes_to_Tx == 0)
    {

      DSPI0->RSER &= ~SPI_RSER_RFDF_RE_MASK;

      //flush rx fifo
      DSPI0->MCR = (DSPI0->MCR & (~(SPI_MCR_CLR_TXF_MASK | SPI_MCR_CLR_RXF_MASK))) | SPI_MCR_CLR_TXF(1) | SPI_MCR_CLR_RXF(1);

      //call driver callback
      switch(device_enabled)
      {
        case CS_S2LP_RADIO: S2lp_SPI_Callback(txrx_Buffer_SPI0); break;
        default: break;
      }
    }
    else
    {
      ///check if there are bytes left to be transmitted
      if( bytes_to_Tx > 0 )
      {
        DSPI0->PUSHR = SPI_PUSHR_TXDATA((UINT16)txrx_Buffer_SPI0[bytes_transmitted]);
        bytes_transmitted++;
        bytes_to_Tx--;
      }
    }
  }
}

//*****************************************************************************
void Enable_CS(uint8_t device_CS_to_enable)
//*****************************************************************************
// Enables selected CS
//*****************************************************************************
{
  //set variable to unset CS after spi data transfer
  device_enabled = device_CS_to_enable;

  switch(device_CS_to_enable)
  {
    case CS_EXTERNAL_FLASH: GPIO_PortClear(GPIOE, 1u << 5); break;
    case CS_S2LP_RADIO:     GPIO_PortClear(GPIOD, 1u << 4); break;
    default: break;
  }
}

//*****************************************************************************
void Disable_CS(uint8_t device_CS_to_disable)
//*****************************************************************************
// Disables selected CS
//*****************************************************************************
{
  switch(device_CS_to_disable)
  {
    case CS_EXTERNAL_FLASH: GPIO_PortSet(GPIOE, 1u << 5); break;
    case CS_S2LP_RADIO:     GPIO_PortSet(GPIOD, 1u << 4); break;
    default: break;
  }

  //unset CS after spi data transfer
  device_enabled = NO_DEVICE;
}
