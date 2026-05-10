const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:5001';

export function getAssetUrl(path) {
  if (!path) {
    return '';
  }

  if (path.startsWith('http://') || path.startsWith('https://')) {
    return path;
  }

  return `${API_BASE_URL}${path}`;
}

export async function solveImage(file) {
  const formData = new FormData();
  formData.append('image', file);

  const response = await fetch(`${API_BASE_URL}/api/solve`, {
    method: 'POST',
    body: formData
  });

  const data = await response.json();

  if (!response.ok) {
    throw new Error(data.details || data.error || 'Failed to analyze image.');
  }

  return data;
}

export async function resetJob(jobId, uploadedFilename) {
  if (!jobId) {
    return;
  }

  const response = await fetch(`${API_BASE_URL}/api/solve/${jobId}`, {
    method: 'DELETE',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ uploadedFilename })
  });

  const data = await response.json();

  if (!response.ok) {
    throw new Error(data.details || data.error || 'Failed to reset job.');
  }
}
