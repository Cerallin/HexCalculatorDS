"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""

import os
import sys
from typing import Optional

import cv2
import numpy as np

from image import Palette, IndexedImage, Bmp8Writer, ImagePreprocessor
from contour import ContourAnalyzer, ContourRecord, hex_to_bgr, rgb_to_16bit
from color_theme import ColorTheme, LightTheme, DarkTheme

DEBUG = False

lightTheme = LightTheme()
darkTheme = DarkTheme()

defaultTheme = lightTheme

# Second-row band used to locate the Circular shadow pill.
_CIRCULAR_BAND = (34, 57, 35, 155)

# Contours below this Y (after preprocess) are ignored except equals / copyright.
_TOP_ROWS_MAX_Y = 57

# Padding added around detected Circular arrow glyphs for touch hit boxes.
_ARROW_HIT_PAD = 2


class HitArea:
    """Axis-aligned hit box written to the AREA_* header (may not be drawn)."""

    def __init__(self, x, y, w, h, alias=None):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.alias = alias

    def position(self):
        return self.x, self.y, self.w, self.h


class BinaryCalculatorExporter:

    def __init__(self, image):
        self.image = image

    def preprocess(self):
        bg = hex_to_bgr(defaultTheme.BG_COLORS[0])

        # Version digits (same corner as hex number strip)
        ImagePreprocessor.fill_rect(self.image, (0, 0), (75, 12), bg)
        # Width & sign drawers (WORD / S) — same region as run.py
        ImagePreprocessor.fill_rect(self.image, (159, 4), (94, 28), bg)
        # Four binary rows + subscripts; stop before copyright tab (left edge ~241)
        ImagePreprocessor.fill_rect(self.image, (6, 62), (235, 106), bg)

        # Match hex exporter crop so copyright strip can close and size matches NDS.
        self.image = ImagePreprocessor.crop(self.image, 5, 6, 5, 4)

    def _collect_record(self, contour) -> ContourRecord:
        rec = ContourRecord(contour)

        rec.shadow_points = ContourAnalyzer.collect_outer_ring(
            self.image, contour, defaultTheme.SHADOW_COLORS)

        inner = ContourAnalyzer.collect_inner_colors(self.image, contour)

        button_activated = any(
            k in defaultTheme.BG_COLORS[0] for k in inner.keys())

        if button_activated:
            colors_to_check = {
                "text": defaultTheme.TEXT_COLORS[0],
                "bg": defaultTheme.BG_COLORS[0],
                "shadow": defaultTheme.SHADOW_COLORS[0],
                "sign": defaultTheme.BORDER_COLORS[0],
            }
        else:
            colors_to_check = {
                "text": defaultTheme.TEXT_COLORS[1],
                "bg": defaultTheme.BG_COLORS[1],
                "shadow": defaultTheme.SHADOW_COLORS[1],
                "sign": defaultTheme.BORDER_COLORS[1],
            }

        for k, pts in inner.items():
            if k == colors_to_check["text"]:
                rec.text_points = pts
            elif k == colors_to_check["bg"]:
                rec.bg_points = pts
            elif k == colors_to_check["shadow"]:
                rec.shadow_points += pts
            elif k == colors_to_check["sign"]:
                rec.sign_points = pts
            else:
                pass

        return rec

    def _find_circular(self) -> Optional[ContourRecord]:
        """Circular mode pill uses shadow colors as its body, not border ink."""
        y0, y1, x0, x1 = _CIRCULAR_BAND
        mask = ContourAnalyzer.build_mask(self.image, defaultTheme.SHADOW_COLORS)
        region = np.zeros_like(mask)
        region[y0:y1, x0:x1] = mask[y0:y1, x0:x1]

        contours, _ = cv2.findContours(
            region, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        if not contours:
            return None

        contour = max(contours, key=cv2.contourArea)
        if cv2.contourArea(contour) < 200:
            return None

        rec = ContourRecord(contour)
        x, y, w, h = rec.position()

        # Expand slightly so flanking arrow glyphs are included.
        pad = 12
        h_img, w_img, _ = self.image.shape
        rx0 = max(0, x - pad)
        ry0 = max(0, y - 2)
        rx1 = min(w_img, x + w + pad)
        ry1 = min(h_img, y + h + 2)

        border_hex = set(defaultTheme.BORDER_COLORS)
        shadow_hex = set(defaultTheme.SHADOW_COLORS)
        text_hex = set(defaultTheme.TEXT_COLORS)
        bg_hex = set(defaultTheme.BG_COLORS)

        # Contour outline from the shadow pill path; arrows collected as text.
        rec.contour_points = [(px, py) for [[px, py]] in contour]

        for py in range(ry0, ry1):
            for px in range(rx0, rx1):
                b, g, r = self.image[py, px]
                key = f"{r:02x}{g:02x}{b:02x}"
                if key in bg_hex:
                    continue
                if key in text_hex or key in border_hex:
                    rec.text_points.append((px, py))
                elif key == defaultTheme.SHADOW_COLORS[1]:
                    rec.bg_points.append((px, py))
                elif key == defaultTheme.SHADOW_COLORS[0]:
                    if (px, py) not in rec.contour_points:
                        rec.shadow_points.append((px, py))

        # Fill remaining shadow interior
        inner = ContourAnalyzer.collect_inner_colors(self.image, contour)
        for k, pts in inner.items():
            if k in shadow_hex:
                if k == defaultTheme.SHADOW_COLORS[0]:
                    rec.shadow_points += pts
                else:
                    rec.bg_points += pts
            elif k in border_hex or k in text_hex:
                rec.text_points += pts

        rec.shadow_points = list(dict.fromkeys(rec.shadow_points))
        rec.bg_points = list(dict.fromkeys(rec.bg_points))
        rec.text_points = list(dict.fromkeys(rec.text_points))
        text_set = set(rec.text_points)
        rec.bg_points = [p for p in rec.bg_points if p not in text_set]
        rec.shadow_points = [p for p in rec.shadow_points if p not in text_set]

        return rec

    def _find_circular_arrows(self, pill_box):
        """Bounding boxes of the ◀ / ▶ glyphs flanking the Circular pill."""
        px, py, pw, ph = pill_box
        h_img, w_img, _ = self.image.shape

        ink = ContourAnalyzer.build_mask(
            self.image,
            list(defaultTheme.TEXT_COLORS) + [defaultTheme.BORDER_COLORS[0]],
        )

        def bbox_in_strip(x0, x1):
            x0 = max(0, x0)
            x1 = min(w_img, x1)
            if x1 <= x0:
                return None
            strip = ink[py:py + ph, x0:x1]
            ys, xs = np.where(strip > 0)
            if len(xs) == 0:
                return None
            ax = int(xs.min()) + x0
            ay = int(ys.min()) + py
            aw = int(xs.max() - xs.min()) + 1
            ah = int(ys.max() - ys.min()) + 1
            pad = _ARROW_HIT_PAD
            ax = max(0, ax - pad)
            ay = max(0, ay - pad)
            aw = min(w_img - ax, aw + 2 * pad)
            ah = min(h_img - ay, ah + 2 * pad)
            return HitArea(ax, ay, aw, ah)

        left = bbox_in_strip(px - 16, px)
        right = bbox_in_strip(px + pw, px + pw + 16)
        if left is not None:
            left.alias = "SHIFT_MODE_PREV"
        if right is not None:
            right.alias = "SHIFT_MODE_NEXT"
        return left, right

    def analyze(self):
        mask = ContourAnalyzer.build_mask(
            self.image, defaultTheme.BORDER_COLORS)
        contours = ContourAnalyzer.find_contours(mask)

        circular = self._find_circular()
        circular_box = circular.position() if circular else None
        arrow_left = arrow_right = None
        if circular_box is not None:
            arrow_left, arrow_right = self._find_circular_arrows(circular_box)

        def inside_circular(x, y, w, h):
            if circular_box is None:
                return False
            cx, cy, cw, ch = circular_box
            # Contour center inside circular pill → label fragment, drop it
            mx, my = x + w // 2, y + h // 2
            return (cx <= mx < cx + cw) and (cy <= my < cy + ch)

        records = []
        copyright_record = None
        equal_record = None

        for c in contours:
            x, y, w, h = cv2.boundingRect(c)
            area = cv2.contourArea(c)

            # Right-edge copyright strip (full-height tab)
            if h > 100:
                copyright_record = ContourRecord(c)
                continue

            if area < 200:
                continue

            if inside_circular(x, y, w, h):
                continue

            # Equals: bottom-right filled circle
            if y > _TOP_ROWS_MAX_Y:
                equal_record = self._collect_record(c)
                continue

            if y >= _TOP_ROWS_MAX_Y:
                continue

            records.append(self._collect_record(c))

        if circular is not None:
            records.append(circular)

        # Propagate focus signs to all same-shaped buttons (top rows only)
        signed_records = [r for r in records if len(r.sign_points) > 0]
        for rec in signed_records:
            sign_points = rec.sign_points.copy()
            x, y, _, _ = rec.position()
            relative_sign_points = [(sx - x, sy - y) for sx, sy in sign_points]

            for other in records:
                if other.shape_key != rec.shape_key:
                    continue
                ox, oy, _, _ = other.position()
                other.sign_points = [(ox + rx, oy + ry)
                                     for rx, ry in relative_sign_points]

        if equal_record is not None:
            # Filled '=' : border-colored body may be classified as text when
            # there is no black glyph; fold body into contour, glyph into text.
            equal_record.contour_points += equal_record.sign_points
            equal_record.contour_points += equal_record.text_points
            equal_record.sign_points = []
            equal_record.text_points = equal_record.bg_points
            equal_record.bg_points = []
            records.append(equal_record)

        if copyright_record is not None:
            # Same as run.py: only pixels inside the closed tab contour
            copyright_inner = ContourAnalyzer.collect_inner_colors(
                self.image, copyright_record.contour)
            for k, pts in copyright_inner.items():
                if k == defaultTheme.BG_COLORS[0]:
                    copyright_record.bg_points = pts
                elif k == defaultTheme.BORDER_COLORS[0]:
                    copyright_record.contour_points = pts
                else:
                    pass
            records.append(copyright_record)

        def sort_key_pos(pos):
            x, y, w, h = pos
            if h > 100:
                row = 3  # copyright last
            elif y < 30:
                row = 0
            elif y < 60:
                row = 1
            else:
                row = 2  # equals
            return (row, x)

        records.sort(key=lambda r: sort_key_pos(r.position()))

        # Hit areas for header: drawn buttons + Circular side arrows
        hit_areas = [HitArea(*r.position()) for r in records]
        for arrow in (arrow_left, arrow_right):
            if arrow is not None:
                hit_areas.append(arrow)
        hit_areas.sort(key=lambda a: sort_key_pos(a.position()))

        self.image = ImagePreprocessor.crop(self.image, 0, 0, 0, 1)

        return records, hit_areas

    def build_image(self, records: list[ContourRecord], theme: ColorTheme, width, height):
        palette = Palette()
        palette.generate_demo(
            theme.common(),
            theme.disabled(),
            theme.selected(),
            len(records),
            offset=16,
        )

        img = IndexedImage(width, height)
        pal_idx = 16

        for record in records:
            palette_map = {
                "contour": pal_idx,
                "shadow": pal_idx + 1,
                "text": pal_idx + 2,
                "bg": pal_idx + 3,
                "sign": pal_idx + 4,
            }
            record.draw(img, palette_map)
            pal_idx += 5

        return img, palette

    def build_c_header(self, hit_areas, theme, output_dir, prefix):
        output_area_file = os.path.join(output_dir, f"{prefix}.h")
        output_color_file = os.path.join(output_dir, f"{prefix}Colors.h")

        with open(output_area_file, "w") as f:
            f.write("#ifndef SUBSCREEN_BINARY_AREA_H\n")
            f.write("#define SUBSCREEN_BINARY_AREA_H\n\n")
            for idx, area in enumerate(hit_areas):
                x, y, w, h = area.position()
                f.write(f"#define AREA_{idx}_X {x}\n")
                f.write(f"#define AREA_{idx}_Y {y}\n")
                f.write(f"#define AREA_{idx}_W {w}\n")
                f.write(f"#define AREA_{idx}_H {h}\n")
                if area.alias:
                    f.write("\n")
                    f.write(f"#define AREA_{area.alias}_X AREA_{idx}_X\n")
                    f.write(f"#define AREA_{area.alias}_Y AREA_{idx}_Y\n")
                    f.write(f"#define AREA_{area.alias}_W AREA_{idx}_W\n")
                    f.write(f"#define AREA_{area.alias}_H AREA_{idx}_H\n")
                f.write("\n")
            f.write("\n")
            f.write("#endif // SUBSCREEN_BINARY_AREA_H\n")

        with open(output_color_file, "w") as f:
            f.write("#ifndef SUBSCREEN_BINARY_COLOR_H\n")
            f.write("#define SUBSCREEN_BINARY_COLOR_H\n\n")

            colors = theme.common()
            f.write("// Common colors\n")
            f.write(f"#define COLOR_COMMON_BORDER {rgb_to_16bit(*colors[0]):#06x}\n")
            f.write(f"#define COLOR_COMMON_SHADOW {rgb_to_16bit(*colors[1]):#06x}\n")
            f.write(f"#define COLOR_COMMON_TEXT   {rgb_to_16bit(*colors[2]):#06x}\n")
            f.write(f"#define COLOR_COMMON_BG     {rgb_to_16bit(*colors[3]):#06x}\n")
            f.write(f"#define COLOR_COMMON_SIGN   {rgb_to_16bit(*colors[4]):#06x}\n")

            colors = theme.disabled()
            f.write("// Disabled colors\n")
            f.write(f"#define COLOR_DISABLED_BORDER {rgb_to_16bit(*colors[0]):#06x}\n")
            f.write(f"#define COLOR_DISABLED_SHADOW {rgb_to_16bit(*colors[1]):#06x}\n")
            f.write(f"#define COLOR_DISABLED_TEXT   {rgb_to_16bit(*colors[2]):#06x}\n")
            f.write(f"#define COLOR_DISABLED_BG     {rgb_to_16bit(*colors[3]):#06x}\n")
            f.write(f"#define COLOR_DISABLED_SIGN   {rgb_to_16bit(*colors[4]):#06x}\n")

            colors = theme.selected()
            f.write("// Selected colors\n")
            f.write(f"#define COLOR_SELECTED_BORDER {rgb_to_16bit(*colors[0]):#06x}\n")
            f.write(f"#define COLOR_SELECTED_SHADOW {rgb_to_16bit(*colors[1]):#06x}\n")
            f.write(f"#define COLOR_SELECTED_TEXT   {rgb_to_16bit(*colors[2]):#06x}\n")
            f.write(f"#define COLOR_SELECTED_BG     {rgb_to_16bit(*colors[3]):#06x}\n")
            f.write(f"#define COLOR_SELECTED_SIGN   {rgb_to_16bit(*colors[4]):#06x}\n")

            f.write("\n#endif // SUBSCREEN_BINARY_COLOR_H\n")

    def export(self, output_file, theme):
        self.preprocess()
        records, hit_areas = self.analyze()
        if DEBUG:
            for idx, area in enumerate(hit_areas):
                x, y, w, h = area.position()
                alias = f" ({area.alias})" if area.alias else ""
                print(f"Area   {idx:2}: pos=({x:3},{y:3}),\tsize=({w:2}x{h:2}){alias}")
            for idx, rec in enumerate(records):
                x, y, w, h = rec.position()
                print(f"Record {idx:2}: pos=({x:3},{y:3}),\tsize=({w:2}x{h:2}),\t"
                      f"{len(rec.shadow_points):2} shadow,\t"
                      f"{len(rec.text_points):2} text,\t"
                      f"{len(rec.bg_points):3} bg,\t"
                      f"{len(rec.sign_points):2} sign")

        output_dir = os.path.dirname(output_file) or "."
        prefix = os.path.splitext(os.path.basename(output_file))[0]
        self.build_c_header(hit_areas, theme, output_dir, prefix)

        h, w, _ = self.image.shape
        image, palette = self.build_image(records, theme, w, h)

        Bmp8Writer.save(output_file, image.get_array(), palette.get())


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python run_binary.py theme input output_file")
        sys.exit(1)

    theme_name = sys.argv[1]
    input_image = sys.argv[2]
    output_file = sys.argv[3]

    theme_map = {
        "light": LightTheme(),
        "dark": DarkTheme(),
    }

    if theme_name not in theme_map:
        print(f"Unknown theme: {theme_name}")
        sys.exit(1)

    img = ImagePreprocessor.read_image(input_image)
    if img is None:
        print(f"Failed to load image {input_image}")
        sys.exit(1)

    exporter = BinaryCalculatorExporter(img)
    exporter.export(output_file, theme_map[theme_name])
