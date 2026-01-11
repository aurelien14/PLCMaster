/* PLC application bindings. */

#include <stdio.h>

#include "app/io/io_view.h"
#include "app/plc/plc_app.h"
#include "core/tag/tag_table.h"

int plc_app_bind(PlcApp_t* app, const TagTable_t* tags)
{
	typedef struct
	{
		TagId_t* dst;
		const char* name;
	} IoBindDesc_t;

	const IoBindDesc_t kIoBinds[] = {
		{ &app->io.X15_Out0, "CPU_IO.X15_Out0" },
		{ &app->io.X12_Out1, "CPU_IO.X12_Out1" },
		{ &app->io.X13_Out2, "CPU_IO.X13_Out2" },
		{ &app->io.X3_Out3, "CPU_IO.X3_Out3" },
		{ &app->io.X4_Out4, "CPU_IO.X4_Out4" },
		{ &app->io.X14_Out5, "CPU_IO.X14_Out5" },
		{ &app->io.X1_Out6, "CPU_IO.X1_Out6" },
		{ &app->io.X11_Out0, "CPU_IO.X11_Out0" },
		{ &app->io.X10_Out1, "CPU_IO.X10_Out1" },
		{ &app->io.X5_Out2, "CPU_IO.X5_Out2" },
		{ &app->io.X6_Out3, "CPU_IO.X6_Out3" },
		{ &app->io.X7_Out4, "CPU_IO.X7_Out4" },
		{ &app->io.X8_Out5, "CPU_IO.X8_Out5" },
		{ &app->io.X9_Out6, "CPU_IO.X9_Out6" },
		{ &app->io.X8a_Out7, "CPU_IO.X8a_Out7" },
		{ &app->io.X30_2_In0, "CPU_IO.X30_2_In0" },
		{ &app->io.X30_4_In1, "CPU_IO.X30_4_In1" },
		{ &app->io.X30_6_In2, "CPU_IO.X30_6_In2" },
		{ &app->io.X30_8_In3, "CPU_IO.X30_8_In3" },
		{ &app->io.X30_11_In4, "CPU_IO.X30_11_In4" },
		{ &app->io.X30_13_In5, "CPU_IO.X30_13_In5" },
		{ &app->io.X30_19_In6, "CPU_IO.X30_19_In6" },
		{ &app->io.X30_21_In7, "CPU_IO.X30_21_In7" },
		{ &app->io.X55_In0, "CPU_IO.X55_In0" },
		{ &app->io.X21_CPU_Pt1_Pt_Value, "CPU_IO.X21_CPU_Pt1_Pt_Value" },
		{ &app->io.X21_CPU_Pt1_Pt_State, "CPU_IO.X21_CPU_Pt1_Pt_State" },
		{ &app->io.X22_CPU_Pt2_Pt_Value, "CPU_IO.X22_CPU_Pt2_Pt_Value" },
		{ &app->io.X22_CPU_Pt2_Pt_State, "CPU_IO.X22_CPU_Pt2_Pt_State" },
		{ &app->io.X23_CPU_VC1_VC_Value, "CPU_IO.X23_CPU_VC1_VC_Value" },
		{ &app->io.X23_CPU_VC1_VC_State, "CPU_IO.X23_CPU_VC1_VC_State" },
		{ &app->io.X21_CPU_Pt1_Ctrl, "CPU_IO.X21_CPU_Pt1_Ctrl" },
		{ &app->io.X21_CPU_Pt2_Ctrl, "CPU_IO.X21_CPU_Pt2_Ctrl" },
		{ &app->io.X23_CPU_VC1_Ctrl, "CPU_IO.X23_CPU_VC1_Ctrl" },
	};
	size_t index;

	if (app == NULL || tags == NULL)
	{
		return -1;
	}

	for (index = 0; index < (sizeof(kIoBinds) / sizeof(kIoBinds[0])); ++index)
	{
		TagId_t id = tag_table_find_id(tags, kIoBinds[index].name);
		if (id == 0)
		{
			printf("Missing IO tag: %s\n", kIoBinds[index].name);
			return -1;
		}

		*(kIoBinds[index].dst) = id;
	}

	return 0;
}
