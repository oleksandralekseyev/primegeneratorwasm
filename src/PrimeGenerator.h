#pragma once

#include <cstdint>

namespace prime {

struct Progress {
  // Reported as a plain percentage so the JS side stays trivial.
  double Percent = 0.0;
};

struct Result {
  // The smallest useful result payload: how many primes were found and the last one.
  std::uint32_t PrimeCount = 0;
  std::uint32_t LastPrime = 0;
};

class PrimeGenerator {
 public:
  virtual ~PrimeGenerator() = default;

  // Native entry point used by the smoke test and by derived wrappers.
  Result GeneratePrimesBelow(std::uint32_t limit) noexcept;

 protected:
  // Derived classes can forward progress somewhere else, for example to JS.
  virtual void OnProgress(const Progress& progress) noexcept;
  virtual void YieldToHost() noexcept;

 private:
  void ReportProgress(double percent) noexcept;

  Progress progress_;
  Result result_;
  bool running_ = false;
};

}  // namespace prime
