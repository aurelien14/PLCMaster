/* L230 device descriptor declarations. */

#ifndef L400_DESC_H
#define L400_DESC_H

#include "core/device/device_desc.h"

#include <stdint.h>

static_assert(sizeof(float) == 4, "float must be 32-bit");

#pragma pack(push, 1)
typedef struct
{
	// RxPDO 0x1600 - L230_DO_Byte0
	union {
		uint8_t L400_DO_Byte0;
		struct {
			uint8_t X14_Out0 : 1;	//%OX1.20.0
			uint8_t X15_Out1 : 1;
			uint8_t X16A_Out2 : 1;
			uint8_t X17_Out3 : 1;
			uint8_t X18_Out4 : 1;	//%OX1.20.4
			uint8_t X19_Out5 : 1;
			uint8_t X20_Out6 : 1;
			uint8_t X21_Out7 : 1;	//%OX1.20.7
		} L400_DO_Byte0_bits;
	};

	// RxPDO 0x1601 - L400_DO_Byte1
	union {
		uint8_t L400_DO_Byte1;
		struct {
			uint8_t X49_Out0 : 1;	//%OX1.21.0
			uint8_t X48_Out1 : 1;	//%OX1.21.1
			uint8_t X8_Out2 : 1;	//%OX1.21.2
			uint8_t X9_Out3 : 1;
			uint8_t X10_Out4 : 1;
			uint8_t X11_Out5 : 1;
			uint8_t X12_Out6 : 1;	//%OX1.21.6
			uint8_t X13_Out7 : 1;
		} L400_DO_Byte1_bits;
	};

	// RxPDO 0x1602 - L400_DO_Byte2
	union {
		uint8_t L400_DO_Byte2;
		struct {
			uint8_t X22_Out0 : 1;
			uint8_t X24_Out1 : 1;
			uint8_t X23_Out2 : 1;
			uint8_t X46_Out3 : 1;
			uint8_t X53_Out4 : 1;
			uint8_t X54_Out5 : 1;
			uint8_t X16B_Out6 : 1;
			uint8_t padding : 1;
		} L400_DO_Byte2_bits;
	};

	// RxPDO 0x1603 - L400_DO_Byte3
	union {
		uint8_t L400_DO_Byte3;
		struct {
			uint8_t X76_1_Out0 : 1;
			uint8_t X76_3_Out1 : 1;
			uint8_t X76_5_Out2 : 1;
			uint8_t X76_7_Out3 : 1;
			uint8_t padding : 4;
		} L400_DO_Byte3_bits;
	};

	// RxPDO 0x1604 - X3_Out0_Out7
	union {
		uint8_t X3_Out0_Out7;
		struct {
			uint8_t Out0 : 1;
			uint8_t Out1 : 1;
			uint8_t Out2 : 1;
			uint8_t Out3 : 1;
			uint8_t Out4 : 1;
			uint8_t Out5 : 1;
			uint8_t Out6 : 1;
			uint8_t Out7 : 1;
		} X3_Out0_Out7_bits;
	};

	// RxPDO 0x1605 - X21_CPU_Pt1
	uint32_t X21_CPU_Pt1_Ctrl;
	// RxPDO 0x1606 - X22_CPU_Pt2
	uint32_t X22_CPU_Pt2_Ctrl;
	// RxPDO 0x1607 - X23_CPU_VC1
	uint32_t X23_CPU_VC1_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1608 - X31_DAIO_Pt1
	uint32_t X31_DAIO_Pt1_Ctrl;
	// RxPDO 0x1609 - X32_DAIO_Pt2
	uint32_t X32_DAIO_Pt2_Ctrl;
	// RxPDO 0x160a - X33_DAIO_Pt3
	uint32_t X33_DAIO_Pt3_Ctrl;
	// RxPDO 0x160b - X34_DAIO_Pt4
	uint32_t X34_DAIO_Pt4_Ctrl;
	// RxPDO 0x160c - X35_DAIO_Pt5
	uint32_t X35_DAIO_Pt5_Ctrl;
	// RxPDO 0x160d - X36_DAIO_Pt6
	uint32_t X36_DAIO_Pt6_Ctrl;
	// RxPDO 0x160e - X37_DAIO_Pt7
	uint32_t X37_DAIO_Pt7_Ctrl;
	// RxPDO 0x160f - X38_DAIO_Pt8
	uint32_t X38_DAIO_Pt8_Ctrl;

	// RxPDO 0x1610 - X41_1_DAIO_VC1
	uint32_t X41_1_DAIO_VC1_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1611 - X41_3_DAIO_VC2
	uint32_t X41_3_DAIO_VC2_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1612 - X40_1_DAIO_VC3
	uint32_t X40_1_DAIO_VC3_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1613 - X40_3_DAIO_VC4
	uint32_t X40_3_DAIO_VC4_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1614 - X40_5_DAIO_VC5
	uint32_t X40_5_DAIO_VC5_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1615 - X40_7_DAIO_VC6
	uint32_t X40_7_DAIO_VC6_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
	// RxPDO 0x1616 - X42_1_DAIO_AC1
	uint32_t X42_1_DAIO_AC1_Ctrl;
	// RxPDO 0x1617 - X42_3_DAIO_AC2
	uint32_t X42_3_DAIO_AC2_Ctrl;
	// RxPDO 0x1618 - X5_1_DAIO_Aout1
	uint32_t X5_1_DAIO_Aout1_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
	// RxPDO 0x1619 - X5_3_DAIO_Aout2
	uint32_t X5_3_DAIO_Aout2_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
	// RxPDO 0x161A - X5_5_DAIO_Aout3
	uint32_t X5_5_DAIO_Aout3_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
	// RxPDO 0x161B - X5_7_DAIO_Aout4
	uint32_t X5_7_DAIO_Aout4_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
	// RxPDO 0x161C - X6_1_DAIO_Aout5
	uint32_t X6_1_DAIO_Aout5_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
	// RxPDO 0x161D - X6_3_DAIO_Aout6
	uint32_t X6_3_DAIO_Aout6_Value; //<Comment>HiWord: 8001 = Voltage; 8003 = Current  LoWord: 0V/mA = 0000 .. 0FA00 = 10V/20mA
} L400_RX_PDO_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	// TxPDO 0x1A00 - L400_DI_Byte0
	struct {
		uint8_t X31_In0 : 1;	//0x6080
		uint8_t X32_In1 : 1;	//0x6090
		uint8_t X33_In2 : 1;	//0x60A0
		uint8_t X34_In3 : 1;	//0x60B0
		uint8_t X75_1_In4 : 1;	//0x60C0
		uint8_t X75_3_In5 : 1;	//0x60D0
		uint8_t X75_5_In6 : 1;	//0x60E0
		uint8_t X75_7_In7 : 1;	//0x60F0
	} L400_DI_Byte0;

	// TxPDO 0x1A01 - L400_DI_Byte1
	struct {
		uint8_t X39_In0 : 1;	//0x6100
		uint8_t X42_In1 : 1;	//0x6110
		uint8_t X41_In2 : 1;	//0x6120
		uint8_t X44_In3 : 1;	//0x6130
		uint8_t X38_In4 : 1;	//0x6140
		uint8_t X37_In5 : 1;	//0x6150
		uint8_t X36_In6 : 1;	//0x6160
		uint8_t X35_In7 : 1;	//0x6170
	} L400_DI_Byte1;

	// TxPDO 0x1A02 - L400_DI_Byte2
	struct {
		uint8_t X55_In0 : 1;	//0x6180
		uint8_t padding : 7;
	} L400_DI_Byte2;

	// TxPDO 0x1A03 - X1_In0_In7
	struct {
		uint8_t In0 : 1;		//0x6000
		uint8_t In1 : 1;		//0x6010
		uint8_t In2 : 1;		//0x6020
		uint8_t In3 : 1;		//0x6030
		uint8_t In4 : 1;		//0x6040
		uint8_t In5 : 1;		//0x6050
		uint8_t In6 : 1;		//0x6060
		uint8_t In7 : 1;		//0x6070
	} X1_In0_In7;

	// TxPDO 0x1A04 - X21_CPU_Pt1 
	struct {
		float Pt_Value;	//0x63A0 (value in ohm)
		uint32_t Pt_State;	//0x63B0 (Bit 0 Error PT100)
	} X21_CPU_Pt1;

	// TxPDO 0x1A05 - X21_CPU_Pt2
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x63D0 (Bit 0 Error PT100)
	} X22_CPU_Pt2;

	// TxPDO 0x1A06 - X21_CPU_VC1 
	struct {
		float VC_Value;	//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X23_CPU_VC1;

	// TxPDO 0x1A07 - X31_DAIO_Pt1 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X31_DAIO_Pt1;

	// TxPDO 0x1A08 - X32_DAIO_Pt2 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X32_DAIO_Pt2;

	// TxPDO 0x1A09 - X33_DAIO_Pt3 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X33_DAIO_Pt3;

	// TxPDO 0x1A0a - X34_DAIO_Pt4 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X34_DAIO_Pt4;

	// TxPDO 0x1A0b - X35_DAIO_Pt5 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X35_DAIO_Pt5;

	// TxPDO 0x1A0C - X36_DAIO_Pt6 
	struct {
		float Pt_Value;	//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X36_DAIO_Pt6;

	// TxPDO 0x1A0D - X37_DAIO_Pt7 
	struct {
		float Pt_Value;		//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X37_DAIO_Pt7;

	// TxPDO 0x1A0E - X37_DAIO_Pt7 
	struct {
		float Pt_Value;		//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x69C0  (Bit 0 Error PT100)
	} X38_DAIO_Pt8;

	// TxPDO 0x1A0F - X41_1_DAIO_VC1 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X41_1_DAIO_VC1;

	// TxPDO 0x1A10 - X41_3_DAIO_VC2 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X41_3_DAIO_VC2;

	// TxPDO 0x1A11 - X40_1_DAIO_VC3 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X40_1_DAIO_VC3;

	// TxPDO 0x1A12 - X40_3_DAIO_VC4 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X40_3_DAIO_VC4;

	// TxPDO 0x1A13 - X40_5_DAIO_VC5 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X40_5_DAIO_VC5;

	// TxPDO 0x1A14 - X40_7_DAIO_VC6 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X40_7_DAIO_VC6;

	// TxPDO 0x1A15 - X42_1_DAIO_AC1 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X42_1_DAIO_AC1;

	// TxPDO 0x1A16 - X42_3_DAIO_AC2 
	struct {
		float VC_Value;		//0x63C0 (value in V/mA)
		uint32_t VC_State;	//0x69C0 (Bit 0 Error U/I)
	} X42_3_DAIO_AC2;

	// TxPDO 0x1A17 - L400_Drehfeld 
	uint32_t L400_X1_Drehfeld;

	// TxPDO 0x1A05 - L400_DO_Byte1
	struct {
		uint32_t ProductCode;
		uint32_t SerialNumber;
		uint32_t HW_Version;
		uint32_t FW_Version;
		uint16_t AliasAddress;
	} slaveinfo;

	struct {
		uint32_t L400_SerialNumber;
		uint32_t L400_HW_Version;
		uint32_t L400_FW_Version;
		uint32_t L400_State;
	} L400Info;

} L400_TX_PDO_t;
#pragma pack(pop)

extern const DeviceDesc_t DEVICE_DESC_L400;

const char* l400_get_model_identifier(void);

#endif /* L400_DESC_H */
