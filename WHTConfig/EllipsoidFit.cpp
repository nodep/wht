#include "stdafx.h"

#include "EllipsoidFit.h"

// *********************************************
// This is a translation of the Java Ellipsoid Fit library found here:
// https://github.com/KEOpenSource/EllipsoidFit
// The Java library is, in turn, based on a MATLAB script written by Yury Petrov. It can be found here:
// http://www.mathworks.com/matlabcentral/fileexchange/24693-ellipsoid-fit
// *********************************************

/**
 * Smallest positive number such that {@code 1 - EPSILON} is not
 * numerically equal to 1: {@value}.
 */
const double EPSILON = 1.1102230246251565E-16;		// 0x1.0p-53
const double SAFE_MIN = 2.2250738585072014E-308;

/** Relative threshold for small singular values. */
const double EPS = 2.220446049250313E-16;
/** Absolute threshold for small singular values. */
const double TINY = 1.6033346880071782E-291;


/**
 * Gets a submatrix. Rows and columns are indicated
 * counting from 0 to n-1.
 *
 * @param startRow Initial row index
 * @param endRow Final row index (inclusive)
 * @param startColumn Initial column index
 * @param endColumn Final column index (inclusive)
 * @return The subMatrix containing the data of the
 *         specified rows and columns
 * @throws org.apache.commons.math3.exception.OutOfRangeException if
 * the indices are not valid.
 */
RealMatrix RealMatrix::getSubMatrix(int startRow, int endRow, int startColumn, int endColumn) const
{
	checkSubMatrixIndex(startRow, endRow, startColumn, endColumn);

	RealMatrix subMatrix(endRow - startRow + 1, endColumn - startColumn + 1);
	for (int i = startRow; i <= endRow; ++i)
		for (int j = startColumn; j <= endColumn; ++j)
			subMatrix[i - startRow][j - startColumn] = (*this)[i][j];

	return subMatrix;
}

/**
 * Returns the transpose of this matrix.
 *
 * @return transpose matrix
 */
RealMatrix RealMatrix::transpose() const
{
    int nRows = getRowDimension();
    int nCols = getColumnDimension();
    RealMatrix out(nCols, nRows);

	for (int c = 0; c < nCols; c++)
		for (int r = 0; r < nRows; r++)
			out[c][r] = (*this)[r][c];

    return out;
}

/**
 * Returns the result of postmultiplying this by m.
 *
 * @param m    matrix to postmultiply by
 * @return     this * m
 * @throws     IllegalArgumentException
 *             if columnDimension(this) != rowDimension(m)
 */
RealMatrix RealMatrix::multiply(const RealMatrix& m) const
{
    // Safety check.
    checkMultiplicationCompatible(m);

    int nRows = getRowDimension();
    int nCols = m.getColumnDimension();
    int nSum  = getColumnDimension();
    RealMatrix out(nRows, nCols);
    for (int row = 0; row < nRows; ++row)
	{
        for (int col = 0; col < nCols; ++col)
		{
            double sum = 0;
            for (int i = 0; i < nSum; ++i)
                sum += (*this)[row][i] * m[i][col];

            out[row][col] = sum;
        }
    }

    return out;
}

/**
 * Replace the submatrix starting at <code>row, column</code> using data in
 * the input <code>subMatrix</code> array. Indexes are 0-based.
 * <p>
 * Example:<br>
 * Starting with <pre>
 * 1  2  3  4
 * 5  6  7  8
 * 9  0  1  2
 * </pre>
 * and <code>subMatrix = {{3, 4} {5,6}}</code>, invoking
 * <code>setSubMatrix(subMatrix,1,1))</code> will result in <pre>
 * 1  2  3  4
 * 5  3  4  8
 * 9  5  6  2
 * </pre></p>
 *
 * @param subMatrix  array containing the submatrix replacement data
 * @param row  row coordinate of the top, left element to be replaced
 * @param column  column coordinate of the top, left element to be replaced
 * @throws ZeroException if {@code subMatrix} does not contain at least one column.
 * @throws OutOfRangeException if {@code subMatrix} does not fit into
 * this matrix from element in {@code (row, column)}.
 * @throws DimensionMismatchException if {@code subMatrix} is not rectangular.
 * (not all rows have the same length) or empty.
 * @throws NullArgumentException if {@code subMatrix} is {@code null}.
 * @since 2.0
 */
void RealMatrix::setSubMatrix(const RealMatrix& subMatrix, int row, int column)
{
    // MathUtils.checkNotNull(subMatrix);

    int nRows = subMatrix.size();
    if (nRows == 0)
	{
		assert(false);
        //throw new NoDataException(LocalizedFormats.AT_LEAST_ONE_ROW);
    }

    int nCols = subMatrix[0].size();
    if (nCols == 0)
	{
		assert(false);
        //throw new NoDataException(LocalizedFormats.AT_LEAST_ONE_COLUMN);
    }

    for (int r = 1; r < nRows; ++r)
	{
        if (subMatrix[r].size() != nCols)
		{
			assert(false);
            // throw new DimensionMismatchException(nCols, subMatrix[r].length);
        }
    }

    checkRowIndex(row);
    checkColumnIndex(column);
    checkRowIndex(nRows + row - 1);
    checkColumnIndex(nCols + column - 1);

    for (int i = 0; i < nRows; ++i)
        for (int j = 0; j < nCols; ++j)
			(*this)[row + i][column + j] = subMatrix[i][j];		// setEntry(row + i, column + j, subMatrix[i][j]);
}

