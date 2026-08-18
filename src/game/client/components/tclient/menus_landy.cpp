#include <base/log.h>
#include <base/math.h>
#include <base/system.h>
#include <base/types.h>

#include <engine/font_icons.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/localization.h>
#include <engine/storage.h>
#include <engine/textrender.h>

#include <game/client/animstate.h>
#include <game/client/components/menus.h>
#include <game/client/gameclient.h>
#include <game/client/lineinput.h>
#include <game/client/ui.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

#include <algorithm>
#include <vector>

const float FontSize = 14.0f;
const float LineSize = 20.0f;
const float HeadlineFontSize = 20.0f;
const float StandardFontSize = 14.0f;

const float HeadlineHeight = HeadlineFontSize + 0.0f;
const float Margin = 10.0f;
const float MarginSmall = 5.0f;
const float MarginExtraSmall = 2.5f;
const float MarginBetweenSections = 30.0f;
const float MarginBetweenViews = 30.0f;

void CMenus::RenderSettingsLandy(CUIRect MainView)
{
	CUIRect Column, LeftView, RightView, Button, Label;

	static CScrollRegion s_ScrollRegion;
	vec2 ScrollOffset(0.0f, 0.0f);
	CScrollRegionParams ScrollParams;
	ScrollParams.m_ScrollUnit = 60.0f;
	ScrollParams.m_Flags = CScrollRegionParams::FLAG_CONTENT_STATIC_WIDTH;
	ScrollParams.m_ScrollbarMargin = 5.0f;
	s_ScrollRegion.Begin(&MainView, &ScrollOffset, &ScrollParams);

	static std::vector<CUIRect> s_SectionBoxes;
	static vec2 s_PrevScrollOffset(0.0f, 0.0f);

	MainView.y += ScrollOffset.y;

	MainView.VSplitRight(5.0f, &MainView, nullptr); // Padding for scrollbar
	MainView.VSplitLeft(5.0f, nullptr, &MainView);  // Padding for scrollbar

	MainView.VSplitMid(&LeftView, &RightView, MarginBetweenViews);
	LeftView.VSplitLeft(MarginSmall, nullptr, &LeftView);
	RightView.VSplitRight(MarginSmall, &RightView, nullptr);

	// Draw section background boxes from the previous frame
	for(CUIRect &Section : s_SectionBoxes)
	{
		float Padding = MarginBetweenViews * 0.6666f;
		Section.w += Padding;
		Section.h += Padding;
		Section.x -= Padding * 0.5f;
		Section.y -= Padding * 0.5f;
		Section.y -= s_PrevScrollOffset.y - ScrollOffset.y;
		Section.Draw(ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f), IGraphics::CORNER_ALL, 10.0f);
	}
	s_PrevScrollOffset = ScrollOffset;
	s_SectionBoxes.clear();

	// ***** LeftView ***** //
	Column = LeftView;

	// Landy Header Panel
	Column.HSplitTop(Margin, nullptr, &Column);
	s_SectionBoxes.push_back(Column);

	CUIRect HeaderPanel;
	Column.HSplitTop(60.0f, &HeaderPanel, &Column);

	CUIRect LabelRect, TeeRect;
	HeaderPanel.VSplitRight(60.0f, &LabelRect, &TeeRect);

	LabelRect.HSplitTop(HeadlineHeight, &Label, &LabelRect);
	Label.y += (HeaderPanel.h - HeadlineHeight) * 0.5f;
	Ui()->DoLabel(&Label, TCLocalize("Landy Client"), HeadlineFontSize, TEXTALIGN_ML);

	// Player Tee Preview following mouse cursor
	{
		const char *pSkinName = g_Config.m_ClPlayerSkin;
		const CSkin *pDefaultSkin = GameClient()->m_Skins.Find("default");
		const CSkins::CSkinContainer *pOwnSkinContainer = GameClient()->m_Skins.FindContainerOrNullptr(pSkinName[0] == '\0' ? "default" : pSkinName);
		if(pOwnSkinContainer != nullptr && pOwnSkinContainer->IsSpecial())
		{
			pOwnSkinContainer = nullptr;
		}

		CTeeRenderInfo OwnSkinInfo;
		OwnSkinInfo.Apply(pOwnSkinContainer == nullptr || pOwnSkinContainer->Skin() == nullptr ? pDefaultSkin : pOwnSkinContainer->Skin().get());
		OwnSkinInfo.ApplyColors(g_Config.m_ClPlayerUseCustomColor, g_Config.m_ClPlayerColorBody, g_Config.m_ClPlayerColorFeet);
		OwnSkinInfo.m_Size = 50.0f;

		vec2 OffsetToMid;
		CRenderTools::GetRenderTeeOffsetToRenderedTee(CAnimState::GetIdle(), &OwnSkinInfo, OffsetToMid);
		const vec2 TeeRenderPos = vec2(TeeRect.x + TeeRect.w / 2.0f, TeeRect.y + TeeRect.h / 2.0f + OffsetToMid.y);

		const vec2 DeltaPosition = Ui()->MousePos() - TeeRenderPos;
		const float Distance = length(DeltaPosition);
		const float InteractionDistance = 20.0f;
		const vec2 TeeDirection = Distance < InteractionDistance ? normalize(vec2(DeltaPosition.x, maximum(DeltaPosition.y, 0.5f))) : normalize(DeltaPosition);
		const int TeeEmote = Distance < InteractionDistance ? EMOTE_HAPPY : g_Config.m_ClPlayerDefaultEyes;

		RenderTools()->RenderTee(CAnimState::GetIdle(), &OwnSkinInfo, TeeEmote, TeeDirection, TeeRenderPos);
	}

	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// Collapsible states
	static bool s_CollapseInput = false;
	static bool s_CollapseKingOfGores = false;
	static bool s_CollapsePractice = false;
	static bool s_CollapseCommunity = false;
	static bool s_CollapseExperimental = false;

	// Input Section
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);

	Column.HSplitTop(HeadlineHeight, &Button, &Column);
	char aInputLabel[128];
	str_format(aInputLabel, sizeof(aInputLabel), "%s  %s", s_CollapseInput ? "[+] " : "[-] ", TCLocalize("Input"));
	static CButtonContainer s_InputHeaderBtn;
	if(DoButton_Menu(&s_InputHeaderBtn, aInputLabel, 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		s_CollapseInput = !s_CollapseInput;
	}

	if(!s_CollapseInput)
	{
		Column.HSplitTop(MarginSmall, nullptr, &Column);
		Column.HSplitTop(HeadlineHeight, &Label, &Column);
		Ui()->DoLabel(&Label, TCLocalize("Input"), HeadlineFontSize, TEXTALIGN_ML);
		Column.HSplitTop(MarginSmall, nullptr, &Column);

		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_TcFastInput, TCLocalize("Enable Input"), &g_Config.m_TcFastInput, &Column, LineSize);

		Column.HSplitTop(LineSize, &Label, &Column);
		Ui()->DoLabel(&Label, TCLocalize("Input Mode"), FontSize, TEXTALIGN_ML);
		Column.HSplitTop(MarginExtraSmall, nullptr, &Column);

		Column.HSplitTop(LineSize, &Button, &Column);
		static CButtonContainer s_aInputModeButtons[4];
		const char *apModeLabels[] = {"TClient", "Landy", "Amadeus+", "Sasuke+"};
		for(int Mode = 0; Mode < 4; Mode++)
		{
			CUIRect ModeButton;
			Button.VSplitLeft(Button.w / (4 - Mode), &ModeButton, &Button);
			const bool Selected = g_Config.m_LandyFastInputMode == Mode;
			if(DoButton_Menu(&s_aInputModeButtons[Mode], TCLocalize(apModeLabels[Mode]), Selected, &ModeButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f,
				Selected ? ColorRGBA(0.0f, 0.5f, 0.25f, 0.5f) : ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
			{
				g_Config.m_LandyFastInputMode = Mode;
				g_Config.m_LandyAmadeusPlus = Mode == 2;
				g_Config.m_LandySasukePlus = Mode == 3;
			}
		}

		Column.HSplitTop(MarginSmall, nullptr, &Column);
		if(g_Config.m_LandyFastInputMode == 0 || g_Config.m_LandyFastInputMode == 1)
		{
			Column.HSplitTop(LineSize, &Button, &Column);
			DoSliderWithScaledValue(&g_Config.m_TcFastInputAmount, &g_Config.m_TcFastInputAmount, &Button, TCLocalize("Amount"), 1, 40, 1, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "ms");
			DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_TcFastInputOthers, TCLocalize("Fast Input others"), &g_Config.m_TcFastInputOthers, &Column, LineSize);
			DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClSubTickAiming, TCLocalize("Sub-Tick aiming"), &g_Config.m_ClSubTickAiming, &Column, LineSize);
		}
		else if(g_Config.m_LandyFastInputMode == 2)
		{
			Column.HSplitTop(LineSize, &Button, &Column);
			Ui()->DoScrollbarOption(&g_Config.m_LandyAmadeusAmount, &g_Config.m_LandyAmadeusAmount, &Button, TCLocalize("Amount"), 100, 500, &CUi::ms_LinearScrollbarScale, 0);
			Column.HSplitTop(LineSize, &Button, &Column);
			Ui()->DoScrollbarOption(&g_Config.m_LandyAmadeusCorrection, &g_Config.m_LandyAmadeusCorrection, &Button, TCLocalize("Correction"), 0, 100, &CUi::ms_LinearScrollbarScale, 0, "%");
			DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_LandyAmadeusOthers, TCLocalize("Input others"), &g_Config.m_LandyAmadeusOthers, &Column, LineSize);
		}
		else
		{
			Column.HSplitTop(LineSize, &Button, &Column);
			Ui()->DoScrollbarOption(&g_Config.m_LandySasukeAmount, &g_Config.m_LandySasukeAmount, &Button, TCLocalize("Amount"), 100, 500, &CUi::ms_LinearScrollbarScale, 0);
			Column.HSplitTop(LineSize, &Button, &Column);
			Ui()->DoScrollbarOption(&g_Config.m_LandySasukeCorrection, &g_Config.m_LandySasukeCorrection, &Button, TCLocalize("Correction"), 0, 100, &CUi::ms_LinearScrollbarScale, 0, "%");
			DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_LandySasukeOthers, TCLocalize("Input others"), &g_Config.m_LandySasukeOthers, &Column, LineSize);
		}

		Column.HSplitTop(MarginSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// Graphics Section
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, TCLocalize("Custom Aspect Ratio"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	Column.HSplitTop(LineSize, &Label, &Column);
	Ui()->DoLabel(&Label, TCLocalize("Preset"), FontSize, TEXTALIGN_ML);
	Column.HSplitTop(LineSize, &Button, &Column);
	static CButtonContainer s_aAspectPresetButtons[5];
	const char *apPresets[] = {"Off", "5:4", "4:3", "3:2", "Custom"};
	for(int Preset = 0; Preset < 5; Preset++)
	{
		CUIRect PresetButton;
		Button.VSplitLeft(Button.w / (5 - Preset), &PresetButton, &Button);
		const bool Selected = g_Config.m_LandyCustomAspectPreset == Preset;
		if(DoButton_Menu(&s_aAspectPresetButtons[Preset], TCLocalize(apPresets[Preset]), Selected, &PresetButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 3.0f, 0.0f,
			Selected ? ColorRGBA(0.0f, 0.5f, 0.25f, 0.5f) : ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
			g_Config.m_LandyCustomAspectPreset = Preset;
	}

	if(g_Config.m_LandyCustomAspectPreset != 0)
	{
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_LandyCustomAspectGameOnly, TCLocalize("Apply Only To Game"), &g_Config.m_LandyCustomAspectGameOnly, &Column, LineSize);
		static CLineInputNumber s_WidthInput, s_HeightInput;
		static int s_LastWidth = -1, s_LastHeight = -1;
		if(!s_WidthInput.IsActive() && s_LastWidth != g_Config.m_LandyCustomAspectWidth) { s_WidthInput.SetInteger(g_Config.m_LandyCustomAspectWidth); s_LastWidth = g_Config.m_LandyCustomAspectWidth; }
		if(!s_HeightInput.IsActive() && s_LastHeight != g_Config.m_LandyCustomAspectHeight) { s_HeightInput.SetInteger(g_Config.m_LandyCustomAspectHeight); s_LastHeight = g_Config.m_LandyCustomAspectHeight; }

		if(g_Config.m_LandyCustomAspectPreset == 4)
		{
			Column.HSplitTop(LineSize, &Button, &Column);
			if(Ui()->DoEditBox(&s_WidthInput, &Button, FontSize))
				g_Config.m_LandyCustomAspectWidth = s_WidthInput.GetInteger();
			Column.HSplitTop(LineSize, &Button, &Column);
			if(Ui()->DoEditBox(&s_HeightInput, &Button, FontSize))
				g_Config.m_LandyCustomAspectHeight = s_HeightInput.GetInteger();
			if(g_Config.m_LandyCustomAspectWidth < 640 || g_Config.m_LandyCustomAspectWidth > 7680 || g_Config.m_LandyCustomAspectHeight < 480 || g_Config.m_LandyCustomAspectHeight > 4320)
			{
				Column.HSplitTop(LineSize, &Label, &Column);
				Ui()->DoLabel(&Label, TCLocalize("Width must be 640-7680 and height must be 480-4320."), FontSize, TEXTALIGN_ML);
			}
		}

		Column.HSplitTop(LineSize, &Label, &Column);
		Ui()->DoLabel(&Label, TCLocalize("Apply"), FontSize, TEXTALIGN_ML);
		Column.HSplitTop(LineSize, &Button, &Column);
		static CButtonContainer s_ApplyFullButton;
		if(DoButton_Menu(&s_ApplyFullButton, TCLocalize("Full"), 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 3.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
		{
			const int Height = Graphics()->ScreenHeight();
			int Width = Graphics()->ScreenWidth();
			if(g_Config.m_LandyCustomAspectPreset == 1) Width = Height * 5 / 4;
			else if(g_Config.m_LandyCustomAspectPreset == 2) Width = Height * 4 / 3;
			else if(g_Config.m_LandyCustomAspectPreset == 3) Width = Height * 3 / 2;
			else if(g_Config.m_LandyCustomAspectPreset == 4)
			{
				Width = g_Config.m_LandyCustomAspectWidth;
				if(Width < 640 || Width > 7680 || g_Config.m_LandyCustomAspectHeight < 480 || g_Config.m_LandyCustomAspectHeight > 4320)
					Width = 0;
				else
					g_Config.m_LandyCustomAspectAppliedHeight = g_Config.m_LandyCustomAspectHeight;
			}
			if(Width > 0)
			{
				g_Config.m_LandyCustomAspectAppliedWidth = Width;
				if(g_Config.m_LandyCustomAspectPreset != 4)
					g_Config.m_LandyCustomAspectAppliedHeight = Height;
			}
		}

		char aResolution[64];
		str_format(aResolution, sizeof(aResolution), "%s: %d x %d", TCLocalize("Current Render Resolution"), g_Config.m_LandyCustomAspectAppliedWidth, g_Config.m_LandyCustomAspectAppliedHeight);
		Column.HSplitTop(LineSize, &Label, &Column);
		Ui()->DoLabel(&Label, aResolution, FontSize, TEXTALIGN_ML);
		char aAspect[64];
		str_format(aAspect, sizeof(aAspect), "%s: %.3f", TCLocalize("Current Aspect Ratio"), g_Config.m_LandyCustomAspectAppliedWidth / (float)g_Config.m_LandyCustomAspectAppliedHeight);
		Column.HSplitTop(LineSize, &Label, &Column);
		Ui()->DoLabel(&Label, aAspect, FontSize, TEXTALIGN_ML);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// Gores Mode Section
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	
	Column.HSplitTop(HeadlineHeight, &Button, &Column);
	char aKogLabel[128];
	str_format(aKogLabel, sizeof(aKogLabel), "%s  %s", s_CollapseKingOfGores ? "[+] " : "[-] ", TCLocalize("Gores Mode"));
	static CButtonContainer s_KogHeaderBtn;
	if(DoButton_Menu(&s_KogHeaderBtn, aKogLabel, 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		s_CollapseKingOfGores = !s_CollapseKingOfGores;
	}

	if(!s_CollapseKingOfGores)
	{
		Column.HSplitTop(MarginSmall, nullptr, &Column);
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_BcGoresMode, TCLocalize("Enable gores mode"), &g_Config.m_BcGoresMode, &Column, LineSize);
		if(g_Config.m_BcGoresMode)
			DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_BcGoresModeDisableIfWeapons, TCLocalize("Disable if you have shotgun, grenade or laser"), &g_Config.m_BcGoresModeDisableIfWeapons, &Column, LineSize);
		else
			Column.HSplitTop(LineSize, nullptr, &Column);
		Column.HSplitTop(MarginSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	LeftView = Column;

	// ***** RightView ***** //
	Column = RightView;

	// Practice Section
	Column.HSplitTop(Margin, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	
	Column.HSplitTop(HeadlineHeight, &Button, &Column);
	char aPracticeLabel[128];
	str_format(aPracticeLabel, sizeof(aPracticeLabel), "%s  %s", s_CollapsePractice ? "[+] " : "[-] ", TCLocalize("Practice"));
	static CButtonContainer s_PracticeHeaderBtn;
	if(DoButton_Menu(&s_PracticeHeaderBtn, aPracticeLabel, 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		s_CollapsePractice = !s_CollapsePractice;
	}

	if(!s_CollapsePractice)
	{
		Column.HSplitTop(MarginSmall, nullptr, &Column);
		// Space reserved for future practice settings
		Column.HSplitTop(MarginSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// Community Section
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	
	Column.HSplitTop(HeadlineHeight, &Button, &Column);
	char aCommunityLabel[128];
	str_format(aCommunityLabel, sizeof(aCommunityLabel), "%s  %s", s_CollapseCommunity ? "[+] " : "[-] ", TCLocalize("Community"));
	static CButtonContainer s_CommunityHeaderBtn;
	if(DoButton_Menu(&s_CommunityHeaderBtn, aCommunityLabel, 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		s_CollapseCommunity = !s_CollapseCommunity;
	}

	if(!s_CollapseCommunity)
	{
		Column.HSplitTop(MarginSmall, nullptr, &Column);
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_TcTranslateAutoIncoming, TCLocalize("Auto-translate incoming chat (Others)"), &g_Config.m_TcTranslateAutoIncoming, &Column, LineSize);
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_TcTranslateAutoOutgoing, TCLocalize("Auto-translate outgoing chat (Yours)"), &g_Config.m_TcTranslateAutoOutgoing, &Column, LineSize);
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_BcTranslateOutgoingStripPunctuation, TCLocalize("No commas or periods"), &g_Config.m_BcTranslateOutgoingStripPunctuation, &Column, LineSize);
		Column.HSplitTop(MarginSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// Experimental Section
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	
	Column.HSplitTop(HeadlineHeight, &Button, &Column);
	char aExperimentalLabel[128];
	str_format(aExperimentalLabel, sizeof(aExperimentalLabel), "%s  %s", s_CollapseExperimental ? "[+] " : "[-] ", TCLocalize("Experimental"));
	static CButtonContainer s_ExperimentalHeaderBtn;
	if(DoButton_Menu(&s_ExperimentalHeaderBtn, aExperimentalLabel, 0, &Button, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		s_CollapseExperimental = !s_CollapseExperimental;
	}

	if(!s_CollapseExperimental)
	{
		Column.HSplitTop(MarginSmall, nullptr, &Column);
		// Space reserved for future experimental settings
		Column.HSplitTop(MarginSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	RightView = Column;

	// Scroll
	CUIRect ScrollRegion;
	ScrollRegion.x = MainView.x;
	ScrollRegion.y = maximum(LeftView.y, RightView.y) + MarginSmall * 2.0f;
	ScrollRegion.w = MainView.w;
	ScrollRegion.h = 0.0f;
	s_ScrollRegion.AddRect(ScrollRegion);
	s_ScrollRegion.End();
}
