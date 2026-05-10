function CandidateList({ candidates = [] }) {
  if (candidates.length === 0) {
    return null;
  }

  return (
    <section className="panel">
      <h2>Candidates</h2>
      <div className="candidate-list">
        {candidates.map((candidate) => (
          <div
            key={candidate.variantName}
            className={candidate.success ? 'candidate success' : 'candidate failure'}
          >
            <div>
              <strong>{candidate.variantName}</strong>
              <span>{candidate.success ? 'Success' : 'Failed'}</span>
            </div>
            {candidate.success ? (
              <p>
                Votes: {candidate.voteCount || 0}
                {candidate.status ? ` | ${candidate.status}` : ''}
              </p>
            ) : (
              <p>{candidate.errorMessage}</p>
            )}
          </div>
        ))}
      </div>
    </section>
  );
}

export default CandidateList;
