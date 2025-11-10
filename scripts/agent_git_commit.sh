#!/usr/bin/env bash
set -euo pipefail

CHECKLIST="$(git rev-parse --show-toplevel)/docs/agent_commit_checklist.md"
if [ ! -f "$CHECKLIST" ]; then
  echo "[agent_git_commit] Checklist not found at $CHECKLIST" >&2
  exit 1
fi

status_line=$(grep -m1 'Current approval status:' "$CHECKLIST" || true)
if [[ -z "$status_line" ]]; then
  echo "[agent_git_commit] Checklist missing status line" >&2
  exit 1
fi

if [[ "$status_line" != *"APPROVED"* ]]; then
  echo "[agent_git_commit] Approval status is not APPROVED. Update $CHECKLIST before committing." >&2
  exit 1
fi

message_line=$(grep -m1 'Last approval message:' "$CHECKLIST" || true)
if [[ -z "$message_line" || "$message_line" == *"(copy"* ]]; then
  echo "[agent_git_commit] Please record the approval message in $CHECKLIST before committing." >&2
  exit 1
fi

ts_line=$(grep -m1 'Approval timestamp' "$CHECKLIST" || true)
if [[ -z "$ts_line" || "$ts_line" == *"(record"* ]]; then
  echo "[agent_git_commit] Please record the approval timestamp in $CHECKLIST before committing." >&2
  exit 1
fi

exec git commit "$@"
