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
    def contour_bbox(contour):
        return cv2.boundingRect(contour)

    @staticmethod
    def contour_area(contour):
        return float(cv2.contourArea(contour))

    @staticmethod
    def filled_mask(contour, height, width):
        filled = np.zeros((height, width), np.uint8)
        cv2.drawContours(filled, [contour], -1, 255, -1)
        return filled

    @staticmethod
    def largest_contour_in_region(mask, y0, y1, x0, x1, min_area=200):
        region = np.zeros_like(mask)
        region[y0:y1, x0:x1] = mask[y0:y1, x0:x1]
        contours, _ = cv2.findContours(
            region, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        if not contours:
            return None
        contour = max(contours, key=cv2.contourArea)
        if cv2.contourArea(contour) < min_area:
            return None
        return contour

    @staticmethod
    def mask_points(mask):
        ys, xs = np.where(mask)
        return list(zip(xs.tolist(), ys.tolist()))

    @staticmethod
    def collect_points_in_bands(image, hex_colors, bands, x_limit):
        """Collect (x, y) for pixels matching hex_colors inside y-bands, x < x_limit."""
        mask = ContourAnalyzer.build_mask(image, hex_colors)
        h = mask.shape[0]
        pts = []
        for y0, y1 in bands:
            for y in range(max(0, y0), min(h, y1)):
                xs = np.where(mask[y, :x_limit] > 0)[0]
                pts.extend((int(x), int(y)) for x in xs)
        return pts

    @staticmethod
    def bbox_of_mask_strip(mask, y0, y1, x0, x1, pad=0):
        """Bounding box of nonzero mask pixels in [y0:y1, x0:x1], optionally padded."""
        h, w = mask.shape
        x0 = max(0, x0)
        x1 = min(w, x1)
        y0 = max(0, y0)
        y1 = min(h, y1)
        if x1 <= x0 or y1 <= y0:
            return None
        strip = mask[y0:y1, x0:x1]
        ys, xs = np.where(strip > 0)
        if len(xs) == 0:
            return None
        ax = int(xs.min()) + x0
        ay = int(ys.min()) + y0
        aw = int(xs.max() - xs.min()) + 1
        ah = int(ys.max() - ys.min()) + 1
        ax = max(0, ax - pad)
        ay = max(0, ay - pad)
        aw = min(w - ax, aw + 2 * pad)
        ah = min(h - ay, ah + 2 * pad)
        return ax, ay, aw, ah

    @staticmethod
    def pixel_hex(image, x, y):
        b, g, r = image[y, x]
        return f"{r:02x}{g:02x}{b:02x}"

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

    @staticmethod
    def classify_pill_pixels(image, contour, expand_pad, border_hex, shadow_hex,
                             text_hex, bg_hex):
        """
        Fill pill interior as bg (no border). Outside-pad ink → text;
        outside-pad shadow → shadow. Returns (bg, text, shadow) point lists.
        """
        h_img, w_img, _ = image.shape
        x, y, w, h = ContourAnalyzer.contour_bbox(contour)
        rx0 = max(0, x - expand_pad)
        ry0 = max(0, y - 2)
        rx1 = min(w_img, x + w + expand_pad)
        ry1 = min(h_img, y + h + 2)

        filled = ContourAnalyzer.filled_mask(contour, h_img, w_img)
        bg_points = []
        text_points = []
        shadow_points = []

        for py in range(ry0, ry1):
            for px in range(rx0, rx1):
                key = ContourAnalyzer.pixel_hex(image, px, py)
                if key in bg_hex:
                    continue
                if filled[py, px]:
                    if key in shadow_hex or key in text_hex or key in border_hex:
                        bg_points.append((px, py))
                elif key in text_hex or key in border_hex:
                    text_points.append((px, py))
                elif key in shadow_hex:
                    shadow_points.append((px, py))

        return (
            list(dict.fromkeys(bg_points)),
            list(dict.fromkeys(text_points)),
            list(dict.fromkeys(shadow_points)),
        )


class ContourRecord:
    def __init__(self, contour):
        self.contour = contour
        self.shape_key = contour_shape_key(contour)

        self.contour_points = [(x, y) for [[x, y]] in contour]
        self.text_points = []
        self.shadow_points = []
        self.bg_points = []
        self.sign_points = []
        # If set, draw() uses this palette index for bg instead of map["bg"]
        self.bg_palette_index = None

    def draw(self, img: IndexedImage, palette_indices: dict):
        img.fill_points(self.shadow_points, palette_indices["shadow"])
        img.fill_points(self.contour_points, palette_indices["contour"])
        img.fill_points(self.text_points, palette_indices["text"])
        bg_idx = self.bg_palette_index
        if bg_idx is None:
            bg_idx = palette_indices["bg"]
        img.fill_points(self.bg_points, bg_idx)
        img.fill_points(self.sign_points, palette_indices["sign"])

    def position(self):
        x, y, w, h = cv2.boundingRect(self.contour)
        return x, y, w, h
