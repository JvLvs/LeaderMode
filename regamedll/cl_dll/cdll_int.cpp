/*

Created Date: 05 Mar 2020

*/

#include "precompiled.h"

cl_enginefunc_t gEngfuncs;
cl_extendedfunc_t gExtFuncs;
bool g_bInGameWorld = false;
char g_szLanguage[64] = "\0";

void CL_DLLEXPORT CAM_Think(void)
{
	CAM_Think2();
}

void CL_DLLEXPORT CL_CameraOffset(float* ofs)
{
	CL_CameraOffset2(ofs);
}

void CL_DLLEXPORT CL_CreateMove(float frametime, usercmd_s* cmd, int active)
{
	CL_CreateMove2(frametime, cmd, active);
}

BOOL CL_DLLEXPORT CL_IsThirdPerson(void)
{
	return (cam_thirdperson ? 1 : 0) || (g_iUser1 && (g_iUser2 == gEngfuncs.GetLocalPlayer()->index));
}

void* CL_DLLEXPORT ClientFactory(void)
{
	return Sys_GetFactoryThis();
}

void CL_DLLEXPORT Demo_ReadBuffer(int size, unsigned char* buffer)
{
}

BOOL CL_DLLEXPORT HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname)
{
	return HUD_AddEntity2(type, ent, modelname);
}

void CL_DLLEXPORT HUD_ChatInputPosition(int* x, int* y)
{
}

BOOL CL_DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	return FALSE;
}

void CL_DLLEXPORT HUD_CreateEntities(void)
{
	HUD_CreateEntities2();
}

void CL_DLLEXPORT HUD_DirectorMessage(int iSize, void* pbuf)
{
	CHudSpectator::DirectorMessage(iSize, pbuf);
}

void CL_DLLEXPORT HUD_DrawNormalTriangles(void)
{
	HUD_DrawNormalTriangles2();
}

void CL_DLLEXPORT HUD_DrawTransparentTriangles(void)
{
	HUD_DrawTransparentTriangles2();
}

void CL_DLLEXPORT HUD_Frame(double flDeltaTime)
{
	g_flClientTimeDelta = flDeltaTime;
	g_flClientTime += flDeltaTime;

	HUD_WeaponsPostThink(&g_sWpnFrom, &g_sWpnTo, &g_sWpnCmd, g_flClientTime, gPseudoPlayer.random_seed);
	Sound_Think(flDeltaTime);
	RegionalFogThink();
}

BOOL CL_DLLEXPORT HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	BOOL FReturn = FALSE;

	switch (hullnumber)
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		FReturn = TRUE;
		break;

	case 1:				// Crouched player
		mins = Vector(-16, -16, -18);
		maxs = Vector(16, 16, 18);
		FReturn = TRUE;
		break;

	case 2:				// Point based hull
		mins = Vector(0, 0, 0);
		maxs = Vector(0, 0, 0);
		FReturn = TRUE;
		break;
	}

	return FReturn;
}

int CL_DLLEXPORT HUD_GetPlayerTeam(int iParam)	// useless & unknown.
{
	return 0;
}

BOOL CL_DLLEXPORT HUD_GetStudioModelInterface(int version, r_studio_interface_s** ppinterface, engine_studio_api_s* pstudio)
{
	return HUD_GetStudioModelInterface2(version, ppinterface, pstudio);
}

cl_entity_t CL_DLLEXPORT* HUD_GetUserEntity(int index)
{
	return HUD_GetUserEntity2(index);
}

void CL_DLLEXPORT HUD_Init(void)
{
	InitInput();

	V_Init();	// Initialize view system
	CAM_Init();
	IN_Init();
	KB_Init();
	Msg_Init();
	gHUD::Init();
	InitRain();
	Wpn_Init();
}

BOOL CL_DLLEXPORT HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	if (keynum == 96)	// Console key. Make sure that engine handles it. LUNA: Why it does not listed in keydef.h???
		return TRUE;

	return HUD_Key_Event2(down, keynum, pszCurrentBinding);
}

void CL_DLLEXPORT HUD_PlayerMove(playermove_s* ppmove, int server)
{
	PM_Move(ppmove, server);
}

void CL_DLLEXPORT HUD_PlayerMoveInit(playermove_s* ppmove)
{
	PM_Init(ppmove);
}

char CL_DLLEXPORT HUD_PlayerMoveTexture(char* name)
{
	return PM_FindTextureType(name);
}

void CL_DLLEXPORT HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	HUD_PostRunCmd2(from, to, cmd, runfuncs, time, random_seed);
}

void CL_DLLEXPORT HUD_ProcessPlayerState(entity_state_s* dst, const entity_state_s* src)
{
	HUD_ProcessPlayerState2(dst, src);
}

BOOL CL_DLLEXPORT HUD_Redraw(float time, int intermission)
{
	gHUD::Redraw(time, intermission);
	return TRUE;
}

void CL_DLLEXPORT HUD_Reset(void)
{
}

void CL_DLLEXPORT HUD_Shutdown(void)
{
	gHUD::Shutdown();
	gFontFuncs::Shutdown();
	Sound_Exit();
	CL_UnloadParticleMan();
}

