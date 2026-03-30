#include "PrimeGenerator.h"

#include <emscripten/bind.h>
#include <emscripten.h>
#include <emscripten/val.h>

namespace prime {

class PrimeGeneratorEmscripten : public PrimeGenerator {
 public:
  Result GeneratePrimesBelow(std::uint32_t limit, const emscripten::val& on_progress) noexcept {
    on_progress_ = on_progress;
    const Result result = PrimeGenerator::GeneratePrimesBelow(limit);
    on_progress_ = emscripten::val::undefined();
    return result;
  }

 protected:
  void OnProgress(const Progress& progress) noexcept override {
    if (on_progress_.isNull() || on_progress_.isUndefined()) {
      return;
    }

    emscripten::val payload = emscripten::val::object();
    payload.set("Percent", progress.Percent);
    on_progress_(payload);
  }

  void YieldToHost() noexcept override {
    // Asyncify pauses here so the browser can paint and handle input.
    emscripten_sleep(0);
  }

 private:
  emscripten::val on_progress_ = emscripten::val::undefined();
};

}  // namespace prime

EMSCRIPTEN_BINDINGS(PrimeGeneratorModule) {
  using emscripten::class_;
  using emscripten::value_object;
  using prime::PrimeGeneratorEmscripten;
  using prime::Progress;
  using prime::Result;

  value_object<Progress>("Progress")
      .field("Percent", &Progress::Percent);

  value_object<Result>("Result")
      .field("PrimeCount", &Result::PrimeCount)
      .field("LastPrime", &Result::LastPrime);

  class_<PrimeGeneratorEmscripten>("PrimeGenerator")
      .constructor<>()
      .function("GeneratePrimesBelow", &PrimeGeneratorEmscripten::GeneratePrimesBelow);
}
