const assert = require('assert');
const { execFileSync } = require('child_process');
const fs = require('fs');
const os = require('os');
const path = require('path');

const repoRoot = path.resolve(__dirname, '..', '..');
const fixtureDir = path.join(repoRoot, 'test-fixtures', 'equations');
const manifestPath = path.join(fixtureDir, 'expected-results.json');
const solverPath = path.resolve(
  repoRoot,
  process.env.SOLVER_PATH || 'solver/build/ocr-equation-solver'
);
const tessdataPath = process.env.TESSDATA_PATH || '/opt/homebrew/share/tessdata';
const tolerance = Number(process.env.SOLVER_ACCURACY_TOLERANCE || '0.01');

function runSolver(imagePath, outputDir) {
  const stdout = execFileSync(
    solverPath,
    [imagePath, tessdataPath, outputDir, '--json'],
    {
      encoding: 'utf8',
      maxBuffer: 1024 * 1024 * 10
    }
  );

  return JSON.parse(stdout);
}

function compareSolution(actual = {}, expected = {}) {
  for (const [variable, expectedValue] of Object.entries(expected)) {
    assert.ok(
      Object.prototype.hasOwnProperty.call(actual, variable),
      `missing variable ${variable}`
    );

    const delta = Math.abs(Number(actual[variable]) - expectedValue);
    assert.ok(
      delta <= tolerance,
      `${variable} expected ${expectedValue}, received ${actual[variable]}`
    );
  }
}

function main() {
  assert.ok(fs.existsSync(solverPath), `solver executable not found at ${solverPath}`);
  assert.ok(fs.existsSync(manifestPath), `fixture manifest not found at ${manifestPath}`);

  const cases = JSON.parse(fs.readFileSync(manifestPath, 'utf8'));
  const outputRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'solver-accuracy-'));
  const failures = [];

  for (const testCase of cases) {
    const imagePath = path.join(fixtureDir, testCase.image);
    const outputDir = path.join(outputRoot, path.parse(testCase.image).name);

    try {
      const result = runSolver(imagePath, outputDir);

      assert.strictEqual(result.best.status, testCase.expected.status);
      compareSolution(result.best.solution, testCase.expected.solution);

      console.log(
        `PASS ${testCase.image}: ${result.best.status} ` +
          `confidence=${Math.round(result.best.confidence * 1000) / 10}%`
      );
    } catch (error) {
      failures.push({ image: testCase.image, message: error.message });
      console.error(`FAIL ${testCase.image}: ${error.message}`);
    }
  }

  const passed = cases.length - failures.length;
  const accuracy = passed / cases.length;
  console.log(`\nSolver fixture accuracy: ${passed}/${cases.length} (${Math.round(accuracy * 100)}%)`);

  if (failures.length > 0) {
    process.exitCode = 1;
  }
}

main();
