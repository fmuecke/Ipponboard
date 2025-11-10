# Agent Commit Checklist

Always update this file *before* running any commit command. Reset the status to `PENDING` immediately after the commit is finished.

- Current approval status: PENDING
- Last approval message: (copy the user's exact approval words here)
- Approval timestamp (UTC): (record after approval)

Steps before committing:
1. Request explicit commit approval from the user.
2. When approval is granted, edit this file: set status to `APPROVED`, capture the user's message and the current UTC timestamp.
3. Use `scripts/agent_git_commit.sh` to perform the commit.
4. After the commit, reset status back to `PENDING` to avoid accidental reuse.

If approval expires or is revoked, immediately reset the status to `PENDING`.
