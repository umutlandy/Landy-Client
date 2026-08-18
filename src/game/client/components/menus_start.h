/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_MENUS_START_H
#define GAME_CLIENT_COMPONENTS_MENUS_START_H

#include <engine/graphics.h>
#include <game/client/component.h>
#include <game/client/ui.h>
#include <game/client/ui_rect.h>

class CMenusStart : public CComponentInterfaces
{
public:
	void OnInit();
	void RenderStartMenu(CUIRect MainView);

private:
	struct SLandyButton
	{
		CButtonContainer m_Container;
		float m_Scale = 1.0f;
	};

	int DoLandyButton(SLandyButton *pBtn, IGraphics::CTextureHandle Texture, const CUIRect *pRect);
	bool CheckHotKey(int Key) const;

	IGraphics::CTextureHandle m_TextureBackground;
	IGraphics::CTextureHandle m_TextureLogo;
	IGraphics::CTextureHandle m_TexturePlay;
	IGraphics::CTextureHandle m_TextureDemos;
	IGraphics::CTextureHandle m_TextureEditor;
	IGraphics::CTextureHandle m_TextureServer;
	IGraphics::CTextureHandle m_TextureSettings;
	IGraphics::CTextureHandle m_TextureQuit;

	SLandyButton m_BtnPlay;
	SLandyButton m_BtnDemos;
	SLandyButton m_BtnEditor;
	SLandyButton m_BtnServer;
	SLandyButton m_BtnSettings;
	SLandyButton m_BtnQuit;

	float m_ServerGlowAlpha = 0.0f;
	float m_GlitchTimer = 0.0f;
	float m_NextGlitchTime = 3.5f;
	float m_GlitchDuration = 0.0f;
};

#endif

