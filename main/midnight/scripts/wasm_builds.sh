# WebAssembly builds. Requires emcc on PATH (Axmol's bundled emsdk:
#   source $AX_ROOT/tools/external/emsdk/emsdk_env.sh)
# Usage: scripts/wasm_builds.sh [LOM|DDR|CITADEL] [version] [build_no]   (default LOM, proj.wasm/version.cmake)
#   e.g. scripts/wasm_builds.sh LOM 2.0.12 57
# Axmol ships no prebuilt wasm zlib; build Emscripten's port and copy it into place.
TME=${1:-LOM}
lower=$(echo $TME | tr 'A-Z' 'a-z')
XC="-DTME=$TME"
[ -n "$2" ] && XC="$XC,-DWASM_APP_VERSION=$2"
[ -n "$3" ] && XC="$XC,-DWASM_APP_BUILD_NO=$3"

embuilder build zlib
mkdir -p $AX_ROOT/3rdparty/zlib/_x/lib/wasm
cp "$(dirname $(which emcc))/cache/sysroot/lib/wasm32-emscripten/libz.a" $AX_ROOT/3rdparty/zlib/_x/lib/wasm/

mkdir -p Builds/wasm-$lower
# configure only (-c), then build the whole dir: axmol would otherwise build the .axproj target
# (midnight), which doesn't exist for DDR/CITADEL
axmol build -p wasm -c -xc "$XC,-BBuilds/wasm-$lower"
cmake --build Builds/wasm-$lower --parallel
# Serve with COOP/COEP headers (pthreads need SharedArrayBuffer), e.g. emrun or a custom server:
#   Builds/wasm-$lower/bin/<app>/<app>.html
