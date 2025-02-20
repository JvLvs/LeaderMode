/*

Created Date: 07 Mar 2020

*/

#pragma once

#include <list>

#define RGB_YELLOWISH		0x00FFA000
#define VEC_YELLOWISH		Vector(1.0f, 0.627451f, 0.0f)
#define RGB_REDISH			0x00FF1010
#define VEC_REDISH			Vector(1.0f, 0.0627451f, 0.0627451f)
#define RGB_GREENISH		0x0000A000
#define VEC_GREENISH		Vector(0.0f, 0.627451f, 0.0f)
#define RGB_CYANISH			0x0033CCFF
#define VEC_CYANISH			Vector(0.2f, 0.8f, 1.0f)
#define RGB_SPRINGGREENISH	0x0066FF99
#define VEC_SPRINGGREENISH	Vector(0.4f, 1.0f, 0.6f)
#define RGB_CT_COLOUR		0x00ADC9EB
#define VEC_CT_COLOUR		Vector(173.0 / 255.0, 201.0 / 255.0, 235.0 / 255.0)
#define RGB_T_COLOUR		0x00D85150
#define VEC_T_COLOUR		Vector(216.0 / 255.0, 81.0 / 255.0, 80.0 / 255.0)
#define RGB_SILVERISH		0x00C0C0C0
#define VEC_SILVERISH		Vector(0.75, 0.75, 0.75)
#define RGB_VIOLET			0x00BF00FF
#define VEC_VIOLET			Vector(0.75, 0.0f, 1.0f)
#define RGB_ORANGE			0x00FF8000
#define VEC_ORANGE			Vector(1.0f, 0.5f, 0.0f)
#define RGB_GRAY			0x00CCCCCC
#define VEC_GRAY			Vector(0.8, 0.8, 0.8)

#define DHN_DRAWZERO	BIT(0)
#define DHN_2DIGITS		BIT(1)
#define DHN_3DIGITS		BIT(2)
#define DHN_FILLZERO	BIT(3)
#define DHN_4DIGITS		BIT(4)
#define DHN_5DIGITS		BIT(5)

#define MIN_ALPHA		128

#define MAX_SPRITE_NAME_LENGTH 24

#define MAX_PLAYER_NAME_LENGTH 128
#define MAX_MOTD_LENGTH 1536

#define FADE_TIME 100

extern cvar_t* cl_lw;
extern cvar_t* cl_righthand;
extern cvar_t* cl_radartype;
extern cvar_t* cl_dynamiccrosshair;
extern cvar_t* cl_crosshair_color;
extern cvar_t* cl_crosshair_size;
extern cvar_t* cl_crosshair_translucent;
extern cvar_t* cl_crosshair_type;
extern cvar_t* cl_killeffect;
extern cvar_t* cl_killmessage;
extern cvar_t* cl_headname;
extern cvar_t* cl_newmenu;
extern cvar_t* cl_newmenu_drawbox;
extern cvar_t* cl_newradar;
extern cvar_t* cl_newradar_size;
extern cvar_t* cl_newradar_dark;
extern cvar_t* cl_newchat;
extern cvar_t* cl_shadows;
extern cvar_t* cl_scoreboard;
extern cvar_t* cl_fog_skybox;
extern cvar_t* cl_fog_density;
extern cvar_t* cl_fog_r;
extern cvar_t* cl_fog_g;
extern cvar_t* cl_fog_b;
extern cvar_t* cl_minmodel;
extern cvar_t* cl_min_t;
extern cvar_t* cl_min_ct;
extern cvar_t* cl_corpsestay;
extern cvar_t* cl_corpsefade;

extern Vector g_LocationColor;

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
} client_sprite_t;

typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	Vector origin;

	// fields that can be changed by the cldll
	Vector viewangles;
	int		iWeaponBits;
	//	int		iAccessoryBits;
	float	fov;	// field of view
} client_data_t;

typedef struct SCREENINFO_s
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];
} SCREENINFO;

typedef struct hud_player_info_s
{
	char* name;
	short ping;
	byte thisplayer;  // TRUE if this is the calling player

	byte spectator;
	byte packetloss;

	char* model;
	short topcolor;
	short bottomcolor;

	uint64 m_nSteamID;
} hud_player_info_t;

