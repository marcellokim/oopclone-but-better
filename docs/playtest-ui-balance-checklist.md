# UI Balance Playtest Checklist

Use this checklist after the phase-2 UI/productization work lands.
Goal: verify that readability, hierarchy, and command clarity improved during real play without harming speed or confidence.

## Test Session Metadata
- Date:
- Tester:
- Build / commit:
- Integrated worker commits / cherry-picks:
- Resolution / window size:
- Font path used:
- Fallback path exercised (yes/no):
- Match length:
- Nation played:
- Automation evidence doc:

## Rating Scale
- **5** — effortless / immediately clear
- **4** — clear with minor friction
- **3** — usable but distracting or slow
- **2** — confusing / slows decisions
- **1** — major usability problem

Record both a numeric score and a short note for each section.

## Preflight Regression Gate
Capture the automated and smoke checks that justify running the playtest on this build.

- `cmake -S . -B build`:
- `cmake --build build -j4 --target territory_war territory_war_tests`:
- `ctest --test-dir build --output-on-failure`:
- Runtime smoke (`./build/territory_war` or equivalent bounded launch):
- Geometry / hit-testing spot check completed:
- Bundled-font path verified:
- Fallback-font path verified:

---

## 1. First-Read Clarity (Opening 30 Seconds)
**Scenario:** Start a fresh match and do not issue any commands for the first 10 seconds. Just read the screen.

### Check
- Can I instantly identify **my nation**?
- Can I instantly identify **my capital**?
- Can I instantly find the **command rail / operational HUD**?
- Can I tell what the current **send ratio** is without searching?
- Does the map still feel like the primary object on screen?

### Score
- First-read clarity score (1-5):
- Notes:

### Tune if weak
- Increase player-nation emphasis in top strip / command panel
- Increase capital distinction vs normal owned tiles
- Reduce right-rail visual dominance if it competes with the map
- Raise send-ratio chip contrast or position prominence

### Applied delta(s) after this scenario
- [ ]

---

## 2. Hierarchy Under Intent Selection
**Scenario:** Select an owned tile, hover a second tile, then clear selection. Repeat 3 times on different fronts.

### Check
- Is **selected tile** always stronger than **hovered tile**?
- Is **capital emphasis** still recognizable while another tile is selected?
- Does the command-preview area reinforce what will happen next?
- Does any panel pull attention away from the active selected tile?

### Score
- Selection hierarchy score (1-5):
- Notes:

### Tune if weak
- Strengthen selected outline/glow before increasing hover effects
- Reduce hover intensity if it competes with selection
- Simplify command-preview copy if it delays recognition

### Applied delta(s) after this scenario
- [ ]

---

## 3. Readability During Front Congestion
**Scenario:** Let multiple fronts become active. Hover tiles while at least one transit marker is moving.

### Check
- Can I still read **troop counts** at a glance?
- Are **moving transits** readable without cluttering the battlefield?
- Can I distinguish **owned / enemy / neutral / capital** quickly?
- Do transit lines or glow effects ever hide troop capsules or tile ownership?

### Score
- Congestion readability score (1-5):
- Notes:

### Tune if weak
- Increase troop capsule contrast before enlarging text
- Reduce trail opacity before shrinking markers
- Raise capital marker separation from normal tile styling
- Reduce decorative layers on busy regions first

### Applied delta(s) after this scenario
- [ ]

---

## 4. Input Confidence Under Pressure
**Scenario:** Play an aggressive opening for 2 minutes using rapid select -> issue order -> hover -> reissue loops.

### Check
- Do I trust that clicks land on the intended tile/card?
- Does the UI make command state feel explicit rather than ambiguous?
- Do hover and selection effects support fast action instead of delaying it?
- Is there any mismatch between what looks clickable and what actually responds?

### Score
- Input confidence score (1-5):
- Notes:

### Tune if weak
- Re-check shared geometry source for draw vs hit-testing
- Increase active tile spacing or edge separation
- Reduce ornamental overlap near selectable surfaces

### Applied delta(s) after this scenario
- [ ]

---

## 5. Right-Rail / Info Density Balance
**Scenario:** Pause mentally every 30–45 seconds and read the right rail instead of the map.

### Check
- Is the right rail **helpful**, or does it feel like too much chrome?
- Are commander, hover intel, live fronts, and objective grouped in a useful order?
- Does any section feel redundant?
- Do I ignore any panel entirely for the whole match?

### Score
- Info-density score (1-5):
- Notes:

### Tune if weak
- Compress or merge ignored panels
- Move frequently used info upward
- Demote low-value copy to lighter styling
- Shorten labels before shrinking panels

### Applied delta(s) after this scenario
- [ ]

---

## 6. Nation Select Dossier Quality
**Scenario:** Return to nation select after a completed match and compare the 4 doctrine cards before choosing.

### Check
- Does each doctrine card feel distinct in identity, not just color?
- Are stats readable without visual overload?
- Is the currently hovered/selected doctrine clearly emphasized?
- Does the screen feel premium rather than like a simple launcher menu?

### Score
- Nation-select quality score (1-5):
- Notes:

### Tune if weak
- Strengthen doctrine summary contrast and spacing
- Increase asymmetry between cards only if click clarity stays intact
- Reduce card text density before shrinking font size

### Applied delta(s) after this scenario
- [ ]

---

## 7. Debrief / Game-Over Comprehension
**Scenario:** Finish a match or force a result, then spend 10 seconds reading the debrief screen.

### Check
- Can I tell **winner vs stalemate** immediately?
- Are standings easy to scan in rank-like order?
- Does the debrief panel feel distinct from the live HUD?
- Is the replay / return instruction obvious?

### Score
- Debrief comprehension score (1-5):
- Notes:

### Tune if weak
- Increase headline-to-subheadline contrast
- Separate standings rows more clearly
- Increase replay instruction contrast near the bottom of the panel

### Applied delta(s) after this scenario
- [ ]

---

## 8. Final Decision Summary
### Aggregate Scores
- First-read clarity:
- Selection hierarchy:
- Congestion readability:
- Input confidence:
- Info density:
- Nation select quality:
- Debrief comprehension:

### Go / Hold Decision
- **GO** — no category below 4
- **HOLD** — any category at 3 or below

### Required Fixes Before Sign-off
- [ ]
- [ ]
- [ ]

### Optional Polish Follow-ups
- [ ]
- [ ]
- [ ]

## Regression Confidence Summary
- Control semantics preserved:
- Draw geometry and hit-testing still aligned:
- Preferred bundled fonts render correctly:
- Graceful fallback path still renders correctly:
- Remaining concerns / follow-up verification:
