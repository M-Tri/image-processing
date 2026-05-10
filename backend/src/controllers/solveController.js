const { v4: uuidv4 } = require('uuid');

const { cleanupJob, runSolver } = require('../services/solverService');

async function solveImage(req, res) {
  if (!req.file) {
    return res.status(400).json({ error: 'Image upload is required.' });
  }

  const jobId = uuidv4();

  try {
    const solverResult = await runSolver(req.file.path, jobId);

    return res.json({
      jobId,
      uploadedFilename: req.file.filename,
      uploadedImageUrl: `/uploads/${req.file.filename}`,
      ...solverResult
    });
  } catch (error) {
    return res.status(500).json({
      error: 'Failed to solve uploaded image.',
      details: error.stderr || error.message
    });
  }
}

async function resetJob(req, res) {
  const { jobId } = req.params;
  const { uploadedFilename } = req.body || {};

  try {
    await cleanupJob(jobId, uploadedFilename);
    return res.json({ success: true });
  } catch (error) {
    return res.status(400).json({
      error: 'Failed to reset job.',
      details: error.message
    });
  }
}

module.exports = {
  resetJob,
  solveImage
};
