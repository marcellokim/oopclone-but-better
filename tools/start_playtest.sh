#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

BUILD_DIR="${BUILD_DIR:-build}"
REPORT_DIR="${REPORT_DIR:-docs/playtest-runs}"
LAUNCH_APP=1
if [[ "${1:-}" == "--no-launch" ]]; then
  LAUNCH_APP=0
fi

mkdir -p "$REPORT_DIR"

TIMESTAMP_UTC="$(date -u +%Y%m%dT%H%M%SZ)"
DATE_LOCAL="$(date '+%Y-%m-%d %H:%M:%S %Z')"
GIT_SHA="$(git rev-parse --short HEAD)"
REPORT_PATH="$REPORT_DIR/playtest-$TIMESTAMP_UTC.md"

export PATH="/opt/homebrew/bin:$PATH"

cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -j4 --target territory_war territory_war_tests
CTEST_OUTPUT="$(ctest --test-dir "$BUILD_DIR" --output-on-failure 2>&1)"

cat > "$REPORT_PATH" <<REPORT
# Playtest Session — $TIMESTAMP_UTC

- Date: $DATE_LOCAL
- Build / commit: $GIT_SHA
- Resolution / window size: 1280x800 target
- Font path used: bundled redistributable fonts
- Automation evidence doc: docs/combined-ui-upgrades-phase2-verification.md

## Preflight Regression Gate
- cmake configure: PASS
- build: PASS
- ctest: PASS

\`\`\`
$CTEST_OUTPUT
\`\`\`

## Manual Playtest Notes
Use this report together with:
- docs/playtest-ui-balance-checklist.md

### First-read clarity
- Score:
- Notes:
- Applied delta(s):

### Selection hierarchy
- Score:
- Notes:
- Applied delta(s):

### Front congestion readability
- Score:
- Notes:
- Applied delta(s):

### Input confidence
- Score:
- Notes:
- Applied delta(s):

### Right-rail density
- Score:
- Notes:
- Applied delta(s):

### Nation-select quality
- Score:
- Notes:
- Applied delta(s):

### Debrief comprehension
- Score:
- Notes:
- Applied delta(s):

## Final Decision
- GO / HOLD:
- Required fixes:
- Optional follow-ups:
REPORT

echo "Playtest report created: $REPORT_PATH"
echo "Checklist: docs/playtest-ui-balance-checklist.md"

if [[ "$LAUNCH_APP" == "1" ]]; then
  echo "Launching game..."
  exec "$BUILD_DIR/territory_war"
fi
