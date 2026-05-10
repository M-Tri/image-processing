const fs = require('fs');
const path = require('path');

const multer = require('multer');

const uploadsDirectory = path.resolve(__dirname, '..', '..', 'uploads');
fs.mkdirSync(uploadsDirectory, { recursive: true });

const allowedMimeTypes = new Set([
  'image/png',
  'image/jpeg',
  'image/jpg'
]);

const storage = multer.diskStorage({
  destination: (req, file, callback) => {
    callback(null, uploadsDirectory);
  },
  filename: (req, file, callback) => {
    const extension = path.extname(file.originalname).toLowerCase();
    const baseName = path.basename(file.originalname, extension).replace(/[^a-z0-9_-]/gi, '-');
    const uniqueName = `${baseName || 'image'}-${Date.now()}${extension}`;
    callback(null, uniqueName);
  }
});

const upload = multer({
  storage,
  fileFilter: (req, file, callback) => {
    const extension = path.extname(file.originalname).toLowerCase();
    const allowedExtension = extension === '.png' || extension === '.jpg' || extension === '.jpeg';

    if (!allowedMimeTypes.has(file.mimetype) || !allowedExtension) {
      callback(new Error('Only PNG, JPG, and JPEG images are supported.'));
      return;
    }

    callback(null, true);
  }
});

module.exports = upload;
