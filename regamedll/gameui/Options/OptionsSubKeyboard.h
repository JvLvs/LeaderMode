/*

Copied Date: May 25 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef OPTIONS_SUB_KEYBOARD_H
#define OPTIONS_SUB_KEYBOARD_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/UtlVector.h"
#include "tier1/UtlSymbol.h"

#include <vgui_controls/PropertyPage.h>
class VControlsListPanel;

#define BUTTON_CODE_LAST	256
typedef int ButtonCode_t;

#define BUTTON_CODE_INVALID	((ButtonCode_t)-1)
#define BUTTON_CODE_NONE	((ButtonCode_t)0)

//-----------------------------------------------------------------------------
// Purpose: Keyboard Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubKeyboard : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubKeyboard, vgui::PropertyPage);

public:
	COptionsSubKeyboard(vgui::Panel* parent);
	~COptionsSubKeyboard();

	virtual void	OnResetData();
	virtual void	OnApplyChanges();
	virtual void	OnKeyCodePressed(vgui::KeyCode code);
	virtual void	OnThink();

	// Trap row selection message
	MESSAGE_FUNC_INT(ItemSelected, "ItemSelected", itemID);

private:
	void Finish(ButtonCode_t code, ButtonCode_t button = 0, const char* pchKeyName = NULL);

	//-----------------------------------------------------------------------------
	// Purpose: Used for saving engine keybindings in case user hits cancel button
	//-----------------------------------------------------------------------------
	struct KeyBinding
	{
		char* binding;
	};

	// Create the key binding list control
	void			CreateKeyBindingList(void);

	virtual void	OnCommand(const char* command);

	// Tell engine to bind/unbind a key
	void			BindKey(const char* key, const char* binding);
	void			UnbindKey(const char* key);

	// Save/restore/cleanup engine's current bindings ( for handling cancel button )
	void			SaveCurrentBindings(void);
	void			DeleteSavedBindings(void);

	// Get column 0 action descriptions for all keys
	void			ParseActionDescriptions(void);

	// Populate list of actions with current engine keybindings
	void			FillInCurrentBindings(void);
	// Remove all current bindings from list of bindings
	void			ClearBindItems(void);
	// Fill in bindings with mod-specified defaults
	void			FillInDefaultBindings(void);
	// Copy bindings out of list and set them in the engine
	void			ApplyAllBindings(void);

	// Bind a key to the item
	void			AddBinding(KeyValues* item, const char* keyname);

	// Remove all instances of a key from all bindings
	void			RemoveKeyFromBindItems(KeyValues* org_item, const char* key);

	// Find item by binding name
	KeyValues* GetItemForBinding(const char* binding);

private:
	void OpenKeyboardAdvancedDialog();
	vgui::DHANDLE<class COptionsSubKeyboardAdvancedDlg> m_OptionsSubKeyboardAdvancedDlg;
	virtual void OnMousePressed(vgui::MouseCode code);
	virtual void OnMouseWheeled(int delta);
	virtual void OnKeyCodeTyped(vgui::KeyCode code);

	VControlsListPanel* m_pKeyBindList;

	vgui::Button* m_pSetBindingButton;
	vgui::Button* m_pClearBindingButton;

	// List of saved bindings for the keys
	KeyBinding m_Bindings[BUTTON_CODE_LAST];

	// List of all the keys that need to have their binding removed
	CUtlVector<CUtlSymbol> m_KeysToUnbind;
};

#endif // OPTIONS_SUB_KEYBOARD_H
