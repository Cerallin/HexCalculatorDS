"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""

import sys
import cv2
import numpy as np
from bmp import Palette, IndexedImage, Bmp8Writer
from contour import contour_shape_key

DEBUG = False

# ============================================================
# Utils
# ============================================================


def hex_to_bgr(hex_color: str):
    hex_color = hex_color.strip().lstrip("#")
    r = int(hex_color[0:2], 16)
    g = int(hex_color[2:4], 16)
    b = int(hex_color[4:6], 16)
    return (b, g, r)


def hex_to_rgb(hex_color: str):
    hex_color = hex_color.strip().lstrip("#")
    r = int(hex_color[0:2], 16)
    g = int(hex_color[2:4], 16)
    b = int(hex_color[4:6], 16)
    return (r, g, b)


class ImagePreprocessor:

    @staticmethod
    def crop(image, top, left, bottom, right):
        h, w, _ = image.shape
        return image[top:h-bottom, left:w-right]

    @staticmethod
    def fill_rect(image, pos, shape, bgr):
        x, y = pos
        w, h = shape
        image[y:y+h, x:x+w] = bgr


class ContourAnalyzer:

    BORDER_COLORS = ["82163d", "676666"]
    SHADOW_COLORS = ["ddbfca", "cecccc"]
    TEXT_COLORS = ["000000", "676666"]
    BG_COLORS = ["f8f8f3", "f0eeee"]

    @staticmethod
    def build_mask(image, hex_colors):
        mask = np.zeros(image.shape[:2], dtype=np.uint8)
        for c in hex_colors:
            bgr = np.array(hex_to_bgr(c), dtype=np.uint8)
            match = np.all(image == bgr, axis=2)
            mask[match] = 255
        return mask

    @staticmethod
    def find_contours(mask):
        kernel = np.ones((3, 3), dtype=np.uint8)
        closed = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        contours, _ = cv2.findContours(
            closed, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        return contours

    @staticmethod
    def mask_points(mask):
        ys, xs = np.where(mask)
        return list(zip(xs.tolist(), ys.tolist()))

    @staticmethod
    def collect_outer_ring(image, contour, hex_colors, dist=1):
        h, w, _ = image.shape

        contour_mask = np.zeros((h, w), np.uint8)
        cv2.drawContours(contour_mask, [contour], -1, 255, 1)

        kernel = np.ones((3, 3), np.uint8)
        dilated = cv2.dilate(contour_mask, kernel, iterations=dist)

        filled = np.zeros((h, w), np.uint8)
        cv2.drawContours(filled, [contour], -1, 255, -1)

        ring = cv2.subtract(dilated, filled)

        color_mask = ContourAnalyzer.build_mask(image, hex_colors)
        near = (ring > 0) & (color_mask > 0)

        return ContourAnalyzer.mask_points(near)

    @staticmethod
    def collect_inner_colors(image, contour):
        h, w, _ = image.shape

        filled = np.zeros((h, w), np.uint8)
        boundary = np.zeros((h, w), np.uint8)

        cv2.drawContours(filled, [contour], -1, 255, -1)
        cv2.drawContours(boundary, [contour], -1, 255, 1)

        inner = cv2.subtract(filled, boundary)

        ys, xs = np.where(inner == 255)

        colors = {}
        for y, x in zip(ys, xs):
            b, g, r = image[y, x]
            key = f"{r:02x}{g:02x}{b:02x}"
            colors.setdefault(key, []).append((x, y))

        return colors


class ContourRecord:
    def __init__(self, contour):
        self.contour = contour
        self.shape_key = contour_shape_key(contour)

        self.contour_points = [(x, y) for [[x, y]] in contour]
        self.text_points = []
        self.shadow_points = []
        self.bg_points = []
        self.sign_points = []

    def draw(self, img: IndexedImage, palette_indices: dict):
        img.fill_points(self.shadow_points, palette_indices["shadow"])
        img.fill_points(self.contour_points, palette_indices["contour"])
        img.fill_points(self.text_points, palette_indices["text"])
        img.fill_points(self.bg_points, palette_indices["bg"])
        img.fill_points(self.sign_points, palette_indices["sign"])

    def position(self):
        x, y, w, h = cv2.boundingRect(self.contour)
        return x, y, w, h


class HexCalculatorExporter:

    def __init__(self, image):
        self.image = image

    def preprocess(self):
        bg = hex_to_bgr(ContourAnalyzer.BG_COLORS[0])

        # numbers
        ImagePreprocessor.fill_rect(self.image, (0, 0), (75, 12), bg)
        # number
        ImagePreprocessor.fill_rect(self.image, (88, 78), (5, 6), bg)
        # width & sign
        ImagePreprocessor.fill_rect(self.image, (159, 4), (94, 28), bg)

        self.image = ImagePreprocessor.crop(self.image, 5, 6, 5, 5)

    def analyze(self):

        mask = ContourAnalyzer.build_mask(
            self.image, ContourAnalyzer.BORDER_COLORS)

        contours = ContourAnalyzer.find_contours(mask)

        records = []

        for c in contours:
            rec = ContourRecord(c)

            rec.shadow_points = ContourAnalyzer.collect_outer_ring(
                self.image, c, ContourAnalyzer.SHADOW_COLORS)

            inner = ContourAnalyzer.collect_inner_colors(self.image, c)

            button_activated: bool = any(
                k in ContourAnalyzer.BG_COLORS[0] for k in inner.keys())

            if button_activated:
                colors_to_check = {
                    "text": ContourAnalyzer.TEXT_COLORS[0],
                    "bg": ContourAnalyzer.BG_COLORS[0],
                    "shadow": ContourAnalyzer.SHADOW_COLORS[0],
                    "sign": ContourAnalyzer.BORDER_COLORS[0],
                }
            else:
                colors_to_check = {
                    "text": ContourAnalyzer.TEXT_COLORS[1],
                    "bg": ContourAnalyzer.BG_COLORS[1],
                    "shadow": ContourAnalyzer.SHADOW_COLORS[1],
                    "sign": ContourAnalyzer.BORDER_COLORS[1],
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

            records.append(rec)

        records.sort(key=lambda r: (r.position()[:2]))

        # remove last 2: equal button and copyright
        if len(records) >= 2:
            records = records[:-2]

        # get unique contours
        unique_contours = []
        for rec in records:
            if rec.shape_key not in unique_contours:
                unique_contours.append(rec.shape_key)

        signed_records = [r for r in records if len(r.sign_points) > 0]

        # get relative position of signed records and
        # assign sign points to other records with the same shape
        for rec in signed_records:
            sign_points = rec.sign_points.copy()
            x, y, _, _ = rec.position()
            relative_sign_points = [(sx - x, sy - y) for sx, sy in sign_points]

            for other in records:
                if other.shape_key != rec.shape_key:
                    continue
                if other == rec:
                    pass
                ox, oy, _, _ = other.position()
                other.sign_points = [(ox + rx, oy + ry)
                                     for rx, ry in relative_sign_points]

        return records

    def build_image(self, records, width, height):

        palette = Palette()

        # palette for demo view
        palette.generate_demo(
            common := [
                hex_to_rgb(ContourAnalyzer.BORDER_COLORS[0]),   # contour
                hex_to_rgb(ContourAnalyzer.SHADOW_COLORS[0]),   # shadow
                hex_to_rgb(ContourAnalyzer.TEXT_COLORS[0]),     # text
                hex_to_rgb(ContourAnalyzer.BG_COLORS[0]),       # bg
                hex_to_rgb(ContourAnalyzer.BG_COLORS[0]),       # sign (default to hide)
            ],
            disabled := [
                hex_to_rgb(ContourAnalyzer.BORDER_COLORS[1]),   # contour
                hex_to_rgb(ContourAnalyzer.SHADOW_COLORS[1]),   # shadow
                hex_to_rgb(ContourAnalyzer.TEXT_COLORS[1]),     # text
                hex_to_rgb(ContourAnalyzer.BG_COLORS[1]),       # bg
                hex_to_rgb(ContourAnalyzer.BG_COLORS[1]),       # sign (hidden)
            ],
            selected := [
                hex_to_rgb(ContourAnalyzer.BORDER_COLORS[0]),   # contour
                hex_to_rgb(ContourAnalyzer.SHADOW_COLORS[0]),   # shadow
                hex_to_rgb(ContourAnalyzer.TEXT_COLORS[0]),     # text
                hex_to_rgb(ContourAnalyzer.BG_COLORS[0]),       # bg
                hex_to_rgb(ContourAnalyzer.BORDER_COLORS[0]),   # sign
            ],
            len(records),
        )

        img = IndexedImage(width, height)

        pal_idx = 1

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

    def build_c_header(self, records, output_file):
        with open(output_file, "w") as f:
            f.write("#ifndef SUBSCREEN_AREA_H\n")
            f.write("#define SUBSCREEN_AREA_H\n\n")
            for idx, rec in enumerate(records):
                x, y, w, h = rec.position()
                f.write(f"#define AREA_{idx}_X {x}\n")
                f.write(f"#define AREA_{idx}_Y {y}\n")
                f.write(f"#define AREA_{idx}_W {w}\n")
                f.write(f"#define AREA_{idx}_H {h}\n")
                f.write("\n")
            f.write("\n")
            f.write("#endif // SUBSCREEN_AREA_H\n")

    def export(self, output_file):
        self.preprocess()
        records = self.analyze()
        if DEBUG:
            for idx, rec in enumerate(records):
                x, y, w, h = rec.position()
                print(f"Record {idx:2}: pos=({x:3},{y:3}),\tsize=({w:2}x{h:2}),\t"
                    f"{len(rec.shadow_points):2} shadow,\t"
                    f"{len(rec.text_points):2} text,\t"
                    f"{len(rec.bg_points):3} bg,\t"
                    f"{len(rec.sign_points):2} sign")

        self.build_c_header(records, output_file.replace(".bmp", ".h"))

        h, w, _ = self.image.shape
        image, palette = self.build_image(records, w, h)

        Bmp8Writer.save(output_file, image.get_array(), palette.get())


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: python script.py input output")
        sys.exit(1)

    input_image = sys.argv[1]
    output_file = sys.argv[2]

    img = cv2.imread(input_image)
    if img is None:
        print(f"Failed to load image {input_image}")
        sys.exit(1)

    exporter = HexCalculatorExporter(img)
    exporter.export(output_file)
