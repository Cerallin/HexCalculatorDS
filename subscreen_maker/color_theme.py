"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""
from contour import hex_to_rgb


class ColorTheme:
    def common(self):
        return [
            hex_to_rgb(self.BORDER_COLORS[0]),   # contour
            hex_to_rgb(self.SHADOW_COLORS[0]),   # shadow
            hex_to_rgb(self.TEXT_COLORS[0]),     # text
            hex_to_rgb(self.BG_COLORS[0]),       # bg
            hex_to_rgb(self.BG_COLORS[0]),       # sign (hidden)
        ]

    def disabled(self):
        return [
            hex_to_rgb(self.BORDER_COLORS[1]),   # contour
            hex_to_rgb(self.SHADOW_COLORS[1]),   # shadow
            hex_to_rgb(self.TEXT_COLORS[1]),     # text
            hex_to_rgb(self.BG_COLORS[1]),       # bg
            hex_to_rgb(self.BG_COLORS[1]),       # sign (hidden)
        ]

    def selected(self):
        return [
            hex_to_rgb(self.BORDER_COLORS[0]),   # contour
            hex_to_rgb(self.SHADOW_COLORS[0]),   # shadow
            hex_to_rgb(self.TEXT_COLORS[0]),     # text
            hex_to_rgb(self.BG_COLORS[0]),       # bg
            hex_to_rgb(self.BORDER_COLORS[0]),   # sign (same as contour)
        ]

class LightTheme(ColorTheme):
    BORDER_COLORS = ["82163d", "676666"]
    SHADOW_COLORS = ["ddbfca", "cecccc"]
    TEXT_COLORS = ["000000", "676666"]
    BG_COLORS = ["f8f8f3", "f0eeee"]

class DarkTheme(ColorTheme):
    BORDER_COLORS = ["ddbfca", "676666"]
    SHADOW_COLORS = ["82163d", "404034"]
    TEXT_COLORS = ["f8f2ff", "676666"]
    BG_COLORS = ["1c1822", "343430"]