class CBaseHudElement
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}
	CBaseHudElement() noexcept {}
	CBaseHudElement(const CBaseHudElement& s) = default;
	CBaseHudElement(CBaseHudElement&& s) = default;
	CBaseHudElement& operator=(const CBaseHudElement& s) = default;
	CBaseHudElement& operator=(CBaseHudElement&& s) = default;
	virtual ~CBaseHudElement() {}

public:
	int	m_bitsFlags{ 0 };// active, moving,
	float m_flLastX{ 0.0f };	// last drawn position
	float m_flLastY{ 0.0f };	// last drawn position

public:
	virtual int Init(void) { return 0; }
	virtual int VidInit(void) { return 0; }
	virtual int Draw(float flTime) { return 0; }
	virtual void Think(void) {}
	virtual void Reset(void) {}
	virtual void InitHUDData(void) {}		// called every time a server is connected to
	virtual void Shutdown(void) {}
};

/*
class CHudExample : public CBaseHudElement
{
public:
	virtual int Init(void) { return 0; }
	virtual int VidInit(void) { return 0; }
	virtual int Draw(float flTime) { return 0; }
	virtual void Think(void) {}
	virtual void Reset(void) {}
	virtual void InitHUDData(void) {}
	virtual void Shutdown(void) {}
};

struct CHudExample2
{
	// Event functions.
	static void	Initialize(void);
	static void	Shutdown(void);
	static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	static void	OnNewRound(void);
	static void	ServerAsksReset(void);

	// Message functions.
	// Custom functions.
	// Game data.
	// Drawing data.
};
*/

// dummy classes derived from CBaseHudElement.
//class CHudAmmo;
//class CHudHealth;
//class CHudSpectator;
class CHudGeiger;
//class CHudBattery;
class CHudTrain;
//class CHudFlashlight;
class CHudMessage;
class CHudStatusBar;
//class CHudDeathNotice;
class CHudSayText;
class CHudMenu;
class CHudNightVision;
class CHudTextMessage;
class CHudRoundTimer;
//class CHudAccountBalance;
class CHudHeadName;
//class CHudRadar;
//class CHudStatusIcons;
//class CHudScenarioStatus;
class CHudProgressBar;
class CHudVGUI2Print;
class CHudSniperScope;
//class CHudCrosshair;
class CHudWeaponList;
//class CHudGrenade;
class CHudScoreboard;
//class CHudClassIndicator;
class CUIBuyMenu;

namespace gHUD
{
	// HUD exports
	void Init(void);
	void Shutdown(void);
	void VidInit(void);
	int Redraw(float flTime, int intermission);
	void Think(void);
	int UpdateClientData(client_data_t* cdata, float time);
	void CalcRefdef(ref_params_s* pparams);
	bool KeyEvent(bool bDown, int iKeyIndex, const char* pszCurrentBinding);	// Return true to allow engine to process the key, otherwise, act on it as needed

	typedef struct element_s
	{
		void	(*pfnInitialize)		(void);
		void	(*pfnShutdown)			(void);
		void	(*pfnConnectToServer)	(void);
		void	(*pfnDraw)				(float flTime, bool bIntermission);
		void	(*pfnThink)				(void);	// Use gHUD::m_flUCDTime
		void	(*pfnOnNewRound)		(void);
		void	(*pfnServerAsksReset)	(void);

		/*
		static void	Initialize		(void);
		static void	Shutdown		(void);
		static void	ConnectToServer	(void);
		static void	Draw			(float flTime, bool bIntermission);
		static void	Think			(void);	// Use gHUD::m_flUCDTime
		static void	OnNewRound		(void);
		static void	ServerAsksReset	(void);
		*/

	} element_t;

