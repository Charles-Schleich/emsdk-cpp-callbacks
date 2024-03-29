
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


docker build $SCRIPT_DIR -f $SCRIPT_DIR/Dockerfile -t cppwasm:build
docker run -v $SCRIPT_DIR:/src -it cppwasm:build "bash build-commands.sh"


rm -rf $SCRIPT_DIR/../src/wasm
cp -r $SCRIPT_DIR/build.emscripten $SCRIPT_DIR/../src/wasm