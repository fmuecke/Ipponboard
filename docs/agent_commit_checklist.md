# Agent Commit Checklist

## Current session

- Current approval status: APPROVED
- Last approval message: "commit now" (explicit commit approval for CMAKE_EXPORT_COMPILE_COMMANDS plus compile_commands.json symlink)
- Approval timestamp: 2026-11-08 UTC
- Approver: repository owner (ralf), via chat instruction "commit now"

## Changes in this commit

- CMakeLists.txt: set CMAKE_EXPORT_COMPILE_COMMANDS ON for Clang/clangd tooling
- CMakeLists.txt: create source-tree compile_commands.json symlink on UNIX out-of-source configures
- .gitignore: ignore generated compile_commands.json symlink
- Validation: fresh Ninja configure to /tmp/ipponboard-cc-check succeeded; compile_commands.json generated and symlink resolved; validation artifacts removed

## Notes

- Commit uses scripts/agent_git_commit.sh per project rule.
