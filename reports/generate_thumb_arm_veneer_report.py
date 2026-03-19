#!/usr/bin/env python3
"""Generate Thumb->ARM veneer report for a built ELF.

This script scans *_from_thumb veneer functions, resolves their ARM targets,
finds all call sites, maps them back to source locations, and emits a Markdown
report.
"""

from __future__ import annotations

import argparse
import datetime as _dt
import os
import re
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Tuple


@dataclass(frozen=True)
class CallSite:
    address: int
    caller_function: str
    location: str
    source_type: str  # "workspace" or "external"


def run_checked(cmd: List[str]) -> str:
    try:
        return subprocess.check_output(cmd, text=True)
    except subprocess.CalledProcessError as exc:
        print(f"error: command failed: {' '.join(cmd)}", file=sys.stderr)
        if exc.output:
            print(exc.output, file=sys.stderr)
        raise


def pick_tool(preferred: str, fallback: str) -> str:
    if Path(preferred).exists():
        return preferred
    return fallback


def resolve_tools() -> Dict[str, str]:
    return {
        "objdump": pick_tool("/opt/devkitpro/devkitARM/bin/arm-none-eabi-objdump", "objdump"),
        "nm": pick_tool("/opt/devkitpro/devkitARM/bin/arm-none-eabi-nm", "nm"),
        "addr2line": pick_tool("/opt/devkitpro/devkitARM/bin/arm-none-eabi-addr2line", "addr2line"),
    }


def parse_veneer_names(nm_output: str) -> List[str]:
    names: List[str] = []
    for line in nm_output.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[2].endswith("_from_thumb"):
            names.append(parts[2])
    names.sort()
    return names


def parse_disassembly_functions(disasm: str) -> List[Tuple[int, str, List[str]]]:
    functions: List[Tuple[int, str, List[str]]] = []
    current = None
    for line in disasm.splitlines():
        m = re.match(r"^([0-9a-f]+) <([^>]+)>:$", line)
        if m:
            current = (int(m.group(1), 16), m.group(2), [])
            functions.append(current)
            continue
        if current and re.match(r"^\s+[0-9a-f]+:\s+[0-9a-f]+\s+", line):
            current[2].append(line)
    return functions


def resolve_veneer_targets(functions: List[Tuple[int, str, List[str]]], veneer_names: Iterable[str]) -> Dict[str, str]:
    veneer_set = set(veneer_names)
    targets: Dict[str, str] = {}
    for _, name, body in functions:
        if name not in veneer_set:
            continue
        target = "UNKNOWN"
        for insn in body[:8]:
            m = re.search(r"\b(?:b|bl)\s+[0-9a-f]+ <([^>]+)>", insn)
            if m:
                callee = m.group(1)
                if callee != name:
                    target = callee
                    break
        targets[name] = target
    return targets