void CL_DLLEXPORT HUD_StudioEvent(const mstudioevent_s* event, const cl_entity_s* entity)
{
	HUD_StudioEvent2(event, entity);
}

void CL_DLLEXPORT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, tempent_s** ppTempEntFree, tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
	HUD_TempEntUpdate2(frametime, client_time, cl_gravity, ppTempEntFree, ppTempEntActive, Callback_AddVisibleEntity, Callback_TempEntPlaySound);
}

void CL_DLLEXPORT HUD_TxferLocalOverrides(entity_state_s* state, const clientdata_s* client)
{
	HUD_TxferLocalOverrides2(state, client);
}

void CL_DLLEXPORT HUD_TxferPredictionData(entity_state_s* ps, const entity_state_s* pps, clientdata_s* pcd, const clientdata_s* ppcd, weapon_data_s* wd, const weapon_data_s* pwd)
{
	HUD_TxferPredictionData2(ps, pps, pcd, ppcd, wd, pwd);
}

BOOL CL_DLLEXPORT HUD_UpdateClientData(client_data_t* pcldata, float flTime)
{
	return gHUD::UpdateClientData(pcldata, flTime);
}

BOOL CL_DLLEXPORT HUD_VidInit(void)
{
	g_pViewEnt = gEngfuncs.GetViewModel();
	g_bInGameWorld = true;

	gHUD::VidInit();
	gFontFuncs::Init();
	gViewModelHandsTexture::Initiation();
	return TRUE;
}

void CL_DLLEXPORT HUD_VoiceStatus(int entindex, BOOL bTalking)
{
}

void CL_DLLEXPORT IN_Accumulate(void)
{
	IN_Accumulate2();
}

void CL_DLLEXPORT IN_ActivateMouse(void)
{
	IN_ActivateMouse2();
}

void CL_DLLEXPORT IN_ClearStates(void)
{
	IN_ClearStates2();
}

void CL_DLLEXPORT IN_DeactivateMouse(void)
{
	IN_DeactivateMouse2();
}

void CL_DLLEXPORT IN_MouseEvent(int mstate)
{
	IN_MouseEvent2(mstate);
}

BOOL CL_DLLEXPORT Initialize_(cl_enginefunc_t* pEnginefuncs, int iVersion)	// LUNA: this has to be renamed because so many other functions are using the same name.
{
	if (iVersion != CLDLL_INTERFACE_VERSION)
		return FALSE;

	Q_memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	Dxt_Initialization();
	Events_Init();
	Sound_Init();

	CL_LoadParticleMan();

	// Get the system language.
	Sys_GetRegKeyValueUnderRoot("Software\\Valve\\Steam", "Language", g_szLanguage, charsmax(g_szLanguage), "english");

	// get tracker interface, if any
	return TRUE;
}

kbutton_s CL_DLLEXPORT* KB_Find(const char* name)
{
	return KB_Find2(name);
}

void CL_DLLEXPORT V_CalcRefdef(ref_params_s* pparams)
{
	// intermission / finale rendering
	if (pparams->intermission)
	{
		V_CalcIntermissionRefdef(pparams);
	}
	else if (pparams->spectator || g_iUser1)	// g_iUser true if in spectator mode
	{
		V_CalcSpectatorRefdef(pparams);
	}
	else if (CL_IsThirdPerson())
	{
		V_CalcThirdPersonRefdef(pparams);
	}
	else
	{
		V_CalcNormalRefdef(pparams);
	}

	gHUD::CalcRefdef(pparams);

	// copy the result and save them for our own use.
	Q_memcpy(&g_pparams, pparams, sizeof(ref_params_s));
}

//
// From here are the extended functions called from metahook module.
//

// Command 'changelevel' and things like that won't trigger this.
// You may still use HUD_VidInit to make a complement.
void CL_DLLEXPORT CL_Disconnect(void)
{
	gExtFuncs.pfnCL_Disconnect();

	g_bInGameWorld = false;
	g_pCurWeapon = nullptr;
}

bool CL_DLLEXPORT MH_LoadClient(unsigned short iVersion, const cl_extendedfunc_t* pfn)
{
	if (iVersion != CLIENT_EXTENDED_FUNCS_API_VERSION)
	{
		Sys_Error("Version mismatch between client and metahook module!\nExpect version: %d\nReceived version: %d", CLIENT_EXTENDED_FUNCS_API_VERSION, iVersion);
		return false;
	}

	if (pfn)
		Q_memcpy(&gExtFuncs, pfn, sizeof(gExtFuncs));
	else
	{
		Sys_Error("Null function table \"cl_extendedfunc_t\" sent!");
		return false;
	}

	return true;
}

void CL_DLLEXPORT S_StartSound(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	if (iChannel == CHAN_STATIC)
		gExtFuncs.pfnS_StartStaticSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
	else
		gExtFuncs.pfnS_StartDynamicSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
}

void CL_DLLEXPORT S_StopAllSounds(bool STFU)
{
	gExtFuncs.pfnS_StopAllSounds(STFU);
}
