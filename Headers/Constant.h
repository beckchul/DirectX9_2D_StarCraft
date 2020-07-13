#ifndef __CONSTANT_H__
#define __CONSTANT_H__

const WORD MIN_STR = 64;
const WORD MID_STR = 128;
const WORD MAX_STR = 256;

const DWORD KEY_A		= 0x00000001;
const DWORD KEY_S		= 0x00000002;
const DWORD KEY_D		= 0x00000004;
const DWORD KEY_F		= 0x00000008;
const DWORD KEY_G		= 0x00000010;
const DWORD KEY_H		= 0x00000020;
const DWORD KEY_J		= 0x00000040;
const DWORD KEY_K		= 0x00000080;
const DWORD KEY_L		= 0x00000100;
const DWORD KEY_Z		= 0x00000200;
const DWORD KEY_X		= 0x00000400;
const DWORD KEY_C		= 0x00000800;
const DWORD KEY_V		= 0x00001000;
const DWORD KEY_B		= 0x00002000;
const DWORD KEY_N		= 0x00004000;
const DWORD KEY_M		= 0x00008000;
const DWORD KEY_Q		= 0x00010000;
const DWORD KEY_W		= 0x00020000;
const DWORD KEY_E		= 0x00040000;
const DWORD KEY_R		= 0x00080000;
const DWORD KEY_T		= 0x00100000;
const DWORD KEY_Y		= 0x00200000;
const DWORD KEY_U		= 0x00400000;
const DWORD KEY_I		= 0x00800000;
const DWORD KEY_O		= 0x01000000;
const DWORD KEY_P		= 0x02000000;
const DWORD KEY_LBUTTON	= 0x04000000;
const DWORD KEY_RBUTTON	= 0x08000000;

const DWORD KEY_LEFT	= 0x10000000;
const DWORD KEY_RIGHT	= 0x20000000;
const DWORD KEY_UP		= 0x40000000;
const DWORD KEY_DOWN	= 0x80000000;



const DWORD MOUSE_U		= 0x00000001;
const DWORD MOUSE_RU	= 0x00000002;
const DWORD MOUSE_LU	= 0x00000004;
const DWORD MOUSE_R		= 0x00000008;
const DWORD MOUSE_L		= 0x00000010;
const DWORD MOUSE_D		= 0x00000020;
const DWORD MOUSE_RD	= 0x00000040;
const DWORD MOUSE_LD	= 0x00000080;
const DWORD MOUSE_NOMAL = 0x00000100;
const DWORD MOUSE_CLICK = 0x00000200;
const DWORD MOUSE_DRAG  = 0x00000400;
const DWORD MOUSE_ENEMY = 0x00000800;
const DWORD MOUSE_OBJECT= 0x00001000;
const DWORD MOUSE_PLAYER= 0x00002000;
const DWORD MOUSE_TARGET_ALLY	= 0x00004000;
const DWORD MOUSE_TARGET_ENEMY  = 0x00008000;
const DWORD MOUSE_TARGET_NEUTRAL= 0x00010000;
const DWORD MOUSE_TARGET_RALLY  = 0x00020000;
const DWORD MOUSE_ATTACK_POINT_NOMAL = 0x00040000;
const DWORD MOUSE_ATTACK_POINT_PLAYER = 0x00080000;
const DWORD MOUSE_ATTACK_POINT_ENEMY = 0x00100000;
const DWORD MOUSE_ATTACK_POINT_PATOL = 0x00200000;
const DWORD MOUSE_SKILL_POINT		 = 0x00400000;
const DWORD MOUSE_SKILL_POINT2 = 0x00800000;
const DWORD MOUSE_SKILL_POINT3 = 0x01000000;
const DWORD MOUSE_CLICK_ALL = 0x02000000;

const DWORD OBJECT_RENDER_TRUE = 0x00000001;
const DWORD OBJECT_RENDER_FALSE = 0x00000002;

const DWORD UNIT_ATTACK			= 0x00000001;
const DWORD UNIT_DIGUP_RESOURCE_M = 0x00000002;
const DWORD UNIT_DIGUP_RESOURCE_G = 0x00000004;
const DWORD UNIT_GIVE_NEXUS		= 0x00000008;
const DWORD UNIT_NORMAL			= 0x00000010;
const DWORD UNIT_DEAD			= 0x00000020;
const DWORD UNIT_GOINGBUILD		= 0x00000040;
const DWORD UNIT_STAND			= 0x00000080;
const DWORD UNIT_MOVE			= 0x00000100;
const DWORD UNIT_HOLD			= 0x00000200;
const DWORD UNIT_PATROL			= 0x00000400;
const DWORD UNIT_CREATE			= 0x00000800;
const DWORD UNIT_SKILL1			= 0x00001000;
const DWORD UNIT_SKILL2			= 0x00002000;
const DWORD UNIT_SKILL3			= 0x00004000;
const DWORD UNIT_MAELSTORM		= 0x00008000;
const DWORD UNIT_STASISFIELD	= 0x00010000;

#endif