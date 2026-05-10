const { execFile } = require('child_process');
const fs = require('fs/promises');
const path = require('path');

const backendRoot = path.resolve(__dirname, '..', '..');

function resolveFromBackend(value) {
  if (path.isAbsolute(value)) {
    return value;
  }

  return path.resolve(backendRoot, value);
}

function runExecutable(command, args) {
  return new Promise((resolve, reject) => {
    execFile(command, args, { maxBuffer: 1024 * 1024 * 10 }, (error, stdout, stderr) => {
      if (error) {
        error.stderr = stderr;
        reject(error);
        return;
      }

      resolve(stdout);
    });
  });
}

function processedImageUrl(jobId, imagePath) {
  return `/processed/${jobId}/${path.basename(imagePath)}`;
}

function mapSolverResult(result, jobId) {
  const bestImageUrl = result.best?.imagePath
    ? processedImageUrl(jobId, result.best.imagePath)
    : null;

  return {
    ...result,
    best: result.best
      ? {
          ...result.best,
          imageUrl: bestImageUrl
        }
      : null,
    variants: Array.isArray(result.variants)
      ? result.variants.map((variant) => ({
          ...variant,
          imageUrl: processedImageUrl(jobId, variant.imagePath),
          isWinning: Array.isArray(result.best?.winningVariants)
            ? result.best.winningVariants.includes(variant.name)
            : variant.name === result.best?.variantName
        }))
      : [],
    candidates: Array.isArray(result.candidates) ? result.candidates : []
  };
}

async function cleanupJob(jobId, uploadedFilename) {
  if (!jobId || !/^[a-zA-Z0-9_-]+$/.test(jobId)) {
    throw new Error('A valid jobId is required.');
  }

  const processedDirectory = path.join(backendRoot, 'processed', jobId);
  await fs.rm(processedDirectory, { recursive: true, force: true });

  if (uploadedFilename) {
    const safeFilename = path.basename(uploadedFilename);
    const uploadPath = path.join(backendRoot, 'uploads', safeFilename);
    await fs.rm(uploadPath, { force: true });
  }
}

async function runSolver(imagePath, jobId) {
  const solverPath = resolveFromBackend(
    process.env.SOLVER_PATH || '../solver/build/ocr-equation-solver'
  );
  const tessdataPath = process.env.TESSDATA_PATH || '/opt/homebrew/share/tessdata';
  const processedDirectory = path.join(backendRoot, 'processed', jobId);

  await fs.mkdir(processedDirectory, { recursive: true });

  const stdout = await runExecutable(solverPath, [
    imagePath,
    tessdataPath,
    processedDirectory,
    '--json'
  ]);

  try {
    const parsedResult = JSON.parse(stdout);
    return mapSolverResult(parsedResult, jobId);
  } catch (error) {
    throw new Error(`Solver returned invalid JSON: ${error.message}`);
  }
}

module.exports = {
  cleanupJob,
  runSolver
};