/**
 * Returns the result of multiplying this by the vector <code>v</code>.
 *
 * @param v the vector to operate on
 * @return this*v
 * @throws IllegalArgumentException if columnDimension != v.size()
 */
RealVector RealMatrix::operate(const RealVector& v) const
{
    int nRows = getRowDimension();
    int nCols = getColumnDimension();
    if (v.size() != nCols)
	{
		assert(false);
        // throw new DimensionMismatchException(v.length, nCols);
    }

    RealVector out(nRows, 0.0);
    for (int row = 0; row < nRows; ++row)
	{
        double sum = 0;
        for (int i = 0; i < nCols; ++i)
            sum += (*this)[row][i] * v[i];

        out[row] = sum;
    }

    return out;
}

/**
 * Build a solver from decomposed matrix.
 *
 * @param singularValues Singular values.
 * @param uT U<sup>T</sup> matrix of the decomposition.
 * @param v V matrix of the decomposition.
 * @param nonSingular Singularity indicator.
 * @param tol tolerance for singular values
 */
DecompositionSolver::DecompositionSolver(const RealVector& singularValues, const RealMatrix& uT, const RealMatrix& v, bool nonSingularArg, double tol)
{
    RealMatrix suT(uT);
    for (size_t i = 0; i < singularValues.size(); ++i)
	{
        double a;
        if (singularValues[i] > tol)
            a = 1 / singularValues[i];
        else
            a = 0;

        RealVector& suTi(suT[i]);
        for (size_t j = 0; j < suTi.size(); ++j)
            suTi[j] *= a;
    }

    pseudoInverse = v.multiply(suT);
    nonSingular = nonSingularArg;
}

/**
 * Calculates the compact Singular Value Decomposition of the given matrix.
 *
 * @param matrix Matrix to decompose.
 */
