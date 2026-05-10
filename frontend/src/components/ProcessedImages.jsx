import { getAssetUrl } from '../api/solveApi';

function ProcessedImages({ variants = [], winningVariants = [] }) {
  if (variants.length === 0) {
    return null;
  }

  return (
    <section className="panel">
      <h2>Processed Images</h2>
      <div className="image-grid">
        {variants.map((variant) => (
          <figure
            key={variant.name}
            className={
              variant.isWinning || winningVariants.includes(variant.name)
                ? 'variant-card winning'
                : 'variant-card'
            }
          >
            <img src={getAssetUrl(variant.imageUrl)} alt={variant.name} />
            <figcaption>{variant.name}</figcaption>
          </figure>
        ))}
      </div>
    </section>
  );
}

export default ProcessedImages;
