function MatrixDisplay({ matrix = [], vector = [] }) {
  if (matrix.length === 0 && vector.length === 0) {
    return null;
  }

  return (
    <section className="panel">
      <h2>Parsed System</h2>
      <div className="matrix-wrap">
        <div>
          <h3>Matrix A</h3>
          <table className="simple-table">
            <tbody>
              {matrix.map((row, rowIndex) => (
                <tr key={rowIndex}>
                  {row.map((value, colIndex) => (
                    <td key={colIndex}>{Number(value).toFixed(6)}</td>
                  ))}
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        <div>
          <h3>Vector b</h3>
          <table className="simple-table compact-table">
            <tbody>
              {vector.map((value, index) => (
                <tr key={index}>
                  <td>{Number(value).toFixed(6)}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </section>
  );
}

export default MatrixDisplay;