SingularValueDecomposition::SingularValueDecomposition(const RealMatrix& matrix)
{
    RealMatrix A;

    // "m" is always the largest dimension.
    if (matrix.getRowDimension() < matrix.getColumnDimension())
	{
        transposed = true;
        A = matrix.transpose();
        m = matrix.getColumnDimension();
        n = matrix.getRowDimension();
    } else {
        transposed = false;
        A = matrix;
        m = matrix.getRowDimension();
        n = matrix.getColumnDimension();
    }

    singularValues = RealVector(n, 0.0);
    RealMatrix U(m, RealVector(n, 0.0));
    RealMatrix V(n, RealVector(n, 0.0));
    RealVector e(n, 0.0);
    RealVector work(m, 0.0);

    // Reduce A to bidiagonal form, storing the diagonal elements
    // in s and the super-diagonal elements in e.
    int nct = std::min(m - 1, n);
    int nrt = std::max(0, n - 2);
    for (int k = 0; k < std::max(nct, nrt); k++)
	{
        if (k < nct) {
            // Compute the transformation for the k-th column and
            // place the k-th diagonal in s[k].
            // Compute 2-norm of k-th column without under/overflow.
            singularValues[k] = 0;
            for (int i = k; i < m; i++)
                singularValues[k] = hypot(singularValues[k], A[i][k]);

            if (singularValues[k] != 0) {
                if (A[k][k] < 0) {
                    singularValues[k] = -singularValues[k];
                }
                for (int i = k; i < m; i++) {
                    A[i][k] /= singularValues[k];
                }
                A[k][k] += 1;
            }
            singularValues[k] = -singularValues[k];
        }
        for (int j = k + 1; j < n; j++) {
            if (k < nct &&
                singularValues[k] != 0) {
                // Apply the transformation.
                double t = 0;
                for (int i = k; i < m; i++) {
                    t += A[i][k] * A[i][j];
                }
                t = -t / A[k][k];
                for (int i = k; i < m; i++) {
                    A[i][j] += t * A[i][k];
                }
            }
            // Place the k-th row of A into e for the
            // subsequent calculation of the row transformation.
            e[j] = A[k][j];
        }
        if (k < nct) {
            // Place the transformation in U for subsequent back
            // multiplication.
            for (int i = k; i < m; i++) {
                U[i][k] = A[i][k];
            }
        }
        if (k < nrt) {
            // Compute the k-th row transformation and place the
            // k-th super-diagonal in e[k].
            // Compute 2-norm without under/overflow.
            e[k] = 0;
            for (int i = k + 1; i < n; i++) {
                e[k] = hypot(e[k], e[i]);
            }
            if (e[k] != 0) {
                if (e[k + 1] < 0) {
                    e[k] = -e[k];
                }
                for (int i = k + 1; i < n; i++) {
                    e[i] /= e[k];
                }
                e[k + 1] += 1;
            }
            e[k] = -e[k];
            if (k + 1 < m &&
                e[k] != 0) {
                // Apply the transformation.
                for (int i = k + 1; i < m; i++) {
                    work[i] = 0;
                }
                for (int j = k + 1; j < n; j++) {
                    for (int i = k + 1; i < m; i++) {
                        work[i] += e[j] * A[i][j];
                    }
                }
                for (int j = k + 1; j < n; j++) {
                    double t = -e[j] / e[k + 1];
                    for (int i = k + 1; i < m; i++) {
                        A[i][j] += t * work[i];
                    }
                }
            }

            // Place the transformation in V for subsequent
            // back multiplication.
            for (int i = k + 1; i < n; i++) {
                V[i][k] = e[i];
            }
        }
    }
    // Set up the final bidiagonal matrix or order p.
    int p = n;
    if (nct < n) {
        singularValues[nct] = A[nct][nct];
    }
    if (m < p) {
        singularValues[p - 1] = 0;
    }
    if (nrt + 1 < p) {
        e[nrt] = A[nrt][p - 1];
    }
    e[p - 1] = 0;

    // Generate U.
    for (int j = nct; j < n; j++) {
        for (int i = 0; i < m; i++) {
            U[i][j] = 0;
        }
        U[j][j] = 1;
    }
    for (int k = nct - 1; k >= 0; k--) {
        if (singularValues[k] != 0) {
            for (int j = k + 1; j < n; j++) {
                double t = 0;
                for (int i = k; i < m; i++) {
                    t += U[i][k] * U[i][j];
                }
                t = -t / U[k][k];
                for (int i = k; i < m; i++) {
                    U[i][j] += t * U[i][k];
                }
            }
            for (int i = k; i < m; i++) {
                U[i][k] = -U[i][k];
            }
            U[k][k] = 1 + U[k][k];
            for (int i = 0; i < k - 1; i++) {
                U[i][k] = 0;
            }
        } else {
            for (int i = 0; i < m; i++) {
                U[i][k] = 0;
            }
            U[k][k] = 1;
        }
    }

    // Generate V.
    for (int k = n - 1; k >= 0; k--) {
        if (k < nrt &&
            e[k] != 0) {
            for (int j = k + 1; j < n; j++) {
                double t = 0;
                for (int i = k + 1; i < n; i++) {
                    t += V[i][k] * V[i][j];
                }
                t = -t / V[k + 1][k];
                for (int i = k + 1; i < n; i++) {
                    V[i][j] += t * V[i][k];
                }
            }
        }
        for (int i = 0; i < n; i++) {
            V[i][k] = 0;
        }
        V[k][k] = 1;
    }

    // Main iteration loop for the singular values.
    int pp = p - 1;
    int iter = 0;
    while (p > 0) {
        int k;
        int kase;
        // Here is where a test for too many iterations would go.
        // This section of the program inspects for
        // negligible elements in the s and e arrays.  On
        // completion the variables kase and k are set as follows.
        // kase = 1     if s(p) and e[k-1] are negligible and k<p
        // kase = 2     if s(k) is negligible and k<p
        // kase = 3     if e[k-1] is negligible, k<p, and
        //              s(k), ..., s(p) are not negligible (qr step).
        // kase = 4     if e(p-1) is negligible (convergence).
        for (k = p - 2; k >= 0; k--)
		{
            double threshold = TINY + EPS * (fabs(singularValues[k]) + fabs(singularValues[k + 1]));
            if (fabs(e[k]) <= threshold)
			{
                e[k] = 0;
                break;
            }
        }

        if (k == p - 2) {
            kase = 4;
        } else {
            int ks;
            for (ks = p - 1; ks >= k; ks--) {
                if (ks == k) {
                    break;
                }
                double t = (ks != p ? fabs(e[ks]) : 0) + (ks != k + 1 ? fabs(e[ks - 1]) : 0);
                if (fabs(singularValues[ks]) <= TINY + EPS * t) {
                    singularValues[ks] = 0;
                    break;
                }
            }
            if (ks == k) {
                kase = 3;
            } else if (ks == p - 1) {
                kase = 1;
            } else {
                kase = 2;
                k = ks;
            }
        }
        k++;
        // Perform the task indicated by kase.
        switch (kase) {
            // Deflate negligible s(p).
            case 1: {
                double f = e[p - 2];
                e[p - 2] = 0;
                for (int j = p - 2; j >= k; j--) {
                    double t = hypot(singularValues[j], f);
                    double cs = singularValues[j] / t;
                    double sn = f / t;
                    singularValues[j] = t;
                    if (j != k)
					{
                        f = -sn * e[j - 1];
                        e[j - 1] = cs * e[j - 1];
                    }

                    for (int i = 0; i < n; i++) {
                        t = cs * V[i][j] + sn * V[i][p - 1];
                        V[i][p - 1] = -sn * V[i][j] + cs * V[i][p - 1];
                        V[i][j] = t;
                    }
                }
            }
            break;
            // Split at negligible s(k).
            case 2: {
                double f = e[k - 1];
                e[k - 1] = 0;
                for (int j = k; j < p; j++)
				{
                    double t = hypot(singularValues[j], f);
                    double cs = singularValues[j] / t;
                    double sn = f / t;
                    singularValues[j] = t;
                    f = -sn * e[j];
                    e[j] = cs * e[j];

                    for (int i = 0; i < m; i++) {
                        t = cs * U[i][j] + sn * U[i][k - 1];
                        U[i][k - 1] = -sn * U[i][j] + cs * U[i][k - 1];
                        U[i][j] = t;
                    }
                }
            }
            break;
            // Perform one qr step.
            case 3: {
                // Calculate the shift.
                double maxPm1Pm2 = std::max(fabs(singularValues[p - 1]), fabs(singularValues[p - 2]));
                double scale = std::max(std::max(std::max(maxPm1Pm2, fabs(e[p - 2])),fabs(singularValues[k])), fabs(e[k]));
                double sp = singularValues[p - 1] / scale;
                double spm1 = singularValues[p - 2] / scale;
                double epm1 = e[p - 2] / scale;
                double sk = singularValues[k] / scale;
                double ek = e[k] / scale;
                double b = ((spm1 + sp) * (spm1 - sp) + epm1 * epm1) / 2.0;
                double c = (sp * epm1) * (sp * epm1);
                double shift = 0;
                if (b != 0  ||  c != 0)
				{
                    shift = sqrt(b * b + c);
                    if (b < 0)
                        shift = -shift;

                    shift = c / (b + shift);
                }
                double f = (sk + sp) * (sk - sp) + shift;
                double g = sk * ek;
                // Chase zeros.
                for (int j = k; j < p - 1; j++)
				{
                    double t = hypot(f, g);
                    double cs = f / t;
                    double sn = g / t;
                    if (j != k) {
                        e[j - 1] = t;
                    }
                    f = cs * singularValues[j] + sn * e[j];
                    e[j] = cs * e[j] - sn * singularValues[j];
                    g = sn * singularValues[j + 1];
                    singularValues[j + 1] = cs * singularValues[j + 1];

                    for (int i = 0; i < n; i++) {
                        t = cs * V[i][j] + sn * V[i][j + 1];
                        V[i][j + 1] = -sn * V[i][j] + cs * V[i][j + 1];
                        V[i][j] = t;
                    }
                    t = hypot(f, g);
                    cs = f / t;
                    sn = g / t;
                    singularValues[j] = t;
                    f = cs * e[j] + sn * singularValues[j + 1];
                    singularValues[j + 1] = -sn * e[j] + cs * singularValues[j + 1];
                    g = sn * e[j + 1];
                    e[j + 1] = cs * e[j + 1];
                    if (j < m - 1) {
                        for (int i = 0; i < m; i++) {
                            t = cs * U[i][j] + sn * U[i][j + 1];
                            U[i][j + 1] = -sn * U[i][j] + cs * U[i][j + 1];
                            U[i][j] = t;
                        }
                    }
                }
                e[p - 2] = f;
                iter = iter + 1;
            }
            break;
            // Convergence.
            default: {
                // Make the singular values positive.
                if (singularValues[k] <= 0) {
                    singularValues[k] = singularValues[k] < 0 ? -singularValues[k] : 0;

                    for (int i = 0; i <= pp; i++) {
                        V[i][k] = -V[i][k];
                    }
                }
                // Order the singular values.
                while (k < pp) {
                    if (singularValues[k] >= singularValues[k + 1]) {
                        break;
                    }
                    double t = singularValues[k];
                    singularValues[k] = singularValues[k + 1];
                    singularValues[k + 1] = t;
                    if (k < n - 1) {
                        for (int i = 0; i < n; i++) {
                            t = V[i][k + 1];
                            V[i][k + 1] = V[i][k];
                            V[i][k] = t;
                        }
                    }
                    if (k < m - 1) {
                        for (int i = 0; i < m; i++) {
                            t = U[i][k + 1];
                            U[i][k + 1] = U[i][k];
                            U[i][k] = t;
                        }
                    }
                    k++;
                }
                iter = 0;
                p--;
            }
            break;
        }
    }

    // Set the small value tolerance used to calculate rank and pseudo-inverse
    tol = std::max(m * singularValues[0] * EPS, sqrt(SAFE_MIN));

    if (!transposed)
	{
        cachedU = U;
        cachedV = V;
    } else {
        cachedU = V;
        cachedV = U;
    }
}

/**
 * Calculates the eigen decomposition of the given symmetric matrix.
 *
 * @param matrix Matrix to decompose. It <em>must</em> be symmetric.
 * @param splitTolerance Dummy parameter (present for backward
 * compatibility only).
 * @throws NonSymmetricMatrixException if the matrix is not symmetric.
 * @throws MaxCountExceededException if the algorithm fails to converge.
 */
EigenDecomposition::EigenDecomposition(const RealMatrix& matrix, double splitTolerance)
{
	if (isSymmetric(matrix, true))
	{
		transformToTridiagonal(matrix);
		findEigenVectors(transformer.getQ());
	}
}

/**
 * Check if a matrix is symmetric.
 *
 * @param matrix Matrix to check.
 * @param raiseException If {@code true}, the method will throw an
 * exception if {@code matrix} is not symmetric.
 * @return {@code true} if {@code matrix} is symmetric.
 * @throws NonSymmetricMatrixException if the matrix is not symmetric and
 * {@code raiseException} is {@code true}.
 */
bool EigenDecomposition::isSymmetric(const RealMatrix& matrix, bool raiseException)
{
    int rows = matrix.getRowDimension();
    int columns = matrix.getColumnDimension();
    double eps = 10 * rows * columns * EPSILON;
    for (int i = 0; i < rows; ++i)
	{
        for (int j = i + 1; j < columns; ++j)
		{
            double mij = matrix[i][j];
            double mji = matrix[j][i];
            if (fabs(mij - mji)  >  std::max(fabs(mij), fabs(mji)) * eps)
			{
                if (raiseException)
				{
					assert(false);
                    //throw new NonSymmetricMatrixException(i, j, eps);
                }
                return false;
            }
        }
    }
    return true;
}

