"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""

import sys
from image import Palette, IndexedImage, Bmp8Writer, ImagePreprocessor
from contour import ContourAnalyzer, ContourRecord
from contour import hex_to_bgr, hex_to_rgb

DEBUG = False


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

        # make copyright contour closed
        self.image = ImagePreprocessor.crop(self.image, 5, 6, 5, 4)

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
        equal_record: ContourRecord = records[-2]
        copyright_record: ContourRecord = records[-1]
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

        # add back removed records
        equal_record.contour_points += equal_record.sign_points
        equal_record.sign_points = []

        copyright_inner = ContourAnalyzer.collect_inner_colors(
            self.image, copyright_record.contour)

        for k, pts in copyright_inner.items():
            if k == ContourAnalyzer.BG_COLORS[0]:
                copyright_record.bg_points = pts
            elif k == ContourAnalyzer.BORDER_COLORS[0]:
                copyright_record.contour_points = pts
            else:
                pass

        records.append(equal_record)
        records.append(copyright_record)

        self.image = ImagePreprocessor.crop(self.image, 0, 0, 0, 1)

        return records

    def build_image(self, records: list[ContourRecord], width, height):

        palette = Palette()

        # palette for demo view
        palette.generate_demo(
            common := [
                hex_to_rgb(ContourAnalyzer.BORDER_COLORS[0]),   # contour
                hex_to_rgb(ContourAnalyzer.SHADOW_COLORS[0]),   # shadow
                hex_to_rgb(ContourAnalyzer.TEXT_COLORS[0]),     # text
                hex_to_rgb(ContourAnalyzer.BG_COLORS[0]),       # bg
                hex_to_rgb(ContourAnalyzer.BG_COLORS[0]),       # sign (hidden)
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
            offset=16,  # remain for text layer (4bpp) colors
        )

        img = IndexedImage(width, height)

        pal_idx = 16  # remain for text layer (4bpp) colors

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

    img = ImagePreprocessor.read_image(input_image)
    if img is None:
        print(f"Failed to load image {input_image}")
        sys.exit(1)

    exporter = HexCalculatorExporter(img)
    exporter.export(output_file)
