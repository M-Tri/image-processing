import { useRef } from 'react';

function ImageUpload({
  file,
  previewUrl,
  loading,
  resetting,
  showReset,
  onFileChange,
  onReset,
  onSubmit
}) {
  const fileInputRef = useRef(null);

  async function handleResetClick() {
    await onReset();

    if (fileInputRef.current) {
      fileInputRef.current.value = '';
    }
  }

  return (
    <form className="panel upload-panel" onSubmit={onSubmit}>
      <label className="field-label" htmlFor="image-upload">
        Equation image
      </label>
      <input
        ref={fileInputRef}
        id="image-upload"
        type="file"
        accept="image/png,image/jpeg"
        onChange={onFileChange}
      />

      {previewUrl && (
        <div className="preview-block">
          <img src={previewUrl} alt="Selected equation" />
        </div>
      )}

      <div className="action-row">
        <button type="submit" disabled={!file || loading}>
          {loading ? 'Analyzing...' : 'Analyze Image'}
        </button>

        {showReset && (
          <button
            className="secondary-button"
            type="button"
            onClick={handleResetClick}
            disabled={resetting}
          >
            {resetting ? 'Resetting...' : 'Reset'}
          </button>
        )}
      </div>
    </form>
  );
}

export default ImageUpload;
