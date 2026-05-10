import { useEffect, useState } from 'react';
import { createRoot } from 'react-dom/client';

import { resetJob, solveImage } from './api/solveApi';
import CandidateList from './components/CandidateList';
import ImagePreview from './components/ImagePreview';
import ImageUpload from './components/ImageUpload';
import MatrixDisplay from './components/MatrixDisplay';
import ProcessedImages from './components/ProcessedImages';
import ResultSummary from './components/ResultSummary';
import './App.css';

function App() {
  const [file, setFile] = useState(null);
  const [previewUrl, setPreviewUrl] = useState('');
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [resetting, setResetting] = useState(false);
  const [error, setError] = useState('');

  useEffect(() => {
    if (!file) {
      setPreviewUrl('');
      return undefined;
    }

    const objectUrl = URL.createObjectURL(file);
    setPreviewUrl(objectUrl);

    return () => URL.revokeObjectURL(objectUrl);
  }, [file]);

  function handleFileChange(event) {
    setFile(event.target.files?.[0] || null);
    setResult(null);
    setError('');
  }

  async function handleSubmit(event) {
    event.preventDefault();

    if (!file) {
      return;
    }

    setLoading(true);
    setError('');

    try {
      const data = await solveImage(file);
      setResult(data);
    } catch (requestError) {
      setError(requestError.message);
    } finally {
      setLoading(false);
    }
  }

  async function handleReset() {
    setError('');
    setResetting(true);

    try {
      if (result?.jobId) {
        await resetJob(result.jobId, result.uploadedFilename);
      }

      setFile(null);
      setResult(null);
      setPreviewUrl('');
    } catch (resetError) {
      setError(resetError.message);
    } finally {
      setResetting(false);
    }
  }

  return (
    <main className="app-shell">
      <header>
        <h1>OCR Equation Solver</h1>
      </header>

      <ImageUpload
        file={file}
        previewUrl={previewUrl}
        loading={loading}
        resetting={resetting}
        showReset={Boolean(file || result)}
        onFileChange={handleFileChange}
        onReset={handleReset}
        onSubmit={handleSubmit}
      />

      {error && <div className="error-box">{error}</div>}

      {result && (
        <>
          <ImagePreview imageUrl={result.uploadedImageUrl} />
          <ProcessedImages
            variants={result.variants}
            winningVariants={result.best?.winningVariants || []}
          />
          <ResultSummary best={result.best} />
          <MatrixDisplay matrix={result.best?.A} vector={result.best?.b} />
          <CandidateList candidates={result.candidates} />
        </>
      )}
    </main>
  );
}

createRoot(document.getElementById('root')).render(<App />);
