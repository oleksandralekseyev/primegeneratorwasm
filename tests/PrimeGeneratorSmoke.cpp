#include "PrimeGenerator.h"

#include <iostream>

int main() {
  prime::PrimeGenerator generator;
  const prime::Result result = generator.GeneratePrimesBelow(100u);

  if (result.PrimeCount != 25u) {
    std::cerr << "Expected 25 primes below 100, got " << result.PrimeCount << ".\n";
    return 1;
  }

  if (result.LastPrime != 97u) {
    std::cerr << "Expected last prime below 100 to be 97, got " << result.LastPrime << ".\n";
    return 1;
  }

  std::cout << "Smoke test passed: " << result.PrimeCount << " primes below 100.\n";
  return 0;
}
