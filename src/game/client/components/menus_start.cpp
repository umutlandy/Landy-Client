/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "menus_start.h"

#include <engine/client/updater.h>
#include <engine/font_icons.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>

#include <generated/client_data.h>

#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/localization.h>
/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "menus_start.h"

#include <engine/client/updater.h>
#include <engine/font_icons.h>
#include <engine/graphics.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>

#include <generated/client_data.h>

#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/localization.h>
#include <game/version.h>

#if defined(CONF_PLATFORM_ANDROID)
#include <android/android_main.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <engine/external/stb_image/stb_image.h>

#include <base/log.h>
#include <algorithm>
#include <cstdio>

static IGraphics::CTextureHandle LoadLandyTexture(const char *pFileName, IGraphics *pGraphics)
{
	char aPathBuffer[512];
	const char *aCandidates[] = {
		"landymenupng/%s",
		"./landymenupng/%s",
		"../landymenupng/%s",
		"build/landymenupng/%s",
		"build/Release/landymenupng/%s",
		"Release/landymenupng/%s",
		"data/landymenupng/%s"
	};

	FILE *pFile = nullptr;
	char aResolvedPath[512] = {0};

	for(const char *pCandidate : aCandidates)
	{
		str_format(aPathBuffer, sizeof(aPathBuffer), pCandidate, pFileName);
		pFile = fopen(aPathBuffer, "rb");
		if(pFile)
		{
			str_copy(aResolvedPath, aPathBuffer);
			fclose(pFile);
			break;
		}
	}

	if(aResolvedPath[0] == '\0')
	{
		log_error("LandyClient", "Failed to load: landymenupng/%s (File not found)", pFileName);
		return IGraphics::CTextureHandle();
	}

	int Width = 0, Height = 0, Channels = 0;
	// Force 4 channels (RGBA)
	stbi_uc *pPixels = stbi_load(aResolvedPath, &Width, &Height, &Channels, 4);

	if(!pPixels)
	{
		log_error("LandyClient", "Failed to decode: %s (stb_image error: %s)", aResolvedPath, stbi_failure_reason());
		return IGraphics::CTextureHandle();
	}

	CImageInfo ImageInfo;
	ImageInfo.m_Width = Width;
	ImageInfo.m_Height = Height;
	ImageInfo.m_Format = CImageInfo::FORMAT_RGBA;
	ImageInfo.m_pData = pPixels;

	IGraphics::CTextureHandle Handle = pGraphics->LoadTextureRaw(ImageInfo, 0, pFileName);

	log_info("LandyClient", "Loaded asset: landymenupng/%s | Path: %s | Dimensions: %dx%d | Channels: %d | Alpha: YES | Handle ID: %d",
		pFileName, aResolvedPath, Width, Height, Channels, Handle.Id());

	stbi_image_free(pPixels);
	return Handle;
}

void CMenusStart::OnInit()
{
	m_TextureBackground = LoadLandyTexture("background.jpg", Graphics());
	m_TextureLogo = LoadLandyTexture("landy_client.png", Graphics());
	m_TexturePlay = LoadLandyTexture("oyna.png", Graphics());
	m_TextureDemos = LoadLandyTexture("demo.png", Graphics());
	m_TextureEditor = LoadLandyTexture("editor.png", Graphics());
	m_TextureServer = LoadLandyTexture("sunucu.png", Graphics());
	m_TextureSettings = LoadLandyTexture("ayarlar.png", Graphics());
	m_TextureQuit = LoadLandyTexture("quit.png", Graphics());
}

int CMenusStart::DoLandyButton(SLandyButton *pBtn, IGraphics::CTextureHandle Texture, const CUIRect *pRect)
{
	const void *pId = &pBtn->m_Container;
	int Clicked = Ui()->DoButtonLogic(pId, 0, pRect, BUTTONFLAG_LEFT);
	bool IsHovered = (Ui()->HotItem() == pId || Ui()->MouseHovered(pRect));
	bool IsPressed = (Ui()->ActiveItem() == pId);

	float TargetScale = 1.0f;
	float TargetBrightness = 1.0f;
	if(IsPressed)
	{
		TargetScale = 0.98f;
		TargetBrightness = 0.92f;
	}
	else if(IsHovered)
	{
		TargetScale = 1.03f;
		TargetBrightness = 1.08f;
	}

	float FrameTime = Client()->RenderFrameTime();
	pBtn->m_Scale += (TargetScale - pBtn->m_Scale) * std::min(1.0f, 15.0f * FrameTime);

	if(Texture.IsValid())
	{
		float BaseW = pRect->w;
		float BaseH = pRect->h;
		float DrawW = BaseW * pBtn->m_Scale;
		float DrawH = BaseH * pBtn->m_Scale;
		float CenterX = pRect->x + BaseW * 0.5f;
		float CenterY = pRect->y + BaseH * 0.5f;

		Graphics()->TextureSet(Texture);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(TargetBrightness, TargetBrightness, TargetBrightness, 1.0f);
		IGraphics::CQuadItem Quad(CenterX - DrawW * 0.5f, CenterY - DrawH * 0.5f, DrawW, DrawH);
		Graphics()->QuadsDrawTL(&Quad, 1);
		Graphics()->QuadsEnd();
	}

	return Clicked;
}

