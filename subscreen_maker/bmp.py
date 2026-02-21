"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""
import struct
import numpy as np


class Palette:
    def __init__(self):
        self.colors = []

    def add_color(self, r, g, b):
        if len(self.colors) >= 256:
            raise ValueError("Palette full (max 256 colors)")
        self.colors.append((r, g, b))
        return len(self.colors) - 1

    def generate_rainbow(self):
        # index 0: reserved for transparent
        self.colors = [(0xFF, 0x00, 0xFF)]
        for i in range(255):
            r = (i * 37) % 256
            g = (i * 73) % 256
            b = (i * 109) % 256
            self.add_color(r, g, b)

    def generate_demo(self, color_array, count):
        # index 0 reserved for transparent
        self.colors = [(0, 0, 0)]
        for _ in range(count):
            for color in color_array:
                self.add_color(*color)

    def get(self):
        return self.colors


class IndexedImage:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.data = np.zeros((height, width), dtype=np.uint8)

    def set_pixel(self, x, y, palette_index):
        self.data[y, x] = palette_index

    def fill_points(self, points, palette_index):
        for x, y in points:
            self.set_pixel(x, y, palette_index)

    def get_array(self):
        return self.data


class Bmp8Writer:

    @staticmethod
    def save(filename, index_img: np.ndarray, palette):
        h, w = index_img.shape

        row_padded = (w + 3) & ~3
        padding = row_padded - w

        pixel_data = bytearray()

        # BMP bottom-up
        for y in range(h - 1, -1, -1):
            row = index_img[y].tobytes()
            pixel_data.extend(row)
            pixel_data.extend(b'\x00' * padding)

        # palette (BGRA)
        palette_bytes = bytearray()
        for r, g, b in palette:
            palette_bytes.extend(struct.pack('<BBBB', b, g, r, 0))

        palette_size = len(palette_bytes)

        file_size = 14 + 40 + palette_size + len(pixel_data)

        bmp_header = struct.pack(
            '<2sIHHI',
            b'BM',
            file_size,
            0,
            0,
            14 + 40 + palette_size
        )

        dib_header = struct.pack(
            '<IIIHHIIIIII',
            40,
            w,
            h,
            1,
            8,
            0,  # BI_RGB
            len(pixel_data),
            2835, 2835,
            len(palette),
            len(palette)
        )

        with open(filename, 'wb') as f:
            f.write(bmp_header)
            f.write(dib_header)
            f.write(palette_bytes)
            f.write(pixel_data)