/**
 * Find eigenvalues and eigenvectors (Dubrulle et al., 1971)
 *
 * @param householderMatrix Householder matrix of the transformation
 * to tridiagonal form.
 */
void EigenDecomposition::findEigenVectors(const RealMatrix& householderMatrix)
{
    RealMatrix z(householderMatrix);
    int n = main.size();
    imagEigenvalues = realEigenvalues = RealVector(n, 0.0);
    RealVector e(n, 0.0);
    for (int i = 0; i < n - 1; i++)
	{
        realEigenvalues[i] = main[i];
        e[i] = secondary[i];
    }
    realEigenvalues[n - 1] = main[n - 1];
    e[n - 1] = 0;

    // Determine the largest main and secondary value in absolute term.
    double maxAbsoluteValue = 0;
    for (int i = 0; i < n; i++)
	{
        if (fabs(realEigenvalues[i]) > maxAbsoluteValue)
            maxAbsoluteValue = fabs(realEigenvalues[i]);

        if (fabs(e[i]) > maxAbsoluteValue)
            maxAbsoluteValue = fabs(e[i]);
    }

    // Make null any main and secondary value too small to be significant
    if (maxAbsoluteValue != 0)
	{
        for (int i=0; i < n; i++)
		{
            if (fabs(realEigenvalues[i]) <= EPSILON * maxAbsoluteValue)
                realEigenvalues[i] = 0;

            if (fabs(e[i]) <= EPSILON * maxAbsoluteValue)
                e[i] = 0;
        }
    }

    for (int j = 0; j < n; j++)
	{
        int its = 0;
        int m;
        do {
            for (m = j; m < n - 1; m++)
			{
                double delta = fabs(realEigenvalues[m]) + fabs(realEigenvalues[m + 1]);
                if (fabs(e[m]) + delta == delta)
                    break;

            }
            if (m != j)
			{
                if (its == maxIter)
				{
					assert(false);
					/*
                    throw new MaxCountExceededException(LocalizedFormats.CONVERGENCE_FAILED,
                                                        maxIter);
														*/
                }
                its++;
                double q = (realEigenvalues[j + 1] - realEigenvalues[j]) / (2 * e[j]);
                double t = sqrt(1 + q * q);
                if (q < 0.0) {
                    q = realEigenvalues[m] - realEigenvalues[j] + e[j] / (q - t);
                } else {
                    q = realEigenvalues[m] - realEigenvalues[j] + e[j] / (q + t);
                }
                double u = 0.0;
                double s = 1.0;
                double c = 1.0;
                int i;
                for (i = m - 1; i >= j; i--) {
                    double p = s * e[i];
                    double h = c * e[i];
                    if (fabs(p) >= fabs(q)) {
                        c = q / p;
                        t = sqrt(c * c + 1.0);
                        e[i + 1] = p * t;
                        s = 1.0 / t;
                        c = c * s;
                    } else {
                        s = p / q;
                        t = sqrt(s * s + 1.0);
                        e[i + 1] = q * t;
                        c = 1.0 / t;
                        s = s * c;
					}

                    if (e[i + 1] == 0.0)
					{
                        realEigenvalues[i + 1] -= u;
                        e[m] = 0.0;
                        break;
                    }
                    q = realEigenvalues[i + 1] - u;
                    t = (realEigenvalues[i] - q) * s + 2.0 * c * h;
                    u = s * t;
                    realEigenvalues[i + 1] = q + u;
                    q = c * t - h;
                    for (int ia = 0; ia < n; ia++)
					{
                        p = z[ia][i + 1];
                        z[ia][i + 1] = s * z[ia][i] + c * p;
                        z[ia][i] = c * z[ia][i] - s * p;
                    }
                }
                
				if (t == 0.0 && i >= j)
                    continue;

                realEigenvalues[j] -= u;
                e[j] = q;
                e[m] = 0.0;
            }
        } while (m != j);
    }

    //Sort the eigen values (and vectors) in increase order
    for (int i = 0; i < n; i++)
	{
        int k = i;
        double p = realEigenvalues[i];
        for (int j = i + 1; j < n; j++)
		{
            if (realEigenvalues[j] > p)
			{
                k = j;
                p = realEigenvalues[j];
            }
        }

        if (k != i)
		{
            realEigenvalues[k] = realEigenvalues[i];
            realEigenvalues[i] = p;
            for (int j = 0; j < n; j++) {
                p = z[j][i];
                z[j][i] = z[j][k];
                z[j][k] = p;
            }
        }
    }

    // Determine the largest eigen value in absolute term.
    maxAbsoluteValue = 0;
    for (int i = 0; i < n; i++)
        if (fabs(realEigenvalues[i]) > maxAbsoluteValue)
            maxAbsoluteValue = fabs(realEigenvalues[i]);

	// Make null any eigen value too small to be significant
    if (maxAbsoluteValue!=0.0) {
        for (int i=0; i < n; i++) {
            if (fabs(realEigenvalues[i]) < EPSILON * maxAbsoluteValue) {
                realEigenvalues[i] = 0;
            }
        }
    }

    eigenvectors.clear();

    RealVector tmp(n, 0.0);

    for (int i = 0; i < n; i++)
	{
        for (int j = 0; j < n; j++)
            tmp[j] = z[j][i];

        eigenvectors.push_back(tmp);
    }
}

