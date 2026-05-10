function ResultSummary({ best }) {
  if (!best) {
    return null;
  }

  const confidencePercent = Math.round((best.confidence || 0) * 1000) / 10;
  const solutionEntries = Object.entries(best.solution || {});
  const winningVariants = best.winningVariants || [best.variantName].filter(Boolean);

  return (
    <section className="panel">
      <h2>Best Result</h2>
      <div className="summary-grid">
        <div>
          <span className="label">Representative variant</span>
          <strong>{best.variantName}</strong>
        </div>
        <div>
          <span className="label">Confidence</span>
          <strong>{confidencePercent}%</strong>
        </div>
        <div>
          <span className="label">Votes</span>
          <strong>
            {best.voteCount} / {best.totalSuccessfulCandidates}
          </strong>
        </div>
        <div>
          <span className="label">Status</span>
          <strong>{best.status}</strong>
        </div>
      </div>

      <h3>Winning Variants</h3>
      <div className="chip-list">
        {winningVariants.map((variantName) => (
          <span className="chip" key={variantName}>
            {variantName}
          </span>
        ))}
      </div>

      <h3>OCR Text</h3>
      <pre className="ocr-text">{best.extractedText}</pre>

      <h3>Solution</h3>
      {solutionEntries.length > 0 ? (
        <table className="simple-table compact-table">
          <tbody>
            {solutionEntries.map(([variable, value]) => (
              <tr key={variable}>
                <th>{variable}</th>
                <td>{Number(value).toFixed(6)}</td>
              </tr>
            ))}
          </tbody>
        </table>
      ) : (
        <p>No solution values returned.</p>
      )}
    </section>
  );
}

export default ResultSummary;
