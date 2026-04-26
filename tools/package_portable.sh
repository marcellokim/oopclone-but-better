#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${TERRITORY_WAR_PACKAGE_BUILD_DIR:-$ROOT_DIR/build-package}"
DIST_DIR="$ROOT_DIR/dist"
CONFIG="${TERRITORY_WAR_CONFIG:-Release}"
RUN_TESTS=0

usage() {
  cat <<'USAGE'
Usage: tools/package_portable.sh [--test] [--debug]

Builds a ready-to-share ZIP. Players unzip it and double-click Play.command.

Options:
  --test    Also build and run the test suite before packaging.
  --debug   Package a Debug build instead of Release.
  --help    Show this help.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --test)
      RUN_TESTS=1
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
    exit 1
  fi
}

platform_name() {
  case "$(uname -s)" in
    Darwin) printf 'macOS' ;;
    Linux) printf 'Linux' ;;
    *) uname -s | tr '[:upper:]' '[:lower:]' ;;
  esac
}

arch_name() {
  case "$(uname -m)" in
    arm64|aarch64) printf 'arm64' ;;
    x86_64|amd64) printf 'x64' ;;
    *) uname -m ;;
  esac
}

write_launcher() {
  local launcher="$1"
  cat > "$launcher" <<'LAUNCHER'
#!/usr/bin/env bash
set -euo pipefail

APP_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$APP_DIR"

if [[ -x "$APP_DIR/bin/territory_war" ]]; then
  exec "$APP_DIR/bin/territory_war"
fi

echo "territory_war executable was not found next to this launcher." >&2
exit 1
LAUNCHER
  chmod +x "$launcher"
}

create_zip() {
  local stage_dir="$1"
  local archive="$2"
  local parent
  local base
  parent="$(dirname "$stage_dir")"
  base="$(basename "$stage_dir")"

  require_command zip
  rm -f "$archive"
  (cd "$parent" && zip -qr -X "$archive" "$base")
}

require_command cmake

PLATFORM="$(platform_name)"
ARCH="$(arch_name)"
PACKAGE_BASENAME="TerritoryWar-$PLATFORM-$ARCH"
STAGE_DIR="$DIST_DIR/$PACKAGE_BASENAME"
ARCHIVE_PATH="$DIST_DIR/$PACKAGE_BASENAME.zip"

echo "Project: $ROOT_DIR"
echo "Build:   $BUILD_DIR"
echo "Package: $ARCHIVE_PATH"
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

rm -rf "$STAGE_DIR"
mkdir -p "$DIST_DIR"

cmake --install "$BUILD_DIR" --config "$CONFIG" --prefix "$STAGE_DIR"
write_launcher "$STAGE_DIR/Play.command"
create_zip "$STAGE_DIR" "$ARCHIVE_PATH"

echo "Ready:"
echo "  Folder: $STAGE_DIR"
echo "  ZIP:    $ARCHIVE_PATH"
