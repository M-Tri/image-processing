import { getAssetUrl } from '../api/solveApi';

function ImagePreview({ imageUrl }) {
  if (!imageUrl) {
    return null;
  }

  return (
    <section className="panel">
      <h2>Uploaded Image</h2>
      <img className="main-image" src={getAssetUrl(imageUrl)} alt="Uploaded equation" />
    </section>
  );
}

export default ImagePreview;
