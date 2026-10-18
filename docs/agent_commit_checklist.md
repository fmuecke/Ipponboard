# Agent Commit Checklist

## Current session

- Current approval status: APPROVED
- Last approval message: "committe" (explicit commit approval for the follow-up
  GitHub Actions modernization in msvc.yml and windows-build.yml)
- Approval timestamp: 2026-10-18 13:19:25 CEST
- Approver: repository owner (ralf), via chat instruction "committe"

## Changes in this commit

- .github/workflows/msvc.yml: actions/checkout@v3 -> v5,
  jurplel/install-qt-action@v3 -> v4.4.1 (Node.js 24, resolves the internal
  actions/setup-python@v5 deprecation), github/codeql-action/upload-sarif@v2
  -> v4 (v2 is discontinued; current releases are v3/v4),
  aqtversion ==3.1.* -> ==3.3.* (v4 action default)
- .github/workflows/windows-build.yml: actions/checkout@v4 -> v5,
  jurplel/install-qt-action@v3 -> v4.4.1, aqtversion ==3.1.* -> ==3.3.*
- .github/workflows/linux-build.yml: aqtversion ==3.1.* -> ==3.3.* for
  consistency with the other workflows (install-qt-action@v4.4.1 was already
  in place from the previous commit)
- Validation: all three workflow YAML files parse cleanly (yaml.safe_load)

## Notes

- Decision (Option C): the pin microsoft/msvc-code-analysis-action@04825f6
  (main-branch commit from 2021, repo frozen since v0.1.1 in Aug 2022, runs
  on node16) is intentionally left untouched; the workflow targets main only.
- windows-build.yml is disabled (if: ${{ false }}, self-hosted); its updates
  can only be validated once the job is re-enabled.
