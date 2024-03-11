# npm run clean
# npm install

set -e
echo "Build C++ -> Wasm"
 
bash build_cpp.sh

echo "Build C++ -> Wasm"

npm run build:ts

echo "Build C++ -> Wasm"