/* FCE Ultra - NES/Famicom Emulator
*
* Copyright notice for this file:
*  Copyright (C) 2023 Marcos Del Sol Vives <marcos@orca.pet>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mapinc.h"

static uint8 prg_reg, mir_reg;
static uint8_t *WRAM;

static void Sync(void) {
	setprg16(0x8000, prg_reg);
	setprg16(0xC000, ~0);
	setchr8(0);
	switch (mir_reg) {
	case 0:
		setmirror(MI_V);
		break;
	case 1:
		setmirror(MI_H);
		break;
	}
}

static DECLFW(MirRegWrite) {
	// 2 bit register for mirroring control, fed LSB first
	mir_reg = V & 1;
	Sync();
}

static DECLFW(PrgRegWrite) {
	// 5 bit shift register, fed LSB first
	prg_reg = ((V & 1) << 4) | (prg_reg >> 1);
	Sync();
}

static void Power(void) {
	Sync();
	SetReadHandler(0x6000, 0xFFFF, CartBR);
	SetWriteHandler(0x6000, 0x7FFF, CartBW);
	SetWriteHandler(0xC000, 0xDFFF, MirRegWrite);
	SetWriteHandler(0xE000, 0xFFFF, PrgRegWrite);
	FCEU_CheatAddRAM(8, 0x6000, WRAM);
}

static void StateRestore(int version) {
	Sync();
}

static void Close() {
	FCEU_free(WRAM);
	WRAM = NULL;
}

void Mapper248_Init(CartInfo *info) {
	info->Power = Power;
	info->Close = Close;
	GameStateRestore = StateRestore;

	AddExState(&prg_reg, sizeof(prg_reg), 0, "PREG");
	AddExState(&mir_reg, sizeof(mir_reg), 0, "MREG");

	WRAM = (uint8*)FCEU_gmalloc(8192);
	SetupCartPRGMapping(0x10, WRAM, 8192, 1);
	AddExState(WRAM, 8192, 0, "WRAM");
}
