/*

Created Date: Mar 12 2020

*/

#pragma once

class CHudVGUI2Print : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);

public:
	int DrawVGUI2StringReverse(const wchar_t* msg, int x, int y, float r, float g, float b);
	int DrawVGUI2StringReverse(const char* charMsg, int x, int y, float r, float g, float b);
	void VGUI2LocalizeArg(const char* pSrc, wchar_t* pDest, int unicodeBufferSizeInBytes);
	void VGUI2HudPrintArgs(const char* charMsg, char* sstr1, char* sstr2, char* sstr3, char* sstr4, int x, int y, float r, float g, float b);
	void VGUI2HudPrint(const char* charMsg, int x, int y, float r, float g, float b);
	int GetHudFontHeight(void);
	void GetStringSize(const wchar_t* string, int* width, int* height);
	int DrawVGUI2String(const wchar_t* msg, int x, int y, float r, float g, float b);
	int DrawVGUI2String(const char* charMsg, int x, int y, float r, float g, float b);

private:
	vgui::HFont m_hudfont;
	float m_flVGUI2StringTime;
	wchar_t m_wCharBuf[1024];
	float m_fR, m_fG, m_fB;
	int m_iX, m_iY;
};
