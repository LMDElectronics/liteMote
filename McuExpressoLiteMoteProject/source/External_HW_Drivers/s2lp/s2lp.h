/*
 * s2lp.h
 *
 *  Created on: 24 may. 2020
 *      Author: MAX PC
 */

#ifndef MCU_DRIVERS_S2LP_S2LP_H_
#define MCU_DRIVERS_S2LP_S2LP_H_

#include "globals.h"

#define DATA_RADIO_BUFFER_LENGTH 130 //check the s2-lp manual page 63, giving enough room to receive all data sent by device

//action to perform interfacing s2lp
#define ADDRESS_READ_HEADER     0x01
#define ADDRESS_WRITE_HEADER    0x00
#define COMMAND_SEND_HEADER     0x80

//define Freq of the HW XO mounted in MHz
#define XTAL_FREQ       50

//register addresses
#define GPIO0_CONF      0x00
#define GPIO1_CONF      0x01
#define GPIO2_CONF      0x02
#define GPIO3_CONF      0x03

#define IF_OFFSET_ANA   0x09
#define IF_OFFSET_DIG   0x0a

#define IRQ_MASK0       0x53
#define IRQ_MASK1       0x52
#define IRQ_MASK2       0x51
#define IRQ_MASK3       0x50

#define PM_CONFIG0      0x79
#define PM_CONFIG1      0x78
#define PM_CONFIG2      0x77
#define PM_CONFIG3      0x76
#define PM_CONFIG4      0x75

#define SYNTH0          0x08
#define SYNTH1          0x07
#define SYNTH2          0x06
#define SYNTH3          0x05

#define XO_RCO_CONF1  0x6C
#define XO_RCO_CONF0  0x6D

#define CHSPACE         0x0c
#define CHNUM           0x0d

#define DEVICE_INFO1 0xF0
#define DEVICE_INFO0 0xF1

#define MOD4    0x0E
#define MOD3    0x0F
#define MOD2    0x10
#define MOD1    0x11
#define MOD0    0x12

#define CHFLT   0x013

#define AFC0    0x16
#define AFC1    0x15
#define AFC2    0x14

#define RSSI_FLT    0x17
#define RSSI_TH     0x18

#define AGCCTRL5 0x19
#define AGCCTRL4 0x1A
#define AGCCTRL3 0x1B
#define AGCCTRL2 0x1C
#define AGCCTRL1 0x1D
#define AGCCTRL0 0x1E

#define ANT_SELECT_CONF 0x1F

#define CLKREC2 0x20
#define CLKREC1 0x21

#define MC_STATE0   0x8E

#define PA_POWER0   0x62
#define PA_POWER1   0x61
#define PA_POWER2   0x60
#define PA_POWER3   0x5F
#define PA_POWER4   0x5E
#define PA_POWER5   0x5D
#define PA_POWER6   0x5C
#define PA_POWER7   0x5B
#define PA_POWER8   0x5A

#define SYNTH_CONFIG2 0x65

#define PCKTCTRL1   0x30
#define PCKTCTRL2   0x2F
#define PCKTCTRL3   0x2E
#define PCKTCTRL4   0x2D
#define PCKTCTRL5   0x2C
#define PCKTCTRL6   0x2B

#define SYNC_0_REG  0x36
#define SYNC_1_REG  0x35
#define SYNC_2_REG  0x34
#define SYNC_3_REG  0x33

#define PCKTLEN1    0x31
#define PCKTLEN0    0x32

#define QI          0x37

#define FIFO_CONFIG0 0x3F
#define FIFO_CONFIG1 0x3E
#define FIFO_CONFIG2 0x3D
#define FIFO_CONFIG3 0x3C

#define PCKT_FLT_OPTIONS    0x40
#define PCKT_FLT_GOALS4     0x41
#define PCKT_FLT_GOALS3     0x42
#define PCKT_FLT_GOALS2     0x43
#define PCKT_FLT_GOALS1     0x44
#define PCKT_FLT_GOALS0     0x45

#define TIMERS5             0x46
#define TIMERS4             0x47
#define TIMERS3             0x48
#define TIMERS2             0x49
#define TIMERS1             0x4A
#define TIMERS0             0x4B

#define CSMA_CONFIG3        0x4C
#define CSMA_CONFIG2        0x4D
#define CSMA_CONFIG1        0x4E
#define CSMA_CONFIG0        0x4F

#define PROTOCOL0   0x3B
#define PROTOCOL1   0x3A
#define PROTOCOL2   0x39

#define VCO_CONFIG 0x68

#define PA_CONFIG0      0x64
#define PA_CONFIG1      0x63

#define TX_FIFO_STATUS  0x8F
#define RX_FIFO_STATUS  0x90