/**
 * Build the transformation to tridiagonal shape of a symmetrical matrix.
 * <p>The specified matrix is assumed to be symmetrical without any check.
 * Only the upper triangular part of the matrix is used.</p>
 *
 * @param matrix Symmetrical matrix to transform.
 * @throws NonSquareMatrixException if the matrix is not square.
 */
TriDiagonalTransformer::TriDiagonalTransformer(const RealMatrix& matrix)
{
    if (!matrix.isSquare())
	{
		assert(false);
		/*
        throw new NonSquareMatrixException(matrix.getRowDimension(),
                                            matrix.getColumnDimension());
											*/
    }

    int m = matrix.getRowDimension();
    householderVectors = matrix;
    main      = RealVector(m, 0.0);
    secondary = RealVector(m - 1, 0.0);
    //cachedQ   = null;
    //cachedQt  = null;
    //cachedT   = null;

    // transform matrix
    transform();
}

/**
 * Transform original matrix to tridiagonal form.
 * <p>Transformation is done using Householder transforms.</p>
 */
void TriDiagonalTransformer::transform()
{
    int m = householderVectors.size();
    RealVector z(m, 0.0);
    for (int k = 0; k < m - 1; k++)
	{
        // zero-out a row and a column simultaneously
        RealVector& hK(householderVectors[k]);
        main[k] = hK[k];
        double xNormSqr = 0;
        for (int j = k + 1; j < m; ++j)
		{
            double c = hK[j];
            xNormSqr += c * c;
        }

        double a = (hK[k + 1] > 0) ? -sqrt(xNormSqr) : sqrt(xNormSqr);
        secondary[k] = a;
        if (a != 0.0)
		{
            // apply Householder transform from left and right simultaneously

            hK[k + 1] -= a;
            double beta = -1 / (a * hK[k + 1]);

            // compute a = beta A v, where v is the Householder vector
            // this loop is written in such a way
            //   1) only the upper triangular part of the matrix is accessed
            //   2) access is cache-friendly for a matrix stored in rows
			std::fill(z.begin() + k + 1, z.begin() + m, 0.0);		// Arrays.fill(z, k + 1, m, 0);

            for (int i = k + 1; i < m; ++i)
			{
                RealVector& hI(householderVectors[i]);
                double hKI = hK[i];
                double zI = hI[i] * hKI;
                for (int j = i + 1; j < m; ++j)
				{
                    double hIJ = hI[j];
                    zI   += hIJ * hK[j];
                    z[j] += hIJ * hKI;
                }
                z[i] = beta * (z[i] + zI);
            }

            // compute gamma = beta vT z / 2
            double gamma = 0;
            for (int i = k + 1; i < m; ++i)
                gamma += z[i] * hK[i];

            gamma *= beta / 2;

            // compute z = z - gamma v
            for (int i = k + 1; i < m; ++i)
                z[i] -= gamma * hK[i];

            // update matrix: A = A - v zT - z vT
            // only the upper triangular part of the matrix is updated
            for (int i = k + 1; i < m; ++i)
			{
                RealVector& hI(householderVectors[i]);
                for (int j = i; j < m; ++j)
                    hI[j] -= hK[i] * z[j] + z[i] * hK[j];
            }
        }
    }

    main[m - 1] = householderVectors[m - 1][m - 1];
}

/**
 * Returns the transpose of the matrix Q of the transform.
 * <p>Q is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
 * @return the Q matrix
 */
RealMatrix TriDiagonalTransformer::getQT()
{
    if (cachedQt.empty())
	{
        int m = householderVectors.size();
        RealMatrix qta(m, RealVector(m, 0.0));

        // build up first part of the matrix by applying Householder transforms
        for (int k = m - 1; k >= 1; --k)
		{
            RealVector hK(householderVectors[k - 1]);
            qta[k][k] = 1;
            if (hK[k] != 0.0)
			{
                double inv = 1.0 / (secondary[k - 1] * hK[k]);
                double beta = 1.0 / secondary[k - 1];
                qta[k][k] = 1 + beta * hK[k];
                for (int i = k + 1; i < m; ++i) {
                    qta[k][i] = beta * hK[i];
                }
                for (int j = k + 1; j < m; ++j) {
                    beta = 0;
                    for (int i = k + 1; i < m; ++i) {
                        beta += qta[j][i] * hK[i];
                    }
                    beta *= inv;
                    qta[j][k] = beta * hK[k];
                    for (int i = k + 1; i < m; ++i) {
                        qta[j][i] += beta * hK[i];
                    }
                }
            }
        }
        qta[0][0] = 1;
        cachedQt = qta;	//cachedQt = MatrixUtils.createRealMatrix(qta);
    }

    // return the cached matrix
    return cachedQt;
}

/**
 * Solve the polynomial expression Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz +
 * 2Gx + 2Hy + 2Iz from the provided points.
 * 
 * @param points
 *            the points that will be fit to the polynomial expression.
 * @return the solution vector to the polynomial expression.
 */
