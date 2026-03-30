import path from "node:path";
import { pathToFileURL } from "node:url";

const distDir = process.argv[2]
  ? path.resolve(process.argv[2])
  : path.resolve("build-wasm/dist");

const modulePath = path.join(distDir, "prime_generator.js");
const { default: createPrimeGeneratorModule } = await import(pathToFileURL(modulePath).href);

const module = await createPrimeGeneratorModule({
  locateFile(file) {
    return path.join(distDir, file);
  },
});

const generator = new module.PrimeGenerator();
const updates = [];

const result = await generator.GeneratePrimesBelow(100, (progress) => {
  updates.push(Number(progress.Percent));
});

if (result.PrimeCount !== 25 || result.LastPrime !== 97) {
  throw new Error(`Unexpected result: count=${result.PrimeCount}, last=${result.LastPrime}.`);
}

if (updates.length === 0 || updates.at(-1) < 100) {
  throw new Error(`Progress did not reach 100%: ${updates.join(", ")}.`);
}

console.log(`WASM smoke test passed: ${result.PrimeCount} primes below 100.`);