def parse_callsites(disasm: str, veneer_names: Iterable[str]) -> Dict[str, List[int]]:
    veneer_set = set(veneer_names)
    out: Dict[str, List[int]] = defaultdict(list)
    for line in disasm.splitlines():
        m = re.match(r"^\s*([0-9a-f]+):\s+[0-9a-f ]+\s+bl\s+[0-9a-f]+ <([^>]+)>", line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        callee = m.group(2)
        if callee in veneer_set:
            out[callee].append(addr)
    return out


def resolve_callsite(addr2line: str, elf: Path, workspace_root: Path, address: int) -> CallSite:
    out = run_checked([addr2line, "-e", str(elf), "-f", "-C", hex(address)]).splitlines()
    func = out[0].strip() if out else "??"
    loc = out[1].strip() if len(out) > 1 else "??:?"
    if " (discriminator " in loc:
        loc = loc.split(" (discriminator ", 1)[0]

    wr = str(workspace_root.resolve())
    if loc.startswith(wr + "/"):
        return CallSite(address=address, caller_function=func, location=loc[len(wr) + 1 :], source_type="workspace")
    return CallSite(address=address, caller_function=func, location=loc, source_type="external")


def render_report(
    elf_rel: str,
    veneer_names: List[str],
    veneer_targets: Dict[str, str],
    details: Dict[str, List[CallSite]],
) -> str:
    lines: List[str] = []
    now = _dt.datetime.now(_dt.timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")

    lines.append("# Thumb to ARM Veneer Report")
    lines.append("")
    lines.append(f"- ELF: `{elf_rel}`")
    lines.append(f"- Veneer count: `{len(veneer_names)}`")
    lines.append(f"- Generated on: `{now}`")
    lines.append("")
    lines.append("## Summary")
    lines.append("")
    lines.append("| Veneer | Target ARM function | Total calls | Workspace call sites | External/unknown call sites |")
    lines.append("|---|---|---:|---:|---:|")

    for veneer in veneer_names:
        all_calls = details.get(veneer, [])
        ws_count = sum(1 for c in all_calls if c.source_type == "workspace")
        ext_count = len(all_calls) - ws_count
        lines.append(
            f"| `{veneer}` | `{veneer_targets.get(veneer, 'UNKNOWN')}` | {len(all_calls)} | {ws_count} | {ext_count} |"
        )

    for veneer in veneer_names:
        calls = details.get(veneer, [])
        ws_calls = [c for c in calls if c.source_type == "workspace"]
        ext_calls = [c for c in calls if c.source_type != "workspace"]

        lines.append("")
        lines.append(f"## `{veneer}`")
        lines.append("")
        lines.append(f"- Target ARM function: `{veneer_targets.get(veneer, 'UNKNOWN')}`")
        lines.append(f"- Total `bl` call sites: `{len(calls)}`")
        lines.append(f"- Workspace call sites: `{len(ws_calls)}`")
        lines.append(f"- External or unresolved call sites: `{len(ext_calls)}`")

        if ws_calls:
            grouped: Dict[Tuple[str, str], List[int]] = defaultdict(list)
            for c in ws_calls:
                grouped[(c.caller_function, c.location)].append(c.address)

            lines.append("")
            lines.append("### Workspace call sites")
            lines.append("")
            lines.append("| Source location | Caller function | Calls | Call instruction addresses |")
            lines.append("|---|---|---:|---|")
            for (func, loc), addrs in sorted(grouped.items(), key=lambda i: (i[0][1], i[0][0])):
                addr_text = ", ".join(f"`0x{a:08x}`" for a in addrs)
                lines.append(f"| `{loc}` | `{func}` | {len(addrs)} | {addr_text} |")

        if ext_calls:
            grouped_ext: Dict[Tuple[str, str], List[int]] = defaultdict(list)
            for c in ext_calls:
                grouped_ext[(c.caller_function, c.location)].append(c.address)

            lines.append("")
            lines.append("### External or unresolved call sites")
            lines.append("")
            lines.append("| Location | Caller function | Calls | Call instruction addresses |")
            lines.append("|---|---|---:|---|")
            for (func, loc), addrs in sorted(grouped_ext.items(), key=lambda i: (i[0][1], i[0][0])):
                addr_text = ", ".join(f"`0x{a:08x}`" for a in addrs)
                lines.append(f"| `{loc}` | `{func}` | {len(addrs)} | {addr_text} |")

        if not calls:
            lines.append("")
            lines.append("No call sites found in the current ELF.")

    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate Thumb->ARM veneer report from an ELF")
    parser.add_argument("--elf", default="build-relwithdebinfo/HexCalculatorDS.elf", help="Path to input ELF")
    parser.add_argument(
        "--output",
        default="reports/thumb-arm-veneer-report.md",
        help="Path to output Markdown report",
    )
    parser.add_argument(
        "--workspace",
        default=".",
        help="Workspace root used to classify call sites as local/external",
    )
    args = parser.parse_args()

    elf = Path(args.elf)
    if not elf.exists():
        print(f"error: ELF not found: {elf}", file=sys.stderr)
        return 2

    workspace_root = Path(args.workspace).resolve()
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    tools = resolve_tools()

    nm_out = run_checked([tools["nm"], "-n", str(elf)])
    veneer_names = parse_veneer_names(nm_out)

    disasm = run_checked([tools["objdump"], "-d", str(elf)])
    functions = parse_disassembly_functions(disasm)
    veneer_targets = resolve_veneer_targets(functions, veneer_names)
    raw_callsites = parse_callsites(disasm, veneer_names)

    details: Dict[str, List[CallSite]] = {}
    cache: Dict[int, CallSite] = {}
    for veneer in veneer_names:
        calls: List[CallSite] = []
        for addr in raw_callsites.get(veneer, []):
            if addr not in cache:
                cache[addr] = resolve_callsite(tools["addr2line"], elf, workspace_root, addr)
            calls.append(cache[addr])
        details[veneer] = calls

    try:
        elf_rel = os.path.relpath(str(elf.resolve()), str(workspace_root))
    except ValueError:
        elf_rel = str(elf)

    report = render_report(elf_rel, veneer_names, veneer_targets, details)
    output.write_text(report, encoding="ascii")

    print(f"Generated report: {output}")
    print(f"Veneers: {len(veneer_names)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
