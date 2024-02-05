
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

rm -rf $SCRIPT_DIR/../src/wasm
cp -r $SCRIPT_DIR/build.emscripten $SCRIPT_DIR/../src/wasm