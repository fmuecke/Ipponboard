# Agent Commit Checklist

## Current session

- Current approval status: APPROVED
- Last approval message: "comitte" (explicit commit approval for the clang-format-19 migration and CI modernization change set)
- Approval timestamp: 2026-10-18 13:09:34 CEST
- Approver: repository owner (ralf), via chat instruction "comitte"

## Changes in this commit

- CI modernization in .github/workflows/linux-build.yml: actions/checkout@v5,
  jurplel/install-qt-action@v4.4.1 (Node.js 24), runs-on ubuntu-latest,
  clang-format-19 instead of clang-format-14, removal of the obsolete
  MarkusJx/install-boost step and all BOOST_DIR/LINUX_BOOST_DIR wiring
- Boost removal in .github/workflows/msvc.yml and .github/workflows/windows-build.yml
  (install step + BOOST_DIR env wiring); choco llvm pin raised to 19.1.7
- scripts/check-format.sh / scripts/check-format.ps1: required clang-format
  version floor raised from 14 to 19
- 9 source files reformatted with clang-format-19 (one-line function bodies,
  include spacing, initializer packing); full validation: 0 formatting
  violations across all 131 tracked C/C++ files
- Validation: IpponboardTest rebuilt (60/60 targets) and run headless —
  all tests passed (493 assertions in 101 test cases)

## Notes

- msvc.yml (actions/checkout@v3, install-qt-action@v3, codeql-action@v2) and
  windows-build.yml (checkout@v4, install-qt-action@v3) still use older action
  versions; intentionally out of scope of this commit, follow-up pending.
