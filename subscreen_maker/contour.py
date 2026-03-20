"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""

import numpy as np
import cv2
from image import IndexedImage


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

def rgb_to_16bit(r, g, b):
    """
    Convert 24-bit RGB color to 16-bit (NDS) format.
    """
    r5 = (r * 31) // 255
    g5 = (g * 31) // 255
    b5 = (b * 31) // 255
    return (r5) | (g5 << 5) | (b5 << 10)

def contour_shape_key(contour, tolorence=0.1) -> bytes:
    """
    Generate a shape key for a contour, invariant to translation and point order.

        Args:
            contour: A list of (x, y) points representing the contour.
        Returns:
            A bytes object representing the shape key of the contour.
    """

    pts = np.asarray(contour).reshape(-1, 2).astype(np.float32)

    # move to centroid
    centroid = pts.mean(axis=0)
    pts -= centroid

    # quantization (noise reduction)
    pts = np.round(pts / tolorence) * tolorence

    # sort by polar angle
    pts = pts[np.lexsort((pts[:, 1], pts[:, 0]))]

    return pts.tobytes()


class ContourAnalyzer:
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
