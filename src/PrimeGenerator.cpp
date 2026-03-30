#include "PrimeGenerator.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace prime {
namespace {

// Yield every N iterations so long runs do not monopolize the host event loop.
constexpr std::size_t kYieldStride = 2048u;

}  // namespace

void PrimeGenerator::OnProgress(const Progress&) noexcept {}

void PrimeGenerator::YieldToHost() noexcept {}

void PrimeGenerator::ReportProgress(double percent) noexcept {
  progress_.Percent = std::clamp(percent, 0.0, 100.0);
  OnProgress(progress_);
}

Result PrimeGenerator::GeneratePrimesBelow(std::uint32_t limit) noexcept {
  // Asyncify does not support overlapping executions on the same logical call stack.
  if (running_) {
    return result_;
  }

  running_ = true;
  progress_ = Progress{};
  result_ = Result{};

  const auto finish = [&](double percent) noexcept {
    ReportProgress(percent);
    running_ = false;
    return result_;
  };

  ReportProgress(0.0);
  YieldToHost();

  if (limit <= 2u) {
    return finish(100.0);
  }

  result_.PrimeCount = 1u;
  result_.LastPrime = 2u;

  if (limit <= 3u) {
    return finish(100.0);
  }

  // Store only odd candidates. Index i corresponds to the odd value 3 + 2*i.
  const std::size_t odd_count = static_cast<std::size_t>((limit - 2u) / 2u);
  std::vector<std::uint8_t> composite(odd_count, 0u);

  const std::uint32_t sqrt_limit = static_cast<std::uint32_t>(std::sqrt(static_cast<double>(limit - 1u)));
  const std::size_t sieve_count = sqrt_limit < 3u
      ? 0u
      : static_cast<std::size_t>(((sqrt_limit - 3u) / 2u) + 1u);
  const double total_work = static_cast<double>(sieve_count + odd_count);

  // First phase: mark composite odd numbers using base primes up to sqrt(limit).
  for (std::size_t index = 0; index < sieve_count; ++index) {
    if (composite[index] != 0u) {
      continue;
    }

    // Index i represents the odd value 3 + 2*i.
    const std::uint32_t prime = 3u + static_cast<std::uint32_t>(index * 2u);
    const std::size_t first = static_cast<std::size_t>(((prime * prime) - 3u) / 2u);

    for (std::size_t composite_index = first; composite_index < odd_count; composite_index += prime) {
      composite[composite_index] = 1u;
    }

    if (((index + 1u) % kYieldStride) == 0u || index + 1u == sieve_count) {
      ReportProgress(100.0 * static_cast<double>(index + 1u) / total_work);
      YieldToHost();
    }
  }

  // Second phase: count surviving primes and remember the last one below the limit.
  for (std::size_t index = 0; index < odd_count; ++index) {
    if (composite[index] == 0u) {
      result_.PrimeCount += 1u;
      result_.LastPrime = 3u + static_cast<std::uint32_t>(index * 2u);
    }

    if (((index + 1u) % kYieldStride) == 0u || index + 1u == odd_count) {
      ReportProgress(100.0 * static_cast<double>(sieve_count + index + 1u) / total_work);
      YieldToHost();
    }
  }

  return finish(100.0);
}

}  // namespace prime
