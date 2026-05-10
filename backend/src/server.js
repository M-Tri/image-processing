const path = require('path');

const cors = require('cors');
const dotenv = require('dotenv');
const express = require('express');

const solveRoutes = require('./routes/solveRoutes');

dotenv.config();

const app = express();
const port = process.env.PORT || 5001;
const frontendOrigin = process.env.FRONTEND_ORIGIN || 'http://localhost:5173';
const backendRoot = path.resolve(__dirname, '..');

app.use(cors({ origin: frontendOrigin }));
app.use(express.json());

app.use('/uploads', express.static(path.join(backendRoot, 'uploads')));
app.use('/processed', express.static(path.join(backendRoot, 'processed')));

app.use('/api/solve', solveRoutes);

app.use((error, req, res, next) => {
  if (res.headersSent) {
    return next(error);
  }

  return res.status(400).json({
    error: error.message || 'Request failed'
  });
});

app.listen(port, () => {
  console.log(`Backend listening on http://localhost:${port}`);
});