	// HUD utils
	int GetSpriteIndex(const char* SpriteName);
	int DrawHudString(int xpos, int ypos, int iMaxX, const char* szIt, int r, int g, int b);
	int DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b);
	int DrawHudStringReverse(int xpos, int ypos, int iMinX, const char* szString, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iNumber, int r, int g, int b);
	int GetNumWidth(int iNumber, int iFlags);
	int GetNumBits(int iNumber);
	void AddHudElem(CBaseHudElement* phudelem);
	float GetSensitivity(void);
	hSprite GetSprite(int index);
	const wrect_t* GetSpriteRect(int index);
	client_sprite_t* GetSpriteFromList(client_sprite_t* pList, const char* psz, int iRes, int iCount);	// get a specific SPR from a list. i.e., a TXT file list.
	void GetSprite(client_sprite_t* pSprite, hSprite& hSPR, wrect_t& rcSPR);
	Vector GetColor(size_t iPlayerIndex);
	bool GetSprite(const char* szSpriteName, hSprite* phSPR, const wrect_t** pprcSPR);
	float GetOscillation(float omega = 2);
	float GetOscillationUnfreezable(float omega = 2);
	void AddFontFiles(void);	// Have to do this extremely early.

	// HUD bridges
	void SlotInput(int iSlot);

	template<typename... CElements>
	inline void AddElementsToList(void)
	{
		(CElements::Initialize(), ...);
	}

	//inline void AddElementsToList(void (*func ...)(void))
	//{
	//	(func(), ...);
	//}

	// VARs
	extern std::list<CBaseHudElement*> m_lstHudElements;

	extern cvar_s* m_pCvarDraw;

	extern client_sprite_t* m_pSpriteList;
	extern int m_iPlayerNum;
	extern float m_flTime;
	extern float m_fOldTime;
	extern double m_flTimeDelta;
	extern int m_iFOV;
	extern int m_iLastFOVDiff;
	extern float m_flDisplayedFOV;
	extern int m_iSpriteCount;
	extern int m_iSpriteCountAllRes;
	extern int m_iRes;
	extern std::vector<hSprite> m_rghSprites;
	extern std::vector<wrect_t> m_rgrcRects;
	extern std::vector<std::string> m_rgszSpriteNames;
	extern char m_szGameMode[32];
	extern int m_HUD_number_0;
	extern int m_iFontHeight;
	extern int m_iFontEngineHeight;
	extern bool m_bIntermission;
	extern char m_szMOTD[2048];
	extern float m_flTimeLeft;
	extern int m_bitsHideHUDDisplay;
	extern float m_flMouseSensitivity;
	extern int m_PlayerFOV[MAX_CLIENTS];
	extern Vector m_vecOrigin;
	extern Vector m_vecAngles;
	extern int m_iKeyBits;
	extern int m_iWeaponBits;
	extern bool m_bPlayerDead;
	extern int m_hCambriaFont;
	extern int m_hTrajanProFont;
	extern bool m_bCursorAllowed;
	extern float m_flUCDTime;
	extern float m_flUCDOldTime;
	extern float m_flUCDTimeDelta;
	extern std::array<hSprite, AMMO_MAXTYPE> m_rghAmmoSprite;
	extern std::array<wrect_t, AMMO_MAXTYPE> m_rgrcAmmoSprite;
	extern client_sprite_t* m_pAmmoTxtList;	// original data. for others to inquiry.
	extern int m_iAmmoTxtListCount;

	extern SCREENINFO m_scrinfo;

	// HUD elements.
	extern std::list<element_t> m_lstElements;
	//extern CHudHealth m_Health;
	//extern CHudSpectator m_Spectator;
	extern CHudGeiger m_Geiger;
	//extern CHudBattery m_Battery;
	extern CHudTrain m_Train;
	//extern CHudFlashlight m_Flash;
	extern CHudMessage m_Message;
	extern CHudStatusBar m_StatusBar;
	//extern CHudDeathNotice m_DeathNotice;
	extern CHudSayText m_SayText;
	extern CHudMenu m_Menu;
	extern CHudNightVision m_NightVision;
	extern CHudTextMessage m_TextMessage;
	extern CHudRoundTimer m_roundTimer;
	//extern CHudAccountBalance m_accountBalance;
	extern CHudHeadName m_headName;
	//extern CHudRadar m_Radar;
	//extern CHudStatusIcons m_StatusIcons;
	//extern CHudScenarioStatus m_scenarioStatus;
	extern CHudProgressBar m_progressBar;
	extern CHudVGUI2Print m_VGUI2Print;
	extern CHudSniperScope m_SniperScope;
	//extern CHudCrosshair m_Crosshair;
	extern CHudWeaponList m_WeaponList;
	//extern CHudGrenade m_Grenade;
	extern CHudScoreboard m_Scoreboard;
	//extern CHudClassIndicator m_ClassIndicator;
	extern CUIBuyMenu m_UI_BuyMenu;
};

