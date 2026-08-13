#!/usr/bin/env python3
"""Execute a tracked Python script in the persistent TruongSinh UE Editor."""

from __future__ import annotations

import argparse
import importlib.util
import json
from pathlib import Path
import sys
import time


DEFAULT_REMOTE_EXECUTION = Path(
    r"C:\Program Files\Epic Games\UE_5.8\Engine\Plugins\Experimental"
    r"\PythonScriptPlugin\Content\Python\remote_execution.py"
)


def load_remote_execution(path: Path):
    spec = importlib.util.spec_from_file_location("ue_remote_execution", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load UE remote execution module: {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def choose_node(nodes: list[dict], project: str) -> dict:
    project_lower = project.lower()
    matching = []
    for node in nodes:
        searchable = json.dumps(node, ensure_ascii=False, sort_keys=True).lower()
        if project_lower in searchable:
            matching.append(node)
    if len(matching) == 1:
        return matching[0]
    raise RuntimeError(
        "Expected exactly one matching Unreal Editor node; discovered: "
        + json.dumps(nodes, ensure_ascii=False, sort_keys=True)
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--script", type=Path)
    mode.add_argument("--statement")
    parser.add_argument("--project", default="TruongSinhUE5")
    parser.add_argument("--timeout", type=float, default=15.0)
    parser.add_argument(
        "--remote-execution-module",
        type=Path,
        default=DEFAULT_REMOTE_EXECUTION,
    )
    args = parser.parse_args()

    if not args.remote_execution_module.is_file():
        raise FileNotFoundError(args.remote_execution_module)

    ue_remote = load_remote_execution(args.remote_execution_module)
    session = ue_remote.RemoteExecution()
    try:
        session.start()
        deadline = time.monotonic() + args.timeout
        nodes: list[dict] = []
        while time.monotonic() < deadline:
            nodes = session.remote_nodes
            if nodes:
                break
            time.sleep(0.2)
        if not nodes:
            raise TimeoutError(
                "No Unreal Python node found. Keep the project Editor open and "
                "enable Python Remote Execution in local editor settings."
            )

        node = choose_node(nodes, args.project)
        session.open_command_connection(node["node_id"])

        if args.script is not None:
            script = args.script.resolve()
            if not script.is_file():
                raise FileNotFoundError(script)
            command = str(script)
            exec_mode = ue_remote.MODE_EXEC_FILE
        else:
            command = args.statement
            exec_mode = ue_remote.MODE_EXEC_STATEMENT

        result = session.run_command(
            command,
            unattended=True,
            exec_mode=exec_mode,
            raise_on_failure=True,
        )
        print(
            json.dumps(
                {"node": node, "command_result": result},
                ensure_ascii=False,
                indent=2,
                sort_keys=True,
            )
        )
        return 0
    finally:
        session.stop()


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
