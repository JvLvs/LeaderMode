/*

Created Date: Mar 11 2020
Remastered Date: May 11 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

struct CHudBattery	// Place after CHudRadar
{
	// Event functions.
	static void	Initialize		(void);
	//static void	Shutdown		(void);
	//static void	ConnectToServer	(void);
	static void	Draw			(float flTime, bool bIntermission);
	static void	Think			(void);
	//static void	OnNewRound		(void);
	//static void	ServerAsksReset	(void);

	// Message functions.
	static void MsgFunc_Battery(const int& iNewArmourValue);
	static void MsgFunc_ArmorType(const int& iArmourType);
#ifdef _DEBUG
	static void CmdFunc_SetArmorType(void);
	static void CmdFunc_SetAP(void);
#endif // _DEBUG


	// Custom functions.
	static float GetMaxArmour(void);
	static void	Reset(void);

	// Game data.
	static inline GLuint m_iIdKevlar = 0U, m_iIdAssaultSuit = 0U;
	static inline unsigned short m_iMaxAP = 100, m_iAP = 0, m_iArmorType = ARMOR_NONE;
	static inline std::wstring m_wcsAPText = L"0";

	// Drawing data.
	static constexpr decltype(auto) COLOR = Vector(8.0 / 255.0, 131.0 / 255.0, 211.0 / 255.0);
	static constexpr decltype(auto) MARGIN_LEFT = 8;	// Margin between this and class indicator.
	static constexpr decltype(auto) MARGIN_BOTTOM = 8;	// Margin between this and HP bar.
	static constexpr decltype(auto) BORDER_THICKNESS = 2;
	static constexpr decltype(auto) PROGRESS_BAR_MARGIN = 2;
	static constexpr decltype(auto) GAP_SIZE = 5;	// Gap between icon and progress bar.
	static constexpr decltype(auto) ICON_SIZE = Vector2D((CHudClassIndicator::PORTRAIT_SIZE.width - MARGIN_BOTTOM) / 2), BAR_SIZE = Vector2D(CHudRadar::SIZE.x - CHudClassIndicator::PORTRAIT_SIZE.width - MARGIN_LEFT - ICON_SIZE.width - GAP_SIZE, ICON_SIZE.height);
	static constexpr decltype(auto) INNERBLOCK_HEIGHT = BAR_SIZE.y - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2;	// standalone, constexpr version. for font initialization.
	static constexpr decltype(auto) TEXT_HEIGHT = INNERBLOCK_HEIGHT;	// Since the text must be placed inside innerblock.
	static inline Vector2D ICON_ANCHOR = Vector2D(), BAR_ANCHOR = Vector2D();
	static inline Vector2D INNERBLOCK_ANCHOR = Vector2D(), INNERBLOCK_SIZE = Vector2D(0, INNERBLOCK_HEIGHT);
	static inline Vector2D TEXT_ANCHOR = Vector2D();
	static inline float m_flAlpha = 255;	// byte
	static inline int m_hFont = 0;
	static inline int m_iTextLength = 1;
};