class CScreenFade
{
public:	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}

	enum SF_PHASE : BYTE
	{
		SF_FADEIN = 1,
		SF_STAY,
		SF_FADEOUT

	};

public:
	color24		m_sColor;
	float		m_flAlpha;
	SF_PHASE	m_iPhase;
	float		m_flTimeThink;
	float		m_flStayLength;
	float		m_flFadeSpeed;
	color24		m_sTargetColor;
	float		m_flTargetAlpha;

	void		Draw	(void);
	void		Think	(void);

	inline void	SetTargetColor(int r, int g, int b) { m_sTargetColor.r = r; m_sTargetColor.g = g; m_sTargetColor.b = b; }
	inline void	SetCurrentColor(int r, int g, int b) { m_sColor.r = r; m_sColor.g = g; m_sColor.b = b; }
	inline void	SetAlpha(float flTarget, float flCur = -1) { m_flTargetAlpha = Q_clamp(flTarget, 0.0f, 255.0f); m_flAlpha = Q_clamp(flCur, 0.0f, 255.0f); }
	inline void	Start(SF_PHASE phase = SF_FADEIN) { m_flTimeThink = 0; m_iPhase = phase; }
	inline void	SetFadeSpeed(float flSpeed) { m_flFadeSpeed = flSpeed; }
	inline void SetStayLength(float flLength = 0) { m_flStayLength = flLength; }
};

namespace OverviewMgr
{
	// variables
	extern float m_flZoom;
	extern Vector2D m_vecOrigin;
	extern bool m_bRotated;
	extern GLuint m_iIdTexture;
	extern int m_iHeight;
	extern int m_iWidth;
	extern Matrix3x3 m_mxTransform;
	extern model_t* m_pMapSprite;
	extern Vector2D m_vecInsetWindowAnchor;	// upper left corner
	extern Vector2D m_vecInsetWindowSize;

	// event calls.
	void OnHUDReset(void);

	// functions
	bool LoadOverviewInfo(const char* pszFilePath);
};

extern CScreenFade gScreenFadeMgr;

extern hud_player_info_t g_PlayerInfoList[MAX_PLAYERS + 1];
extern int g_PlayerScoreAttrib[MAX_PLAYERS + 1];
extern TEMPENTITY* g_DeadPlayerModels[MAX_PLAYERS + 1];

// LUNA: I think these two should move to weapons.cpp. Why are they here??
extern EquipmentIdType FindNextEquipment(bool bLoopFromStart, EquipmentIdType iStartingFrom = gPseudoPlayer.m_iUsingGrenadeId);
extern EquipmentIdType FindLastEquipment(bool bLoopFromEnd, EquipmentIdType iStartingFrom = gPseudoPlayer.m_iUsingGrenadeId);
extern int CountEquipments(void);

// HUD command funcs
void CommandFunc_Slot1(void);
void CommandFunc_Slot2(void);
void CommandFunc_Slot3(void);
void CommandFunc_Slot4(void);
void CommandFunc_Slot5(void);
void CommandFunc_Slot6(void);
void CommandFunc_Slot7(void);
void CommandFunc_Slot8(void);
void CommandFunc_Slot9(void);
void CommandFunc_Slot10(void);
void CommandFunc_CancelSelect(void);
void CommandFunc_NextWeapon(void);
void CommandFunc_PrevWeapon(void);
void CommandFunc_SelectLastItem(void);
void CommandFunc_NextEquipment(void);
void CommandFunc_PrevEquipment(void);
void CommandFunc_AlterAct(void);

#define ScreenHeight (gHUD::m_scrinfo.iHeight)
#define ScreenWidth (gHUD::m_scrinfo.iWidth)

#define XPROJECT(x)	((1.0f + (x)) * ScreenWidth * 0.5f)
#define YPROJECT(y) ((1.0f - (y)) * ScreenHeight * 0.5f)

// Use this to set any co-ords in 640x480 space
inline int XRES(float x) { return static_cast<int>(x * (static_cast<float>(ScreenWidth) / 640.0f) + 0.5f); }
inline int YRES(float y) { return static_cast<int>(y * (static_cast<float>(ScreenHeight) / 480.0f) + 0.5f); }

// for HUD_PostRunCmd
extern float g_lastFOV;
