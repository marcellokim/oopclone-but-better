# Cleanup Plan — README + Release Hygiene

## Scope
- `README.md` (new)
- `.gitignore`
- generated local OMX directories only if safe to remove from working tree (`.omx/team/`, `.omx/drafts/`)

## Behavior Lock
- No gameplay/runtime code changes.
- Re-run build + tests after edits to confirm no accidental regressions.

## Smells to Remove
1. Missing repository entrypoint documentation
2. Missing release-oriented ignore rules for generated team/draft artifacts
3. Leftover generated local OMX directories that do not belong in a release-facing repo state

## Ordered Passes
1. Add `README.md` with overview, features, controls, build/run/test instructions, and repo layout
2. Tighten `.gitignore` for generated OMX artifacts
3. Remove safe generated local directories if present
4. Re-run verification commands
