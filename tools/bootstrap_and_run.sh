#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${TERRITORY_WAR_BUILD_DIR:-$ROOT_DIR/build}"
CONFIG="${TERRITORY_WAR_CONFIG:-Release}"
RUN_APP=1
RUN_TESTS=0
INSTALL_APP=0

usage() {
  cat <<'USAGE'
Usage: tools/bootstrap_and_run.sh [--test] [--install] [--no-run] [--debug]

Builds the game with CMake and launches it.

Options:
  --test      Also build and run the test suite.
  --install   Install a local copy to dist/territory-war.
  --no-run    Build only; do not launch the game.
  --debug     Use Debug build type instead of Release.
  --help      Show this help.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --test)
      RUN_TESTS=1
      shift
      ;;
    --install)
      INSTALL_APP=1
      shift
      ;;
    --no-run)
      RUN_APP=0
      shift
      ;;
    --debug)
      CONFIG="Debug"
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

require_command() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "Missing required command: $1" >&2
    echo "Install CMake and a C++20 compiler, then run this script again." >&2
    exit 1
  fi
}

find_executable() {
  local candidate
  for candidate in \
    "$BUILD_DIR/territory_war" \
    "$BUILD_DIR/$CONFIG/territory_war" \
    "$BUILD_DIR/bin/territory_war" \
    "$BUILD_DIR/bin/$CONFIG/territory_war"; do
    if [[ -x "$candidate" ]]; then
      printf '%s\n' "$candidate"
      return 0
    fi
  done
  return 1
}

require_command cmake

echo "Project: $ROOT_DIR"
echo "Build:   $BUILD_DIR"
echo "Config:  $CONFIG"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$CONFIG"

targets=(territory_war)
if [[ "$RUN_TESTS" == "1" ]]; then
  targets+=(territory_war_tests)
fi

cmake --build "$BUILD_DIR" --config "$CONFIG" -j4 --target "${targets[@]}"

if [[ "$RUN_TESTS" == "1" ]]; then
  ctest --test-dir "$BUILD_DIR" --build-config "$CONFIG" --output-on-failure
fi

if [[ "$INSTALL_APP" == "1" ]]; then
  cmake --install "$BUILD_DIR" --config "$CONFIG" --prefix "$ROOT_DIR/dist/territory-war"
  echo "Installed local copy: $ROOT_DIR/dist/territory-war"
fi

if [[ "$RUN_APP" == "1" ]]; then
  executable="$(find_executable)" || {
    echo "Build completed, but territory_war executable was not found in $BUILD_DIR." >&2
    exit 1
  }
  echo "Launching: $executable"
  cd "$ROOT_DIR"
  exec "$executable"
fi
