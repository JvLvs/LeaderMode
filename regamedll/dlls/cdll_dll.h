/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

constexpr int MAX_WEAPON_SLOTS = 5;		// hud item selection slots
constexpr int MAX_ITEM_TYPES   = 6;		// hud item selection slots
constexpr int MAX_AMMO_SLOTS   = 32;	// not really slots
constexpr int MAX_ITEMS        = 4;		// hard coded item types

constexpr int DEFAULT_FOV      = 90;	// the default field of view

#define HIDEHUD_WEAPONS             BIT(0)
#define HIDEHUD_FLASHLIGHT          BIT(1)
#define HIDEHUD_ALL                 BIT(2)
#define HIDEHUD_HEALTH              BIT(3)
#define HIDEHUD_TIMER               BIT(4)
#define HIDEHUD_MONEY               BIT(5)
#define HIDEHUD_CROSSHAIR           BIT(6)
#define HIDEHUD_OBSERVER_CROSSHAIR  BIT(7)

#define STATUSICON_OSCLI_COLOUR				BIT(0)
#define STATUSICON_OSCLI_ALPHA				BIT(1)
#define STATUSICON_ICON_ALPHA_NOFADE		BIT(2)
#define STATUSICON_TEXT_KEYBIND				BIT(3)
#define STATUSICON_TEXT_LOC					BIT(4)
#define STATUSICON_TEXT_LITERAL				BIT(5)
#define STATUSICON_TEXT_HUE_INDEPENDENT		BIT(6)
#define STATUSICON_TEXT_ALPHA_INDEPENDENT	BIT(7)
#define STATUSICON_TEXT_ALPHA_NOFADE		BIT(8)
#define STATUSICON_MSGFL_DEL_THIS			BIT(9)

#define HUD_PRINTNOTIFY  1
#define HUD_PRINTCONSOLE 2
#define HUD_PRINTTALK    3
#define HUD_PRINTCENTER  4
#define HUD_PRINTRADIO   5

#define STATUS_NIGHTVISION_ON  1
#define STATUS_NIGHTVISION_OFF 0

#define ITEM_STATUS_NIGHTVISION	BIT(0)
#define ITEM_STATUS_FLASHLIGHT	BIT(1)
#define ITEM_STATUS_DETONATOR	BIT(2)

#define SCORE_STATUS_DEAD       BIT(0)
#define SCORE_STATUS_GODFATHER       BIT(1)
#define SCORE_STATUS_COMMANDER        BIT(2)
#define SCORE_STATUS_DEFKIT     BIT(3)

// player data iuser3
#define PLAYER_CAN_SHOOT        BIT(0)
#define PLAYER_FREEZE_TIME_OVER BIT(1)
#define PLAYER_IN_BOMB_ZONE     BIT(2)
#define PLAYER_HOLDING_SHIELD   BIT(3)
#define PLAYER_PREVENT_DUCK     BIT(4)
#define PLAYER_PREVENT_CLIMB    BIT(5) // The player can't climb ladder
#define PLAYER_PREVENT_JUMP     BIT(6)

#define MENU_KEY_1 BIT(0)
#define MENU_KEY_2 BIT(1)
#define MENU_KEY_3 BIT(2)
#define MENU_KEY_4 BIT(3)
#define MENU_KEY_5 BIT(4)
#define MENU_KEY_6 BIT(5)
#define MENU_KEY_7 BIT(6)
#define MENU_KEY_8 BIT(7)
#define MENU_KEY_9 BIT(8)
#define MENU_KEY_0 BIT(9)

#define WEAPON_SUIT       31
#define WEAPON_ALLWEAPONS (~(1<<WEAPON_SUIT))

#define HEADNAME_HIDE		0
#define HEADNAME_TEAMMATE	1
#define HEADNAME_ZOMBIE		2
#define HEADNAME_ALL		3

// custom enum
enum class VGUIMenu : BYTE
{
	OFF,
	TEAM,
	TEAM_NO_SPEC,
	TEAM_IG,
	TEAM_NO_SPEC_IG,
	ROLE,
};

// custom enum
enum VGUIMenuSlot
{
	VGUI_MenuSlot_Buy_Pistol = 1,
	VGUI_MenuSlot_Buy_ShotGun,
	VGUI_MenuSlot_Buy_SubMachineGun,
	VGUI_MenuSlot_Buy_Rifle,
	VGUI_MenuSlot_Buy_MachineGun,
	VGUI_MenuSlot_Buy_PrimAmmo,
	VGUI_MenuSlot_Buy_SecAmmo,
	VGUI_MenuSlot_Buy_Item,
};