RealVector EllipsoidFit::solveSystem(const std::vector<mag_point_t>& points)
{
	// determine the number of points
	int numPoints = points.size();

	// the design matrix
	// size: numPoints x 9
	RealMatrix d(numPoints, RealVector(9, 0.0));

	// Fit the ellipsoid in the form of
	// Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz
	std::vector<mag_point_t>::const_iterator pi(points.begin());
	for (int i = 0; i < numPoints; i++)
	{
		mag_point_t poffset = *pi;
		poffset.x -= offset.x;
		poffset.y -= offset.y;
		poffset.z -= offset.z;

		double xx = pow((double) poffset.x, 2);
		double yy = pow((double) poffset.y, 2);
		double zz = pow((double) poffset.z, 2);
		double xy = 2 * (poffset.x * poffset.y);
		double xz = 2 * (poffset.x * poffset.z);
		double yz = 2 * (poffset.y * poffset.z);
		double x = 2 * poffset.x;
		double y = 2 * poffset.y;
		double z = 2 * poffset.z;

		d[i][0] = xx;
		d[i][1] = yy;
		d[i][2] = zz;
		d[i][3] = xy;
		d[i][4] = xz;
		d[i][5] = yz;
		d[i][6] = x;
		d[i][7] = y;
		d[i][8] = z;

		++pi;
	}

	// solve the normal system of equations
	// v = (( d' * d )^-1) * ( d' * ones.mapAddToSelf(1));

	// Multiply: d' * d
	RealMatrix dtd = d.transpose().multiply(d);

	// Create a vector of ones.
	RealVector ones(numPoints, 1.0);

	// Multiply: d' * ones.mapAddToSelf(1)
	RealVector dtOnes(d.transpose().operate(ones));

	// Find ( d' * d )^-1
	DecompositionSolver solver = SingularValueDecomposition(dtd).getSolver();
	RealMatrix dtdi = solver.getInverse();

	// v = (( d' * d )^-1) * ( d' * ones.mapAddToSelf(1));
	RealVector v = dtdi.operate(dtOnes);

	return v;
}

/**
 * Create a matrix in the algebraic form of the polynomial Ax^2 + By^2 +
 * Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1.
 * 
 * @param v
 *            the vector polynomial.
 * @return the matrix of the algebraic form of the polynomial.
 */
RealMatrix EllipsoidFit::formAlgebraicMatrix(const RealVector& v)
{
	// a =
	// [ Ax^2 2Dxy 2Exz 2Gx ]
	// [ 2Dxy By^2 2Fyz 2Hy ]
	// [ 2Exz 2Fyz Cz^2 2Iz ]
	// [ 2Gx  2Hy  2Iz  -1  ]
	RealMatrix a(4, RealVector(4, 0.0));

	a[0][0] = v[0];
	a[0][1] = v[3];
	a[0][2] = v[4];
	a[0][3] = v[6];
	a[1][0] = v[3];
	a[1][1] = v[1];
	a[1][2] = v[5];
	a[1][3] = v[7];
	a[2][0] = v[4];
	a[2][1] = v[5];
	a[2][2] = v[2];
	a[2][3] = v[8];
	a[3][0] = v[6];
	a[3][1] = v[7];
	a[3][2] = v[8];
	a[3][3] = -1;

	return a;
}

/**
 * Find the center of the ellipsoid.
 * 
 * @param a
 *            the algebraic from of the polynomial.
 * @return a vector containing the center of the ellipsoid.
 */
void EllipsoidFit::setCenter(const RealMatrix& a)
{
	RealMatrix subA = a.getSubMatrix(0, 2, 0, 2);

	for (int q = 0; q < subA.getRowDimension(); q++)
		for (int s = 0; s < subA.getColumnDimension(); s++)
			subA[q][s] *= -1.0;

	RealVector subV = a[3].getSubVector(0, 3);

	// inv (dtd)
	DecompositionSolver solver = SingularValueDecomposition(subA).getSolver();
	RealMatrix subAi = solver.getInverse();

	RealVector op = subAi.operate(subV);

	center.x = op[0];
	center.y = op[1];
	center.z = op[2];
}

/**
 * Translate the algebraic form of the ellipsoid to the center.
 * 
 * @param center
 *            vector containing the center of the ellipsoid.
 * @param a
 *            the algebraic form of the polynomial.
 * @return the center translated form of the algebraic ellipsoid.
 */
RealMatrix EllipsoidFit::translateToCenter(RealMatrix a)
{
	// Form the corresponding translation matrix.
	RealMatrix t = RealMatrix::createRealIdentityMatrix(4);

	RealMatrix centerMatrix(1, 3);	// RealMatrix centerMatrix = new Array2DRowRealMatrix(1, 3);

	centerMatrix[0][0] = center.x;
	centerMatrix[0][1] = center.y;
	centerMatrix[0][2] = center.z;

	t.setSubMatrix(centerMatrix, 3, 0);

	// Translate to the center.
	RealMatrix r = t.multiply(a).multiply(t.transpose());

	return r;
}

void EllipsoidFit::setRadii()
{
	for (int i = 0; i < 3; ++i)
		radii[i] = sqrt(1 / evals[i]);
}

