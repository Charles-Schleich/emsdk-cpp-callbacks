# CPP - WASM

CPP -> Wasm experiments via Emscripten, 
With Async, and callbacks from TS

-------------------------------
## How to build it

> :warning: **WARNING** :warning: : Everything is WIP, subject to very breaking changes. You've been warned.

1. Make sure that the following utilities are available on your platform. 
 - [Docker](https://www.docker.com/), Please check [here](https://docs.docker.com/engine/install/) to learn how to install it.
 - [NPM](https://www.npmjs.com/package/npm)
 - Typescript 

2. Clone the source with `git`:

   ```bash
   git clone https://github.com/Charles-Schleich/emsdk-cpp-callbacks
   ```

3. Build:

    While in active development, running the command

  ```bash
    npm clean_build_package
  ```

  Will run the build pipeline of 
  - Starting a Docker container containing the [Emscripten-SDK](https://emscripten.org/)
  - Building `cpp code` to target WASM, using CMake + the Emscripten toolchain
  - Copying the artifacts from the build container to the host system
  - The artifacts include:
    - `cpp-wasm.js`
    - `cpp-wasm.wasm`
    - `cpp-wasm.worker.js`
  - The compiling the typescript interface to Javascript, and exporting a types declaration file.
  - Webpack to package

> :warning: **WARNING** :warning: : Important Info regarding embind and Threads   
embind referes to JS objects which cannot be shared between threads.   
Basically, embind only works on a per-thread bases.  
If you want to proxy work to other threads it cannot involved embind object.  



## Running the Examples

  In order to run the examples, the user must:
  1. From the `./examples/web/` directory, start an instance of the web application.
```bash
npm install && npm run develop
```