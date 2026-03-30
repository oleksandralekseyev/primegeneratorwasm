import createPrimeGeneratorModule from "./prime_generator.js";

export class PrimeGeneratorClient {
  static async create() {
    const module = await createPrimeGeneratorModule();
    return new PrimeGeneratorClient(module);
  }

  constructor(module) {
    this.generator = new module.PrimeGenerator();
    this.running = false;
  }

  async generatePrimesBelow(limit, onProgress = () => {}) {
    if (this.running) {
      throw new Error("Generation already running.");
    }

    this.running = true;

    try {
      const result = await this.generator.GeneratePrimesBelow(this.#normalizeLimit(limit), (progress) => {
        onProgress({ percent: Number(progress.Percent) });
      });

      return {
        primeCount: Number(result.PrimeCount),
        lastPrime: Number(result.LastPrime),
      };
    } finally {
      this.running = false;
    }
  }

  #normalizeLimit(limit) {
    if (!Number.isFinite(limit) || limit <= 0) {
      return 0;
    }

    return Math.min(0xffffffff, Math.trunc(limit));
  }
}