/*
	For some reason, the fit ellipsoid algorithm fails if the surface of the ellipsoid
	gets close to the coordinate center. My math skills are very bad, and I have no way to figure out
	why this happens, but to avoid this, I calculate a simple offset and try to center the ellipsoid
	before I pass the points over to the rest of the calculation. Then at the very and of the
	fitEllipsoid() function, I re-apply the offsets, and everything is hunky dory.
*/
void EllipsoidFit::calcOffsets(const std::vector<mag_point_t>& points)
{
	if (points.empty())
		return;

	Point<int>	pmin, pmax;

	pmin.x = points.front().x;
	pmin.y = points.front().y;
	pmin.z = points.front().z;

	pmax = pmin;

	for (std::vector<mag_point_t>::const_iterator pit(points.begin() + 1); pit < points.end(); pit++)
	{
		if (pmin.x > pit->x)		pmin.x = pit->x;
		if (pmin.y > pit->y)		pmin.y = pit->y;
		if (pmin.z > pit->z)		pmin.z = pit->z;

		if (pmax.x < pit->x)		pmax.x = pit->x;
		if (pmax.y < pit->y)		pmax.y = pit->y;
		if (pmax.z < pit->z)		pmax.z = pit->z;
	}

	offset.x = (pmin.x + pmax.x) / 2;
	offset.y = (pmin.y + pmax.y) / 2;
	offset.z = (pmin.z + pmax.z) / 2;
}

/**
 * Fit points to the polynomial expression Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz
 * + 2Fyz + 2Gx + 2Hy + 2Iz = 1 and determine the center and radii of the
 * fit ellipsoid.
 * 
 * @param points
 *            the points to be fit to the ellipsoid.
 */
void EllipsoidFit::fitEllipsoid(const std::vector<mag_point_t>& points)
{
	// calc the centering offsets
	calcOffsets(points);

	// Fit the points to Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1 and solve the system.
	// v = (( d' * d )^-1) * ( d' * ones.mapAddToSelf(1));
	RealVector v = solveSystem(points);

	// Form the algebraic form of the ellipsoid.
	RealMatrix a = formAlgebraicMatrix(v);

	// Find the center of the ellipsoid.
	setCenter(a);

	// Translate the algebraic form of the ellipsoid to the center.
	RealMatrix r = translateToCenter(a);

	// Generate a submatrix of r.
	RealMatrix subr = r.getSubMatrix(0, 2, 0, 2);

	// subr[i][j] = subr[i][j] / -r[3][3]).
	double divr = -r[3][3];
	int i, j;
	for (i = 0; i < subr.getRowDimension(); i++)
		for (j = 0; j < subr.getRowDimension(); j++)
			subr[i][j] /= divr;		// subr.setEntry(i, j, subr.getEntry(i, j) / divr);

	// Get the eigenvalues and eigenvectors.
	EigenDecomposition ed(subr, 0);
	RealVector& ev = ed.getRealEigenvalues();
	for (i = 0; i < 3; ++i)
		evals[i] = ev[i];

	for (i = 0; i < 3; ++i)
	{
		RealVector& ev = ed.getEigenvector(i);
		for (j = 0; j < 3; ++j)
			evecs[i][j] = ev[j];
	}

	// Find the radii of the ellipsoid.
	setRadii();

	// fix the radii/eigenvectors order (not needed)
	//fixOrder();

	// re-apply the offsets
	center.x += offset.x;
	center.y += offset.y;
	center.z += offset.z;
}

/**
 * Puts the radii and evals in the correct order (most of the time)
 * This peace of code is written by Petar Pavlovic, found on:
 * http://diydrones.com/forum/topics/magnetometer-soft-and-hard-iron-calibration?commentId=705844%3AComment%3A1511340
 * It turns out that
   a) this does not work most of the time
   b) tests show it is not even necessary

   But I will keep it here for future reference...
 */
void EllipsoidFit::fixOrder()
{
	int or[3];
	int i,j;
		
	or[0] = 0;  // orientation vector, has info which radii is which
	for (i = 1; i < 3; ++i) 
		if (fabs(evecs[0][or[0]]) < fabs(evecs[0][i]))
			or[0] = i;
	
	or[1] = 0;
	for(i = 1; i < 3; ++i) 
		if (fabs(evecs[1][or[1]]) < fabs(evecs[1][i]))
			or[1] = i;

	or[2] = 0;
	for(i = 1; i < 3; ++i) 
		if (fabs(evecs[2][or[2]]) < fabs(evecs[2][i]))
			or[2] = i;

	// did we find what we needed?
	if (or[0] == or[1]  ||  or[1] == or[2]  ||  or[0] == or[2])
		return;

	// get eigenvectors, eigenvalues and the radii in correct order
	double evc[3][3];
	double rad[3];
	double evl[3];
	for(i = 0; i < 3; ++i)
	{
		evc[or[0]][i] = evecs[0][i];
		evc[or[1]][i] = evecs[1][i];
		evc[or[2]][i] = evecs[2][i];

		rad[or[i]] = radii[i];
		evl[or[i]] = evals[i];
	}

	// copy the values back into the member variables
	for (i = 0; i < 3; ++i)
	{
		radii[i] = rad[i];
		evals[i] = evl[i];

		for (j = 0; j < 3; ++j)
			evecs[i][j] = evc[i][j];
	}
}

/**
 * calculates a matrix which puts the magnetometer measurements on a sphere
 * This peace of code is written by Petar Pavlovic, found on:
 * http://diydrones.com/forum/topics/magnetometer-soft-and-hard-iron-calibration?commentId=705844%3AComment%3A1511340
 */
void EllipsoidFit::calcCalibMatrix(double scale)
{
	int i, j, k;

	// calculates the transformation matrix
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			calibMatrix[i][j] = 0;
			for (k = 0; k < 3; ++k)
				calibMatrix[i][j] += evecs[k][i] * evecs[k][j] / radii[k];

			// scale the matrix
			calibMatrix[i][j] *= scale;
		}
	}
}