#define RX_PCKT_LEN1 0xA4
#define RX_PCKT_LEN0 0xA5

#define RX_ADDRE_FIELD1 0xAA
#define RX_ADDRE_FIELD0 0xAB

#define REG_FIFO 0xff

//define data masks
#define B_MASK          0x10 //SYNTH3
#define D_MASK          0x08 //XO_RCO_CONFIG0
#define MODULATION_MASK 0xF0 //MOD2

//define upper and lower limits for RF synth freqs
#define MIN_MIDDLE_BAND_RFSYNTH_FREQ 413
#define MAX_MIDDLE_BAND_RFSYNTH_FREQ 527

#define MIN_HIGH_BAND_RFSYNTH_FREQ 826
#define MAX_HIGH_BAND_RFSYNTH_FREQ 1055

#define BS_DEFAULT 8

//define modulations
#define TWO_FSK         0
#define FOUR_FSK        1
#define TWO_GFSK_BT_1   2
#define FOUR_GFSK_BT_1  3
#define ASK_OOK         5
#define POLAR_MODE      6
#define UNMODULATED     7
#define TWO_GFSK_BT_05  10
#define FOUR_GFSK_BT_05 11

#define DATA_RATE_2_KBPS  2
#define DATA_RATE_25_KBPS  25
#define DATA_RATE_50_KBPS  50
#define DATA_RATE_100_KBPS  100
#define DATA_RATE_200_KBPS  200
#define DATA_RATE_300_KBPS  300
#define DATA_RATE_400_KBPS  400
#define DATA_RATE_500_KBPS  500

#define EUROPEAN_FREQ_BAND 868
#define AMERICAN_FREQ_BAND 915

//STAck packet config data
#define STACK_PREAMBLE_BYTE_PAIRS_MSB_MASK  0x0300
#define STACK_PREAMBLE_BYTE_PAIRS_MSB_SHIFT 0x08
#define STACK_PREAMBLE_BIT_PAIRS            16 //preamble pair bits [0-2046], from s2lp datasheet

#define STACK_SYNC_BITS_MASK                0xFC
#define STACK_SYNC_BITS_SHIFT               0x02
#define STACK_SYNC_BITS                     32 //number of sync bits [0-32], from s2lp datasheet

#define SYNC_0_DATA                         0xF0
#define SYNC_1_DATA                         0xF0
#define SYNC_2_DATA                         0xF0
#define SYNC_3_DATA                         0xF0

#define TX_ACK_MASK                         0x04

#define CRC_MODE_MASK 0xE0
#define CRCMODE0    0
#define CRCMODE1    1
#define CRCMODE2    2
#define CRCMODE3    3
#define CRCMODE4    4
#define CRCMODE5    5

#define DATA_MODE_MASK                  0x0C
#define DATAMODE_NORMAL                 0
#define DATAMODE_DIRECT_THROUGH_FIFO    1
#define DATAMODE_DIRECT_THROUGH_GPIO    2
#define DATAMODE_TEST_PSEUDO_RANDOM     3

#define IRQ_STATUS3 0xFA
#define IRQ_STATUS2 0xFB
#define IRQ_STATUS1 0xFC
#define IRQ_STATUS0 0xFD

#define STATE_MASK  0xFE

//command list
#define TX                  0x60
#define RX                  0x61
#define READY               0x62
#define STANDBY             0x63
#define SLEEP               0x64
#define LOCKRX              0x65
#define LOCKTX              0x66
#define SABORT              0x67
#define LDC_RELOAD          0x68
#define SRES                0x70
#define FLUSHRXFIFO         0x71
#define FLUSHTXFIFO         0x72
#define SEQUENCE_UPDATE     0x73

//state codes
#define STATE_SLEEP_A     0x01
#define STATE_STANDBY     0x02
#define STATE_SLEEP_B     0x03
#define STATE_READY       0x00
#define STATE_LOCK        0x0C
#define STATE_RX          0x30
#define STATE_TX          0x5C
#define STATE_SYNTH_SETUP 0x50

#define TEST_PACKET_LENGHT 20

