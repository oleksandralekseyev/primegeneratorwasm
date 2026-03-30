# Design

## Goal

Build a small WASM prime number service:

- implement the prime calculation in C++20
- avoid exceptions in project C++ code
- expose the C++ side through embind with PascalCase exports
- provide a camelCase JavaScript wrapper
- run the computation asynchronously on the C++ side so the UI stays responsive
- report progress and show it on the page
- avoid `SharedArrayBuffer`
- keep the core generator small and easy to review

## Class Split

The code is split into a plain C++ base class and a small WASM-specific derived class:

- `PrimeGenerator`: core algorithm and progress flow
- `PrimeGeneratorEmscripten`: JS callback bridge defined locally in the embind file

The embind surface is minimal:

- `PrimeGenerator`
- `Progress`
- `Result`
- `PrimeGenerator::GeneratePrimesBelow(limit, onProgress)`

The important detail is that the core files `PrimeGenerator.h` and `PrimeGenerator.cpp` do not include Emscripten headers and do not use `#ifdef __EMSCRIPTEN__`. The subclass exists only inside `PrimeGeneratorBindings.cpp`.

## Core Algorithm

The implementation uses a direct odd-only sieve.

- `2` is handled separately as the only even prime
- index `i` represents the odd number `3 + 2*i`
- one byte array stores only odd candidates below the limit

Calculation flow:

1. Allocate a byte array for odd values from `3` up to `limit - 1`.
2. Iterate candidate base primes up to `sqrt(limit)`.
3. For each unmarked base prime `p`, start at `p*p` and mark its odd multiples as composite.
4. Scan the array once more and count every unmarked odd value as prime.
5. Record the last unmarked value as `LastPrime`.

This avoids storing even numbers and keeps the implementation short enough for manual review.

## Async Strategy In WASM

Without `SharedArrayBuffer`, this project does not use pthreads or a true background WASM thread.

Instead, the C++ code yields cooperatively with `emscripten_sleep(0)`:

1. start in C++
2. report `0%`
3. yield to the browser event loop
4. process part of the sieve
5. report a new percent
6. yield again

That keeps the page responsive even though the computation is single-threaded.

The yield hook is a virtual method on `PrimeGenerator`. Native builds use the base no-op implementation, while `PrimeGeneratorEmscripten` overrides it with `emscripten_sleep(0)`.

## Progress Model

Progress is just:

- `Progress { Percent }`

The percent is based on combined work from both phases:

- marking composite values
- scanning the array to count primes and record the last one

## Result Model

The result is just:

- `Result { PrimeCount, LastPrime }`

This keeps the API small while returning two values that demonstrate the computation result.

## No-Exception Policy

Project code does not use `throw`/`catch`.

Build settings disable exception handling:

- native builds: `-fno-exceptions`
- Emscripten builds: `-fno-exceptions` and `-sDISABLE_EXCEPTION_CATCHING=1`

## GitHub-Hosted Test Page

The `main` branch is intended to publish the built demo to GitHub Pages at:

- `https://oleksandralekseyev.github.io/primegeneratorwasm/`

The CI workflow also uploads the exact built `dist` folder as a workflow artifact on every run.
