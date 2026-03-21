#!/usr/bin/env bash
set -euo pipefail

# Regenerate the Thumb->ARM veneer report from an existing ELF build.
# Usage:
#   reports/generate_thumb_arm_veneer_report.sh
#   reports/generate_thumb_arm_veneer_report.sh path/to/input.elf path/to/output.md

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PY_SCRIPT="${ROOT_DIR}/reports/generate_thumb_arm_veneer_report.py"

ELF_PATH="${1:-${ROOT_DIR}/build-relwithdebinfo/HexCalculatorDS.elf}"
OUT_PATH="${2:-${ROOT_DIR}/reports/thumb-arm-veneer-report.md}"

if [[ ! -f "${PY_SCRIPT}" ]]; then
    echo "error: generator not found: ${PY_SCRIPT}" >&2
    exit 2
fi

cmake -S ${ROOT_DIR} -B ${ROOT_DIR}/build-relwithdebinfo \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/cmake/devkitarm-toolchain.cmake
cmake --build ${ROOT_DIR}/build-relwithdebinfo -j $(nproc)

python3 "${PY_SCRIPT}" \
    --workspace "${ROOT_DIR}" \
    --elf "${ELF_PATH}" \
    --output "${OUT_PATH}"

echo "Done: ${OUT_PATH}"