//interrupt masks according to s2-lp datasheet chapter 9.2
#define RX_DATA_READY                    0x00000001
#define RX_DATA_DISCARDED                0x00000002
#define TX_DATA_SENT                     0x00000004
#define MAX_RETRANSMISSIONS_REACHED      0x00000008
#define CRC_ERROR                        0x00000010
#define TX_FIFO_ERROR                    0x00000020
#define RX_FIFO_ERROR                    0x00000040
#define TX_FIFO_ALMOST_FULL              0x00000080
#define TX_FIFO_ALMOST_EMPTY             0x00000100
#define RX_FIFO_ALMOST_FULL              0x00000200
#define RX_FIFO_ALMOST_EMPTY             0x00000400
#define MAX_BACKOFF_DURING_CCA           0x00000800
#define VALID_PREAMBLE_DETECTED          0x00001000
#define SYNC_WORD_DETECTED               0x00002000
#define RSSI_ABOVE_THRESHOLD             0x00004000
#define WAKEUP_TIMEOUT_IN_LDCR_MODE      0x00008000
#define S2LP_IS_READY                    0x00010000
#define SWITCH_TO_STANDBY_ONGOING        0x00020000
#define LOW_BAT_LEVEL                    0x00040000
#define POWER_ON_RESET                   0x00080000

#define RX_TIMER_TIMEOUT                 0x10000000
#define SNIFF_TIMER_TIMEOUT              0x20000000


typedef struct s2lp_parameters
{
  UINT8 partNum;
  UINT8 version;
}s2lp_parameters;

void S2lp_Init_Pinout(void);
void S2lp_Init(void);

void S2lp_Config_Interrupt(UINT32 intBitsMask);
UINT32 s2lp_Get_IRQ_Mask(void);

void s2lp_Set_Modulation_Type(UINT8 modulation);
UINT8 s2lp_Get_Modulation_Type(void);

UINT8 S2lp_Set_Base_Center_Freq(float baseFreq);
float s2lp_Get_Base_Center_Freq(void);

void s2lp_Set_DataRate(UINT32 dataRate);
float s2lp_Get_DataRate(void);

void s2lp_Set_Channel_Num(UINT8 channelNum);
UINT8 s2lp_Get_Channel_Num(void);

void s2lp_Set_Channel_Space(UINT8 channelSpace);
UINT8 s2lp_Get_Channel_Space(void);

void s2lp_Set_Tx_Power_Config(UINT8 txPowerData);
SINT8 s2lp_Get_Tx_Power_Config(void);

void s2lp_Set_Packet_Format_BASIC(void);
void s2lp_Set_Packet_Format_StAck(void);

void s2lp_Set_Source_Address(UINT8 sourceAddr);
UINT8 s2lp_Get_Source_Address(void);

void s2lp_Set_Destination_Address(UINT8 destinationAddr);
UINT8 s2lp_Get_Destination_Address(void);

void s2lp_Set_Packet_Length(UINT16 dataPacketLength);
UINT16 s2lp_Get_Tx_Packet_Length(void);
UINT16 s2lp_Get_Received_Packet_Length(void);
UINT8 s2lp_Get_Packet_Received_Address(void);

void s2lp_Set_Src_Addr_Filt_Addr(UINT8 scrAddrMask);
UINT8 s2lp_Get_Src_Addr_Filt_Addr(void);

void s2lp_Enable_Ack_For_Tx_Packet(void);
void s2lp_Disable_Ack_For_Tx_Packet(void);

void s2lp_Set_CRC_Mode(UINT8 crcMode);
UINT8 s2lp_Get_CRC_Mode(void);

void s2lp_Set_Tx_Source_Data_Mode(UINT8 dataMode);
UINT8 s2lp_Get_Tx_Source_Data_Mode(void);

void s2lp_Load_Tx_FIFO(UINT8 *dataBuffer, UINT8 byteCount);
void s2lp_Retrieve_Rx_FIFO_Data(UINT8 bytesToRead, UINT8 *dataBuffer);

UINT8 s2lp_Get_Tx_FIFO_Elements();
UINT8 s2lp_Get_Rx_FIFO_Elements();

UINT8 s2lp_Set_Operating_State(UINT8 newStateCommand);
UINT8 s2lp_Get_Operating_State(void);

void s2lp_Start_Tx(void);

void s2lp_Clear_IrqStatus(void);
UINT32 s2lp_Check_IrqStatus(void);

void S2lp_Enable_ShutDown_Mode(void);
void S2lp_Disable_ShutDown_Mode(void);

void S2lp_SPI_Callback(volatile UINT8 *dataReceived);
UINT8 S2lp_Operation_Finished(void);

UINT8 S2lp_Read_Register(UINT8 registerToRead);
UINT8 S2lp_Get_Register_Data_Read(void);

void S2lp_Write_Register(UINT8 registerToWrite, UINT8 dataToWrite);
void S2lp_Send_Command(UINT8 commandToSend);

UINT32 s2lp_GetPacketsTx(void);
void s2lp_ResetPacketsTx(void);

UINT8 s2lp_Get_PacketReceivedFlag(void);
void s2lp_Clear_PacketReceivedFlag(void);

void s2lp_Config_Test_Registers(void);
void S2lp_Test(void);

#endif /* MCU_DRIVERS_S2LP_S2LP_H_ */
