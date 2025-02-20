/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "precompiled.h"


char g_szLanguage[64] = "\0";
cl_enginefunc_t gEngfuncs;
IBaseFileSystem* g_pFileSystem = nullptr;

IGameUIFuncs *gameuifuncs = NULL;
vgui::IEngineVGui *enginevguifuncs = NULL;
vgui::ISurface *enginesurfacefuncs = NULL;
IServerBrowser* serverbrowser = NULL;
IGameUI* g_pGameUI = nullptr;
IBaseUI* baseuifuncs = NULL;

// Metahook API
IKeyValuesSystem* (*KeyValuesSystem)(void) = nullptr;
ICommandLine* (*CommandLine)(void) = nullptr;
cl_enginefunc_t* (*GetEngineFuncs)() = nullptr;
unsigned int (*LoadDDS)(const char* szFile, int* iWidth, int* iHeight) = nullptr;

static CBasePanel* staticPanel = NULL;
vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;

inline void LoadMetahookAPI(void)
{
	HMODULE hMetahookDLL = GetModuleHandle("lm_metahook_module.dll");

	if (!hMetahookDLL)
	{
		Sys_Error("lm_metahook_module.dll no found!");
		return;
	}

	if (!(*(void**)&KeyValuesSystem = GetProcAddress(hMetahookDLL, "GetKeyValueSystem")))
	{
		Sys_Error("lm_metahook_module.dll export function \"GetKeyValueSystem\" no found!");
		return;
	}

	if (!(*(void**)&CommandLine = GetProcAddress(hMetahookDLL, "CommandLine")))
	{
		Sys_Error("lm_metahook_module.dll export function \"CommandLine\" no found!");
		return;
	}

	if ((*(void**)&GetEngineFuncs = GetProcAddress(hMetahookDLL, "GetEngineFuncs")) != nullptr)
	{
		memcpy(&gEngfuncs, GetEngineFuncs(), sizeof(cl_enginefunc_t));
	}

	if (!(*(void**)&LoadDDS = GetProcAddress(hMetahookDLL, "LoadDDS")))
	{
		Sys_Error("lm_metahook_module.dll export function \"LoadDDS\" no found!");
		return;
	}
}

IGameUI::IGameUI()
{
	g_pGameUI = this;
	m_szPreviousStatusText[0] = 0;
	m_bLoadlingLevel = false;
}

IGameUI::~IGameUI()
{
	g_pGameUI = nullptr;
}

void IGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	// Get the system language.
	Sys_GetRegKeyValueUnderRoot("Software\\Valve\\Steam", "Language", g_szLanguage, charsmax(g_szLanguage), "english");

	char szDllName[512];

	// load and initialize vgui
	CreateInterfaceFn thisFactory = Sys_GetFactoryThis();
	CreateInterfaceFn engineFactory = factories[ 0 ];
	CreateInterfaceFn vguiFactory = factories[ 1 ];
	CreateInterfaceFn fileSystemFactory = factories[ 2 ];

	g_pFileSystem = (IFileSystem *)fileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL);

	// load the VGUI library
	g_pFileSystem->GetLocalPath("cl_dlls\\vgui_dll.dll", szDllName, sizeof(szDllName));
	CreateInterfaceFn vguiDllFactory = Sys_GetFactory(Sys_LoadModule(szDllName));

	// Access MH module.
	LoadMetahookAPI();

	// setup the factory list
	CreateInterfaceFn factoryList[5] =
	{
		thisFactory,
		engineFactory,
		vguiFactory,
		fileSystemFactory,
		vguiDllFactory,
	};

	// setup VGUI library
	IVGuiDLL *staticVGuiDllFuncs = (IVGuiDLL *)vguiDllFactory(VGUI_IVGUIDLL_INTERFACE_VERSION, NULL);
	staticVGuiDllFuncs->Init(factories, count);

	ConnectTier3Libraries(factoryList, _countof(factoryList));

	// setup vgui controls
	if (!vgui::VGui_InitInterfacesList("GameUI", factories, count))
	{
		Sys_Error("Failed to setup VGUI controls!");
		return;
	}

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/vgui_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/gameui_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/leadermode_%language%.txt");

	enginevguifuncs = (vgui::IEngineVGui *)engineFactory(VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = (vgui::ISurface *)vguiDllFactory(VGUI_SURFACE_INTERFACE_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs *)engineFactory(VENGINE_GAMEUIFUNCS_VERSION, NULL);
	baseuifuncs = (IBaseUI*)engineFactory(BASEUI_INTERFACE_VERSION, NULL);

	if (!enginesurfacefuncs || !gameuifuncs || !enginevguifuncs || !baseuifuncs)
	{
		Sys_Error("IGameUI::Initialize() failed to get necessary interfaces\n");
	}

	vgui::scheme()->LoadSchemeFromFile("Resource/SourceScheme.res", "SourceScheme");
	serverbrowser = (IServerBrowser*)CreateInterface(SERVERBROWSER_INTERFACE_VERSION, NULL);

	if (serverbrowser)
		serverbrowser->Initialize(factories, count);

	int swide, stall;
	g_pVGuiSurface->GetScreenSize(swide, stall);

	// setup base panel
	staticPanel = new CBasePanel();
	staticPanel->SetBounds(0, 0, swide, stall);
	staticPanel->SetPaintBorderEnabled(false);
	staticPanel->SetPaintBackgroundEnabled(true);
	staticPanel->SetPaintEnabled(false);
	staticPanel->SetVisible(true);
	staticPanel->SetMouseInputEnabled(false);
	staticPanel->SetKeyBoardInputEnabled(false);

	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel(vgui::PANEL_GAMEUIDLL);
	staticPanel->SetParent(rootpanel);

	if (serverbrowser)
		serverbrowser->SetParent(staticPanel->GetVPanel());

	vgui::surface()->SetAllowHTMLJavaScript(true);
}

void IGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, IBaseSystem* system)
{
	memcpy(&gEngfuncs, engineFuncs, sizeof(gEngfuncs));

	gEngfuncs.pfnClientCmd("mp3 loop music/Mountains_Of_Home.mp3\n");

	ModInfo().LoadCurrentGameInfo();

	if (serverbrowser)
	{
		serverbrowser->ActiveGameName("Leader Mode", gEngfuncs.pfnGetGameDirectory());	// If I change the game dir, this has to be change as well. FIXME
		serverbrowser->Reactivate();
	}
}

void IGameUI::Shutdown(void)
{
	if (serverbrowser)
	{
		serverbrowser->Deactivate();
		serverbrowser->Shutdown();
	}
}

int IGameUI::ActivateGameUI(void)
{
	if (!m_bLoadlingLevel && g_hLoadingDialog.Get() && IsInLevel())
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}

	if (!m_bLoadlingLevel)
	{
		if (IsGameUIActive())
			return 1;
	}

	staticPanel->OnGameUIActivated();
	staticPanel->SetVisible(true);

	gEngfuncs.pfnClientCmd("setpause");
	return 1;
}

int IGameUI::ActivateDemoUI(void)
{
	staticPanel->OnOpenDemoDialog();
	return 1;
}

int IGameUI::HasExclusiveInput(void)
{
	return IsGameUIActive();
}

void IGameUI::RunFrame(void)
{
	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);
	staticPanel->SetSize(wide, tall);

	BasePanel()->RunFrame();
}

void IGameUI::ConnectToServer(const char *game, int IP, int port)
{
	if (serverbrowser)
		serverbrowser->ConnectToGame(IP, port);

	gEngfuncs.pfnClientCmd("mp3 stop\n");
}

void IGameUI::DisconnectFromServer(void)
{
	if (serverbrowser)
		serverbrowser->DisconnectFromGame();
}

void IGameUI::HideGameUI(void)
{
	if (!IsGameUIActive())
		return;

	if (!IsInLevel())
		return;

	staticPanel->SetVisible(false);

	gEngfuncs.pfnClientCmd("unpause");
	gEngfuncs.pfnClientCmd("hideconsole");

	if (!m_bLoadlingLevel && g_hLoadingDialog.Get())
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}
}

bool IGameUI::IsGameUIActive(void)
{
	return staticPanel->IsVisible();
}

void IGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	m_bLoadlingLevel = true;

	staticPanel->OnLevelLoadingStarted(resourceName);
}

void IGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	m_bLoadlingLevel = false;

	staticPanel->OnLevelLoadingFinished();
	baseuifuncs->HideGameUI();
}

void IGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	if (!g_hLoadingDialog.Get())
		g_hLoadingDialog = new CLoadingDialog(staticPanel);

	m_szPreviousStatusText[0] = 0;
	g_hLoadingDialog->SetProgressRange(0, progressSteps);
	g_hLoadingDialog->SetProgressPoint(0.0f);
	g_hLoadingDialog->Open();
}

int IGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	if (!g_hLoadingDialog.Get())
	{
		g_hLoadingDialog = new CLoadingDialog(staticPanel);
		g_hLoadingDialog->SetProgressRange(0, 24);
		g_hLoadingDialog->SetProgressPoint(0.0f);
		g_hLoadingDialog->Open();
	}

	g_hLoadingDialog->Activate();
	g_hLoadingDialog->SetProgressPoint(progressPoint);
	return 1;
}

void IGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if (!g_hLoadingDialog.Get() && bError)
		g_hLoadingDialog = new CLoadingDialog(staticPanel);

	if (!g_hLoadingDialog.Get())
		return;

	if (bError)
	{
		g_hLoadingDialog->DisplayGenericError(failureReason, extendedReason);
	}
	else
	{
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}
}

int IGameUI::SetProgressBarStatusText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return false;

	if (!statusText)
		return false;

	if (!Q_stricmp(statusText, m_szPreviousStatusText))
		return false;

	g_hLoadingDialog->SetStatusText(statusText);
	Q_strncpy(m_szPreviousStatusText, statusText, sizeof(m_szPreviousStatusText));
	return true;
}

void IGameUI::SetSecondaryProgressBar(float progress)
{
	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgress(progress);
}

void IGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgressText(statusText);
}

void IGameUI::ValidateCDKey(bool force, bool inConnect)
{
}

void IGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username)
{
}

bool IGameUI::IsInLevel(void)
{
	const char* levelName = gEngfuncs.pfnGetLevelName();

	return (levelName && strlen(levelName) > 0);
}

bool IGameUI::IsInMultiplayer(void)
{
	return (IsInLevel() && gEngfuncs.GetMaxClients() > 1);
}

EXPOSE_SINGLE_INTERFACE(IGameUI, IGameUI, GAMEUI_INTERFACE_VERSION);
