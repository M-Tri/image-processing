const express = require('express');

const { resetJob, solveImage } = require('../controllers/solveController');
const upload = require('../middleware/uploadMiddleware');

const router = express.Router();

router.post('/', upload.single('image'), solveImage);
router.delete('/:jobId', resetJob);

module.exports = router;
