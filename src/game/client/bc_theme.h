/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_BC_THEME_H
#define GAME_CLIENT_BC_THEME_H

#include <base/color.h>

#include <engine/shared/config.h>

#include <game/client/ui_rect.h>

#include <algorithm>
#include <cmath>

namespace BCTheme
{
// --- Config-driven palette (AMF Assets tab style) ---
// When disabled (bc_menu_theme 0) the palette falls back to the classic
// semi-transparent black look so every themed panel keeps rendering.

inline bool Enabled()
{
	return g_Config.m_BcMenuTheme != 0;
}

inline ColorRGBA PanelSurface()
{
	if(Enabled())
		return color_cast<ColorRGBA>(ColorHSLA(g_Config.m_BcThemeSurfaceColor, true));
	return ColorRGBA(0.0f, 0.0f, 0.0f, 0.5f);
}

inline ColorRGBA PanelSurfaceAlt()
{
	if(Enabled())
	{
		const ColorRGBA Surface = PanelSurface();
		return ColorRGBA(
			std::min(1.0f, Surface.r + 0.012f),
			std::min(1.0f, Surface.g + 0.016f),
			std::min(1.0f, Surface.b + 0.020f),
			Surface.a);
	}
	return ColorRGBA(0.0f, 0.0f, 0.0f, 0.3f);
}

inline ColorRGBA PanelBorder()
{
	if(Enabled())
		return color_cast<ColorRGBA>(ColorHSLA(g_Config.m_BcThemeBorderColor, true));
	return ColorRGBA(0.0f, 0.0f, 0.0f, 0.5f);
}

inline ColorRGBA PanelBorderSoft()
{
	const ColorRGBA Border = PanelBorder();
	return ColorRGBA(Border.r * 0.8f, Border.g * 0.8f, Border.b * 0.8f, Border.a * 0.8f);
}

inline ColorRGBA ButtonNeutral()
{
	if(!Enabled())
		return ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f);
	const ColorRGBA Surface = PanelSurface();
	return ColorRGBA(std::max(0.02f, Surface.r * 2.0f), std::max(0.03f, Surface.g * 2.0f), std::max(0.04f, Surface.b * 2.0f), 0.92f);
}

inline ColorRGBA ButtonNeutralHover()
{
	if(!Enabled())
		return ColorRGBA(1.0f, 1.0f, 1.0f, 0.25f);
	const ColorRGBA Surface = PanelSurface();
	return ColorRGBA(std::min(1.0f, Surface.r * 3.0f), std::min(1.0f, Surface.g * 3.0f), std::min(1.0f, Surface.b * 3.0f), 0.95f);
}

inline ColorRGBA ButtonNeutralDisabled()
{
	const ColorRGBA Surface = PanelSurface();
	return ColorRGBA(Surface.r * 1.2f, Surface.g * 1.2f, Surface.b * 1.2f, 0.52f);
}

// --- Accent colors (derived from interface color) ---

inline ColorRGBA AccentColor(ColorHSLA Hsla, float Lightness, float Alpha)
{
	return color_cast<ColorRGBA>(ColorHSLA(Hsla.h, Hsla.s, std::clamp(Lightness, 0.0f, 1.0f), Alpha));
}

inline float AccentLightness(ColorHSLA Hsla)
{
	return std::clamp(Hsla.l, 0.28f, 0.58f);
}

inline ColorRGBA AccentSelected(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla), 0.94f);
}

inline ColorRGBA AccentHover(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla) + 0.08f, 0.92f);
}

inline ColorRGBA AccentPressed(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla) - 0.08f, 0.96f);
}

inline ColorRGBA AccentSoft(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla), 0.62f);
}

inline ColorRGBA AccentOutline(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla), 0.34f);
}

inline ColorRGBA AccentHeader(ColorHSLA Hsla)
{
	return AccentColor(Hsla, AccentLightness(Hsla), 0.48f);
}

// --- Draw helpers ---

// Two-layer panel: outer border + inner surface (AMF style)
inline void DrawBorderedPanel(class CUIRect *pRect, float Rounding)
{
	CUIRect Outer = *pRect;
	CUIRect Inner = *pRect;
	Outer.Draw(PanelBorder(), IGraphics::CORNER_ALL, Rounding);
	Inner.Margin(1.0f, &Inner);
	Inner.Draw(PanelSurface(), IGraphics::CORNER_ALL, std::max(0.0f, Rounding - 1.0f));
}

// Single dark panel with rounded corners (settings content background)
inline void DrawDarkPanel(class CUIRect *pRect, int Corners, float Rounding)
{
	pRect->Draw(PanelSurface(), Corners, Rounding);
}

// Draw a colored button with border (AMF DoOrganizerButton style)
inline void DrawAccentButton(class CUIRect *pRect, ColorHSLA Hsla, bool Selected, bool Hovered, bool Active, bool Enabled, int Corners = IGraphics::CORNER_ALL, float Rounding = 4.0f)
{
	const float Light = AccentLightness(Hsla);
	const ColorRGBA Fill = !Enabled ? ButtonNeutralDisabled() :
		(Selected ? AccentColor(Hsla, Light, 0.76f) :
			(Active ? AccentPressed(Hsla) :
				(Hovered ? ButtonNeutralHover() : ButtonNeutral())));
	const ColorRGBA Border = !Enabled ? PanelBorderSoft() :
		(Selected ? AccentSelected(Hsla) :
			(Hovered || Active ? AccentSoft(Hsla) : PanelBorder()));

	CUIRect Inner = *pRect;
	pRect->Draw(Border, Corners, Rounding);
	Inner.Margin(1.0f, &Inner);
	Inner.Draw(Fill, Corners, std::max(0.0f, Rounding - 1.0f));
}

} // namespace BCTheme

#endif
