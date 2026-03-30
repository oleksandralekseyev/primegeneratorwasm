# Prime Generator WASM

Small C++20/WebAssembly prime number demo with a deliberately tiny C++ API:

- `PrimeGenerator`
- `Progress { Percent }`
- `Result { PrimeCount, LastPrime }`

## Build

Native smoke build:

```sh
cmake -S . -B build-native -DCMAKE_BUILD_TYPE=Release
cmake --build build-native
ctest --test-dir build-native --output-on-failure
```

WASM build:

```sh
brew install emscripten
emcmake cmake -S . -B build-wasm -DCMAKE_BUILD_TYPE=Release
cmake --build build-wasm
python3 -m http.server 8000 --directory build-wasm/dist
```

Hosted demo from `main`:

- `https://oleksandralekseyev.github.io/primegeneratorwasm/`
