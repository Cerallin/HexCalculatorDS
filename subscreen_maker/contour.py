"""
HexCalculatorDS - An integer calculator for Nintendo DS.
Copyright (C) 2026  Cerallin

SPDX-License-Identifier: GPL-2.0-or-later
"""

import numpy as np


def contour_shape_key(contour, tolorence = 0.1) -> bytes:
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
