/* Pré-résolu au build, jamais modifié ensuite. */

#ifndef IO_VIEW_H
#define IO_VIEW_H

#include "core/tag/tag_table.h"

typedef struct IOView
{
	TagId_t X15_Out0;
	TagId_t X12_Out1;
	TagId_t X13_Out2;
	TagId_t X3_Out3;
	TagId_t X4_Out4;
	TagId_t X14_Out5;
	TagId_t X1_Out6;
	TagId_t X11_Out0;
	TagId_t X10_Out1;
	TagId_t X5_Out2;
	TagId_t X6_Out3;
	TagId_t X7_Out4;
	TagId_t X8_Out5;
	TagId_t X9_Out6;
	TagId_t X8a_Out7;
	TagId_t X30_2_In0;
	TagId_t X30_4_In1;
	TagId_t X30_6_In2;
	TagId_t X30_8_In3;
	TagId_t X30_11_In4;
	TagId_t X30_13_In5;
	TagId_t X30_19_In6;
	TagId_t X30_21_In7;
	TagId_t X55_In0;
	TagId_t X21_CPU_Pt1_Pt_Value;
	TagId_t X21_CPU_Pt1_Pt_State;
	TagId_t X22_CPU_Pt2_Pt_Value;
	TagId_t X22_CPU_Pt2_Pt_State;
	TagId_t X23_CPU_VC1_VC_Value;
	TagId_t X23_CPU_VC1_VC_State;
	TagId_t X21_CPU_Pt1_Ctrl;
	TagId_t X21_CPU_Pt2_Ctrl;
	TagId_t X23_CPU_VC1_Ctrl;
} IOView_t;

#endif /* IO_VIEW_H */
