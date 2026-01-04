/* PLC application bindings. */

#include <stddef.h>

#include "app/plc/plc_app.h"

int plc_app_bind(PlcApp_t* app, const TagTable_t* tags)
{
	size_t i;
	static const struct
	{
		const char* name;
		size_t offset;
	} kMappings[] = {
		{ "CPU_IO.X15_Out0", offsetof(IOView_t, X15_Out0) },
		{ "CPU_IO.X12_Out1", offsetof(IOView_t, X12_Out1) },
		{ "CPU_IO.X13_Out2", offsetof(IOView_t, X13_Out2) },
		{ "CPU_IO.X3_Out3", offsetof(IOView_t, X3_Out3) },
		{ "CPU_IO.X4_Out4", offsetof(IOView_t, X4_Out4) },
		{ "CPU_IO.X14_Out5", offsetof(IOView_t, X14_Out5) },
		{ "CPU_IO.X1_Out6", offsetof(IOView_t, X1_Out6) },
		{ "CPU_IO.X11_Out0", offsetof(IOView_t, X11_Out0) },
		{ "CPU_IO.X10_Out1", offsetof(IOView_t, X10_Out1) },
		{ "CPU_IO.X5_Out2", offsetof(IOView_t, X5_Out2) },
		{ "CPU_IO.X6_Out3", offsetof(IOView_t, X6_Out3) },
		{ "CPU_IO.X7_Out4", offsetof(IOView_t, X7_Out4) },
		{ "CPU_IO.X8_Out5", offsetof(IOView_t, X8_Out5) },
		{ "CPU_IO.X9_Out6", offsetof(IOView_t, X9_Out6) },
		{ "CPU_IO.X8a_Out7", offsetof(IOView_t, X8a_Out7) },
		{ "CPU_IO.X30_2_In0", offsetof(IOView_t, X30_2_In0) },
		{ "CPU_IO.X30_4_In1", offsetof(IOView_t, X30_4_In1) },
		{ "CPU_IO.X30_6_In2", offsetof(IOView_t, X30_6_In2) },
		{ "CPU_IO.X30_8_In3", offsetof(IOView_t, X30_8_In3) },
		{ "CPU_IO.X30_11_In4", offsetof(IOView_t, X30_11_In4) },
		{ "CPU_IO.X30_13_In5", offsetof(IOView_t, X30_13_In5) },
		{ "CPU_IO.X30_19_In6", offsetof(IOView_t, X30_19_In6) },
		{ "CPU_IO.X30_21_In7", offsetof(IOView_t, X30_21_In7) },
		{ "CPU_IO.X55_In0", offsetof(IOView_t, X55_In0) },
		{ "CPU_IO.X21_CPU_Pt1.Pt_Value", offsetof(IOView_t, X21_CPU_Pt1_Pt_Value) },
		{ "CPU_IO.X21_CPU_Pt1.Pt_State", offsetof(IOView_t, X21_CPU_Pt1_Pt_State) },
		{ "CPU_IO.X22_CPU_Pt2.Pt_Value", offsetof(IOView_t, X22_CPU_Pt2_Pt_Value) },
		{ "CPU_IO.X22_CPU_Pt2.Pt_State", offsetof(IOView_t, X22_CPU_Pt2_Pt_State) },
		{ "CPU_IO.X23_CPU_VC1.VC_Value", offsetof(IOView_t, X23_CPU_VC1_VC_Value) },
		{ "CPU_IO.X23_CPU_VC1.VC_State", offsetof(IOView_t, X23_CPU_VC1_VC_State) },
		{ "CPU_IO.X21_CPU_Pt1_Ctrl", offsetof(IOView_t, X21_CPU_Pt1_Ctrl) },
		{ "CPU_IO.X21_CPU_Pt2_Ctrl", offsetof(IOView_t, X21_CPU_Pt2_Ctrl) },
		{ "CPU_IO.X23_CPU_VC1_Ctrl", offsetof(IOView_t, X23_CPU_VC1_Ctrl) },
	};

	if (app == NULL || tags == NULL)
	{
		return -1;
	}

	for (i = 0; i < sizeof(kMappings) / sizeof(kMappings[0]); ++i)
	{
		TagId_t id = tag_table_find_id(tags, kMappings[i].name);
		TagId_t* field;

		if (id == 0)
		{
			return -1;
		}

		field = (TagId_t*)((char*)&app->io + kMappings[i].offset);
		*field = id;
	}

	return 0;
}
