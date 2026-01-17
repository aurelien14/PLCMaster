/* L230 device descriptor declarations. */

#ifndef L230_DESC_H
#define L230_DESC_H

#include "core/device/device_desc.h"

#include <stdint.h>

static_assert(sizeof(float) == 4, "float must be 32-bit");

#pragma pack(push, 1)
typedef struct
{
	// RxPDO 0x1600 - L230_DO_Byte0
	union {
		uint8_t L230_DO_Byte0;
		struct {
			uint8_t X15_Out0 : 1;
			uint8_t X12_Out1 : 1;
			uint8_t X13_Out2 : 1;
			uint8_t X3_Out3 : 1;
			uint8_t X4_Out4 : 1;
			uint8_t X14_Out5 : 1;
			uint8_t X1_Out6 : 1;
			uint8_t padding : 1;
		} L230_DO_Byte0_bits;
	};

	// RxPDO 0x1601 - L230_DO_Byte1
	union {
		uint8_t L230_DO_Byte1;
		struct {
			uint8_t X11_Out0 : 1;
			uint8_t X10_Out1 : 1;
			uint8_t X5_Out2 : 1;
			uint8_t X6_Out3 : 1;
			uint8_t X7_Out4 : 1;
			uint8_t X8_Out5 : 1;
			uint8_t X9_Out6 : 1;
			uint8_t X8a_Out7 : 1;
		} L230_DO_Byte1_bits;
	};

	// RxPDO 0x1602 - X21_CPU_Pt1
	uint32_t X21_CPU_Pt1_Ctrl;
	// RxPDO 0x1603 - X21_CPU_Pt2
	uint32_t X22_CPU_Pt2_Ctrl;
	// RxPDO 0x1604 - X21_CPU_VC1
	uint32_t X23_CPU_VC1_Ctrl; //<Comment>16#80000000 = Channel Off, 16#80010000 = 0-10V, 16#80030000 = 0-20mA
} L230_RX_PDO_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	// TxPDO 0x1A00 - L230_DI_Byte0
	struct {
		uint8_t X30_2_In0 : 1;	//0x6080
		uint8_t X30_4_In1 : 1;	//0x6090
		uint8_t X30_6_In2 : 1;	//0x60A0
		uint8_t X30_8_In3 : 1;	//0x60B0
		uint8_t X30_11_In4 : 1;	//0x60C0
		uint8_t X30_13_In5 : 1;	//0x60D0
		uint8_t X30_19_In6 : 1;	//0x60E0
		uint8_t X30_21_In7 : 1;	//0x60F0
	} L230_DI_Byte0;

	// TxPDO 0x1A01 - L230_DO_Byte1
	struct {
		uint8_t X55_In0 : 1;
		uint8_t padding : 7;
	} L230_DI_Byte1;

	// TxPDO 0x1A02 - X21_CPU_Pt1 
	struct {
		float Pt_Value;		//0x63A0 (value in ohm)
		uint32_t Pt_State;	//0x63B0 (Bit 0 Error PT100)
	} X21_CPU_Pt1;


	// TxPDO 0x1A03 - X22_CPU_Pt2
	struct {
		float Pt_Value;		//0x63C0 (value in ohm)
		uint32_t Pt_State;	//0x63D0 (Bit 0 Error PT100)
	} X22_CPU_Pt2;

	// TxPDO 0x1A04 - X23_CPU_VC1 
	struct {
		float VC_Value;		//0x63E0 (value in V/mA)
		uint32_t VC_State;	//0x63F0 (Bit 0 Error U/I)
	} X23_CPU_VC1;


	// TxPDO 0x1A05
	struct {
		uint32_t ProductCode;	//0x6400
		uint32_t SerialNumber;	//0x6401
		uint32_t HW_Version;	//0x6402
		uint32_t FW_Version;	//0x6403
		uint16_t AliasAddress;	//0x6404 Hex switch
	} slaveinfo;

	struct {
		uint32_t L230_SerialNumber;
		uint32_t L230_HW_Version;
		uint32_t L230_FW_Version;
		uint32_t L230_State;
	} L230Info;

} L230_TX_PDO_t;
#pragma pack(pop)

extern const DeviceDesc_t DEVICE_DESC_L230;

const char *l230_get_model_identifier(void);

#endif /* L230_DESC_H */
