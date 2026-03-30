import { PrimeGeneratorClient } from "./PrimeGeneratorClient.js";

const elements = {
  form: document.querySelector("[data-form]"),
  limit: document.querySelector("[data-limit]"),
  run: document.querySelector("[data-run]"),
  status: document.querySelector("[data-status]"),
  progressBar: document.querySelector("[data-progress-bar]"),
  progressText: document.querySelector("[data-progress-text]"),
  primeCount: document.querySelector("[data-prime-count]"),
  lastPrime: document.querySelector("[data-last-prime]"),
};

function setBusy(isBusy) {
  elements.run.disabled = isBusy;
}

function renderProgress(percent) {
  elements.progressBar.value = percent;
  elements.progressText.textContent = `${percent.toFixed(1)}%`;
}

function renderResult(result) {
  elements.primeCount.textContent = result.primeCount.toLocaleString();
  elements.lastPrime.textContent = result.lastPrime.toLocaleString();
}

async function main() {
  const generator = await PrimeGeneratorClient.create();

  elements.status.textContent = "ready";
  renderProgress(0);
  renderResult({ primeCount: 0, lastPrime: 0 });

  elements.form.addEventListener("submit", async (event) => {
    event.preventDefault();

    setBusy(true);
    elements.status.textContent = "running";
    renderProgress(0);

    try {
      const result = await generator.generatePrimesBelow(Number(elements.limit.value), (progress) => {
        renderProgress(progress.percent);
      });

      renderResult(result);
      elements.status.textContent = "done";
    } catch (error) {
      console.error(error);
      elements.status.textContent = "failed";
      elements.progressText.textContent = error instanceof Error ? error.message : String(error);
    } finally {
      setBusy(false);
    }
  });
}

main().catch((error) => {
  console.error(error);
  elements.status.textContent = "failed";
  elements.progressText.textContent = error instanceof Error ? error.message : String(error);
  setBusy(false);
});