void CMenusStart::RenderStartMenu(CUIRect MainView)
{
	if(!m_TextureLogo.IsValid())
	{
		OnInit();
	}

	CUIRect FullScreen = *Ui()->Screen();
	float GlobalTime = Client()->GlobalTime();
	float FrameTime = Client()->RenderFrameTime();

	// 1. Render Fullscreen Background
	if(m_TextureBackground.IsValid())
	{
		float ImageAspect = 2688.0f / 1536.0f; // ~1.75
		float ScreenAspect = FullScreen.w / FullScreen.h;
		float DrawW, DrawH, DrawX, DrawY;

		if(ScreenAspect > ImageAspect)
		{
			DrawW = FullScreen.w;
			DrawH = FullScreen.w / ImageAspect;
			DrawX = 0.0f;
			DrawY = (FullScreen.h - DrawH) * 0.5f;
		}
		else
		{
			DrawH = FullScreen.h;
			DrawW = FullScreen.h * ImageAspect;
			DrawX = (FullScreen.w - DrawW) * 0.5f;
			DrawY = 0.0f;
		}

		Graphics()->TextureSet(m_TextureBackground);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		IGraphics::CQuadItem BgQuad(DrawX, DrawY, DrawW, DrawH);
		Graphics()->QuadsDrawTL(&BgQuad, 1);
		Graphics()->QuadsEnd();
	}

	// 2. Render Animated Landy Client Logo (~25% larger than before)
	float LogoW = std::min(FullScreen.w * 0.58f, 600.0f);
	float LogoH = LogoW / 5.945f;
	float LogoX = (FullScreen.w - LogoW) * 0.5f;
	float LogoY = 30.0f;

	// Animation math:
	// A. Micro floating Y movement (sine wave)
	float FloatY = std::sin(GlobalTime * 1.6f) * 1.8f;

	// B. Micro scale breathing (±0.5%)
	float ScaleFactor = 1.0f + std::sin(GlobalTime * 1.2f) * 0.005f;

	// C. Micro glitch / shimmer (rare, 1-2 frames)
	m_GlitchTimer += FrameTime;
	if(m_GlitchTimer >= m_NextGlitchTime)
	{
		m_GlitchTimer = 0.0f;
		m_NextGlitchTime = 3.5f + (float)(rand() % 35) / 10.0f;
		m_GlitchDuration = 0.06f + (float)(rand() % 4) / 100.0f;
	}

	bool IsGlitching = (m_GlitchTimer < m_GlitchDuration);
	float GlitchShiftX = 0.0f;
	float GlitchBrightness = 1.0f;

	if(IsGlitching)
	{
		GlitchShiftX = (rand() % 2 == 0 ? 1.8f : -1.8f);
		GlitchBrightness = 1.15f;
	}

	float AnimW = LogoW * ScaleFactor;
	float AnimH = LogoH * ScaleFactor;
	float AnimX = LogoX + (LogoW - AnimW) * 0.5f + GlitchShiftX;
	float AnimY = LogoY + FloatY + (LogoH - AnimH) * 0.5f;

	if(m_TextureLogo.IsValid())
	{
		Graphics()->TextureSet(m_TextureLogo);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(GlitchBrightness, GlitchBrightness, GlitchBrightness, 1.0f);
		IGraphics::CQuadItem LogoQuad(AnimX, AnimY, AnimW, AnimH);
		Graphics()->QuadsDrawTL(&LogoQuad, 1);
		Graphics()->QuadsEnd();
	}

	// 3. Render Main Menu Buttons Stack
	// Visual order: oyna, demo, editor, sunucu, ayarlar
	float BtnW = std::min(FullScreen.w * 0.35f, 350.0f);
	float PlayH = BtnW / 4.3527f;
	float DemosH = BtnW / 5.3807f;
	float EditorH = BtnW / 5.5489f;
	float ServerH = BtnW / 6.2515f;
	float SettingsH = BtnW / 6.6081f;

	float Spacing = 8.0f;
	float TotalStackH = PlayH + DemosH + EditorH + ServerH + SettingsH + 4.0f * Spacing;

	float LogoBottom = LogoY + LogoH;
	float AvailableHeight = FullScreen.h - LogoBottom - 70.0f;
	float StartY = LogoBottom + (AvailableHeight - TotalStackH) * 0.5f;
	if(StartY < LogoBottom + 10.0f)
		StartY = LogoBottom + 10.0f;

	float CenterX = FullScreen.w * 0.5f;
	int NewPage = -1;

	// Oyna
	CUIRect PlayRect;
	PlayRect.w = BtnW;
	PlayRect.h = PlayH;
	PlayRect.x = CenterX - BtnW * 0.5f;
	PlayRect.y = StartY;
	if(DoLandyButton(&m_BtnPlay, m_TexturePlay, &PlayRect) || Ui()->ConsumeHotkey(CUi::HOTKEY_ENTER) || CheckHotKey(KEY_P))
	{
		NewPage = g_Config.m_UiPage >= CMenus::PAGE_INTERNET && g_Config.m_UiPage <= CMenus::PAGE_FAVORITE_COMMUNITY_5 ? g_Config.m_UiPage : CMenus::PAGE_INTERNET;
	}

	// Demolar
	CUIRect DemosRect;
	DemosRect.w = BtnW;
	DemosRect.h = DemosH;
	DemosRect.x = CenterX - BtnW * 0.5f;
	DemosRect.y = PlayRect.y + PlayH + Spacing;
	if(DoLandyButton(&m_BtnDemos, m_TextureDemos, &DemosRect) || CheckHotKey(KEY_D))
	{
		NewPage = CMenus::PAGE_DEMOS;
	}

	// Editör
	CUIRect EditorRect;
	EditorRect.w = BtnW;
	EditorRect.h = EditorH;
	EditorRect.x = CenterX - BtnW * 0.5f;
	EditorRect.y = DemosRect.y + DemosH + Spacing;
	if(DoLandyButton(&m_BtnEditor, m_TextureEditor, &EditorRect) || CheckHotKey(KEY_E))
	{
		g_Config.m_ClEditor = 1;
		Input()->MouseModeRelative();
	}

	// Sunucuyu başlat
	CUIRect ServerRect;
	ServerRect.w = BtnW;
	ServerRect.h = ServerH;
	ServerRect.x = CenterX - BtnW * 0.5f;
	ServerRect.y = EditorRect.y + EditorH + Spacing;

	// Check DDNet real local server state for active glow (#2E7B6E teal)
	const bool LocalServerRunning = GameClient()->m_LocalServer.IsServerRunning();
	float TargetGlowAlpha = LocalServerRunning ? 1.0f : 0.0f;
	float FadeSpeed = LocalServerRunning ? 6.0f : 8.0f;
	m_ServerGlowAlpha += (TargetGlowAlpha - m_ServerGlowAlpha) * std::min(1.0f, FadeSpeed * FrameTime);

	// Render soft teal active glow behind ServerRect
	if(m_ServerGlowAlpha > 0.001f)
	{
		float Pulse = 0.38f + 0.12f * (0.5f + 0.5f * std::sin(GlobalTime * 2.5f));
		float CurrentAlpha = m_ServerGlowAlpha * Pulse;

		float BaseR = 46.0f / 255.0f;  // #2E7B6E teal
		float BaseG = 123.0f / 255.0f;
		float BaseB = 110.0f / 255.0f;

		Graphics()->TextureClear();

		// Outer glow layer
		float Margin3 = 14.0f;
		Graphics()->QuadsBegin();
		Graphics()->SetColor(BaseR, BaseG, BaseB, CurrentAlpha * 0.25f);
		IGraphics::CQuadItem Quad3(ServerRect.x - Margin3, ServerRect.y - Margin3, ServerRect.w + Margin3 * 2.0f, ServerRect.h + Margin3 * 2.0f);
		Graphics()->QuadsDrawTL(&Quad3, 1);
		Graphics()->QuadsEnd();

		// Middle glow layer
		float Margin2 = 8.0f;
		Graphics()->QuadsBegin();
		Graphics()->SetColor(BaseR, BaseG, BaseB, CurrentAlpha * 0.45f);
		IGraphics::CQuadItem Quad2(ServerRect.x - Margin2, ServerRect.y - Margin2, ServerRect.w + Margin2 * 2.0f, ServerRect.h + Margin2 * 2.0f);
		Graphics()->QuadsDrawTL(&Quad2, 1);
		Graphics()->QuadsEnd();

		// Inner glow layer
		float Margin1 = 4.0f;
		Graphics()->QuadsBegin();
		Graphics()->SetColor(BaseR * 1.1f, BaseG * 1.1f, BaseB * 1.1f, CurrentAlpha * 0.70f);
		IGraphics::CQuadItem Quad1(ServerRect.x - Margin1, ServerRect.y - Margin1, ServerRect.w + Margin1 * 2.0f, ServerRect.h + Margin1 * 2.0f);
		Graphics()->QuadsDrawTL(&Quad1, 1);
		Graphics()->QuadsEnd();
	}

	if(DoLandyButton(&m_BtnServer, m_TextureServer, &ServerRect) || (CheckHotKey(KEY_R) && Input()->KeyPress(KEY_R)))
	{
		if(LocalServerRunning)
		{
			GameClient()->m_LocalServer.KillServer();
		}
		else
		{
			GameClient()->m_LocalServer.RunServer({});
		}
	}

	// Ayarlar
	CUIRect SettingsRect;
	SettingsRect.w = BtnW;
	SettingsRect.h = SettingsH;
	SettingsRect.x = CenterX - BtnW * 0.5f;
	SettingsRect.y = ServerRect.y + ServerH + Spacing;
	if(DoLandyButton(&m_BtnSettings, m_TextureSettings, &SettingsRect) || CheckHotKey(KEY_S))
	{
		NewPage = CMenus::PAGE_SETTINGS;
	}

	// 4. Render Quit Button near bottom-left corner
	float QuitW = 55.0f;
	float QuitH = 55.0f * (253.0f / 248.0f);
	CUIRect QuitRect;
	QuitRect.x = 35.0f;
	QuitRect.y = FullScreen.h - QuitH - 35.0f;
	QuitRect.w = QuitW;
	QuitRect.h = QuitH;

	bool UsedEscape = false;
	if(DoLandyButton(&m_BtnQuit, m_TextureQuit, &QuitRect) || (UsedEscape = Ui()->ConsumeHotkey(CUi::HOTKEY_ESCAPE)) || CheckHotKey(KEY_Q))
	{
		if(UsedEscape || GameClient()->Editor()->HasUnsavedData() || (GameClient()->CurrentRaceTime() / 60 >= g_Config.m_ClConfirmQuitTime && g_Config.m_ClConfirmQuitTime >= 0))
		{
			GameClient()->m_Menus.ShowQuitPopup();
		}
		else
		{
			Client()->Quit();
		}
	}

	// 5. Version Info & Console Toggle in Bottom-Right Corner
	CUIRect CurVersion, ConsoleButton;
	MainView.HSplitBottom(35.0f, nullptr, &CurVersion);
	CurVersion.VSplitRight(30.0f, &CurVersion, nullptr);
	CurVersion.HSplitTop(18.0f, &ConsoleButton, &CurVersion);
	ConsoleButton.VSplitRight(32.0f, nullptr, &ConsoleButton);
	Ui()->DoLabel(&CurVersion, GAME_RELEASE_VERSION, 12.0f, TEXTALIGN_MR);

	static CButtonContainer s_ConsoleButton;
	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH | ETextRenderFlags::TEXT_RENDER_FLAG_NO_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_Y_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_PIXEL_ALIGNMENT | ETextRenderFlags::TEXT_RENDER_FLAG_NO_OVERSIZE);
	if(GameClient()->m_Menus.DoButton_Menu(&s_ConsoleButton, FontIcon::TERMINAL, 0, &ConsoleButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.1f)))
	{
		GameClient()->m_GameConsole.Toggle(CGameConsole::CONSOLETYPE_LOCAL);
	}
	TextRender()->SetRenderFlags(0);
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);

	if(NewPage != -1)
	{
		GameClient()->m_Menus.SetShowStart(false);
		GameClient()->m_Menus.SetMenuPage(NewPage);
	}
}

bool CMenusStart::CheckHotKey(int Key) const
{
	return !Input()->ShiftIsPressed() && !Input()->ModifierIsPressed() && !Input()->AltIsPressed() &&
	       Input()->KeyPress(Key) &&
	       !GameClient()->m_GameConsole.IsActive();
}
