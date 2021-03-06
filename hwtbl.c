/*
 * This file defines the hardware hooks table
 */
#include "dcpuhw.h"

/* Hardware functions */
void DCPU_Register();
void Memory_Register();
void DCPUBUS_Register();
void Keyboard_Register();
void Clock_Register();
void Nya_LEM_Register();
void Disk_M35FD_Register();
void EEROM_Register();

void isi_register_objects()
{
	DCPU_Register();
	Memory_Register();
	DCPUBUS_Register();
	Keyboard_Register();
	Clock_Register();
	Nya_LEM_Register();
	Disk_M35FD_Register();
	EEROM_Register();
}

