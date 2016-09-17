#pragma once

#include "../dongle/reports.h"
#include "Point.h"

// *********************************************
// This is a translation of the Java Ellipsoid Fit library found here:
// https://github.com/KEOpenSource/EllipsoidFit
// The Java library is, in turn, based on a MATLAB script written by Yury Petrov. It can be found here:
// http://www.mathworks.com/matlabcentral/fileexchange/24693-ellipsoid-fit
// *********************************************

struct RealVector: public std::vector<double>
{
	RealVector()
	{}

	RealVector(const_iterator first, const_iterator last)
		: std::vector<double>(first, last)
	{}

	RealVector(size_t n, const double& val)
		: std::vector<double>(n, val)
	{}

	RealVector getSubVector(int index, int n) const
	{
		return RealVector(begin() + index, begin() + index + n);
	}
};

// this is not really a matrix but a vector of variable size vectors, but it will do...
struct RealMatrix: public std::vector<RealVector>
{
	RealMatrix()
	{}

	RealMatrix(size_t rows, size_t cols)
		: std::vector<RealVector>(rows, RealVector(cols, 0.0))
	{}

	RealMatrix(size_t rows, const RealVector& val)
		: std::vector<RealVector>(rows, val)
	{}

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
	RealMatrix getSubMatrix(int startRow, int endRow, int startColumn, int endColumn) const;

    /**
     * Is this a square matrix?
     * @return true if the matrix is square (rowDimension = columnDimension)
     */
    bool isSquare() const
	{
		if (empty())
			return true;

		return size() == front().size();
	}

    /**
     * Returns the number of rows in the matrix.
     *
     * @return rowDimension
     */
	int getRowDimension() const
	{
		return size();
	}

    /**
     * Returns the number of columns in the matrix.
     *
     * @return columnDimension
     */
	int getColumnDimension() const
	{
		if (empty())
			return 0;

		return front().size();
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
	void setSubMatrix(const RealMatrix& subMatrix, int row, int column);

    /**
     * Returns the result of postmultiplying this by m.
     *
     * @param m    matrix to postmultiply by
     * @return     this * m
     * @throws     IllegalArgumentException
     *             if columnDimension(this) != rowDimension(m)
     */
    RealMatrix multiply(const RealMatrix& m) const;

    /**
     * Returns the transpose of this matrix.
     *
     * @return transpose matrix
     */
    RealMatrix transpose() const;

    /**
     * Returns the result of multiplying this by the vector <code>v</code>.
     *
     * @param v the vector to operate on
     * @return this*v
     * @throws IllegalArgumentException if columnDimension != v.size()
     */
	RealVector operate(const RealVector& v) const;

    /**
     * Returns a {@link RealMatrix} with specified dimensions.
     * <p>The type of matrix returned depends on the dimension. Below
     * 2<sup>12</sup> elements (i.e. 4096 elements or 64&times;64 for a
     * square matrix) which can be stored in a 32kB array, a {@link
     * Array2DRowRealMatrix} instance is built. Above this threshold a {@link
     * BlockRealMatrix} instance is built.</p>
     * <p>The matrix elements are all set to 0.0.</p>
     * @param rows number of rows of the matrix
     * @param columns number of columns of the matrix
     * @return  RealMatrix with specified dimensions
     * @see #createRealMatrix(double[][])
     */
	static RealMatrix createRealIdentityMatrix(int dimension)
	{
		RealMatrix m(dimension, RealVector(dimension, 0.0));		// final RealMatrix m = createRealMatrix(dimension, dimension);

		for (int i = 0; i < dimension; ++i)
			m[i][i] = 1.0;

		return m;
	}

    /**
     * Check if submatrix ranges indices are valid.
     * Rows and columns are indicated counting from 0 to {@code n - 1}.
     *
     * @param m Matrix.
     * @param startRow Initial row index.
     * @param endRow Final row index.
     * @param startColumn Initial column index.
     * @param endColumn Final column index.
     * @throws OutOfRangeException if the indices are invalid.
     * @throws NumberIsTooSmallException if {@code endRow < startRow} or
     * {@code endColumn < startColumn}.
     */
    void checkSubMatrixIndex(size_t startRow, size_t endRow, size_t startColumn, size_t endColumn) const
	{
        checkRowIndex(startRow);
        checkRowIndex(endRow);
        if (endRow < startRow)
		{
			assert(false);
            // throw new NumberIsTooSmallException(LocalizedFormats.INITIAL_ROW_AFTER_FINAL_ROW, endRow, startRow, false);
        }

        checkColumnIndex(startColumn);
        checkColumnIndex(endColumn);
        if (endColumn < startColumn)
		{
			assert(false);
			// throw new NumberIsTooSmallException(LocalizedFormats.INITIAL_COLUMN_AFTER_FINAL_COLUMN, endColumn, startColumn, false);
        }
	}

    /**
     * Check if a row index is valid.
     *
     * @param m Matrix.
     * @param row Row index to check.
     * @throws OutOfRangeException if {@code row} is not a valid index.
     */
    void checkRowIndex(int row) const
	{
        if (row < 0  ||  row >= getRowDimension())
		{
			assert(false);
			/*
            throw new OutOfRangeException(LocalizedFormats.ROW_INDEX,
                                          row, 0, m.getRowDimension() - 1);
										  */
        }
    }

    /**
     * Check if a column index is valid.
     *
     * @param m Matrix.
     * @param column Column index to check.
     * @throws OutOfRangeException if {@code column} is not a valid index.
     */
    void checkColumnIndex(int column) const
	{
        if (column < 0 || column >= getColumnDimension())
		{
			assert(false);
            // throw new OutOfRangeException(LocalizedFormats.COLUMN_INDEX, column, 0, m.getColumnDimension() - 1);
        }
    }

    /**
     * Check if matrices are multiplication compatible
     *
     * @param left Left hand side matrix.
     * @param right Right hand side matrix.
     * @throws DimensionMismatchException if matrices are not multiplication compatible.
     */
    void checkMultiplicationCompatible(const RealMatrix& rhs) const
	{
        if (getColumnDimension() != rhs.getRowDimension())
		{
			assert(false);
            // throw new DimensionMismatchException(left.getColumnDimension(), right.getRowDimension());
        }
    }
};

/**
 * Class handling decomposition algorithms that can solve A &times; X = B.
 * <p>Decomposition algorithms decompose an A matrix has a product of several specific
 * matrices from which they can solve A &times; X = B in least squares sense: they find X
 * such that ||A &times; X - B|| is minimal.</p>
 * <p>Some solvers like {@link LUDecomposition} can only find the solution for
 * square matrices and when the solution is an exact linear solution, i.e. when
 * ||A &times; X - B|| is exactly 0. Other solvers can also find solutions
 * with non-square matrix A and with non-null minimal norm. If an exact linear
 * solution exists it is also the minimal norm solution.</p>
 *
 * @version $Id: DecompositionSolver.java 1244107 2012-02-14 16:17:55Z erans $
 * @since 2.0
 */
class DecompositionSolver
{
private:
	/** Pseudo-inverse of the initial matrix. */
	RealMatrix	pseudoInverse;
	/** Singularity indicator. */
	bool		nonSingular;

public:

    /**
     * Build a solver from decomposed matrix.
     *
     * @param singularValues Singular values.
     * @param uT U<sup>T</sup> matrix of the decomposition.
     * @param v V matrix of the decomposition.
     * @param nonSingular Singularity indicator.
     * @param tol tolerance for singular values
     */
    DecompositionSolver(const RealVector& singularValues, const RealMatrix& uT, const RealMatrix& v, bool nonSingularArg, double tol);

    /**
     * Get the pseudo-inverse of the decomposed matrix.
     *
     * @return the inverse matrix.
     */
    const RealMatrix& getInverse() const
	{
        return pseudoInverse;
    }
};

/**
 * Calculates the compact Singular Value Decomposition of a matrix.
 * <p>
 * The Singular Value Decomposition of matrix A is a set of three matrices: U,
 * &Sigma; and V such that A = U &times; &Sigma; &times; V<sup>T</sup>. Let A be
 * a m &times; n matrix, then U is a m &times; p orthogonal matrix, &Sigma; is a
 * p &times; p diagonal matrix with positive or null elements, V is a p &times;
 * n orthogonal matrix (hence V<sup>T</sup> is also orthogonal) where
 * p=min(m,n).
 * </p>
 * <p>This class is similar to the class with similar name from the
 * <a href="http://math.nist.gov/javanumerics/jama/">JAMA</a> library, with the
 * following changes:</p>
 * <ul>
 *   <li>the {@code norm2} method which has been renamed as {@link #getNorm()
 *   getNorm},</li>
 *   <li>the {@code cond} method which has been renamed as {@link
 *   #getConditionNumber() getConditionNumber},</li>
 *   <li>the {@code rank} method which has been renamed as {@link #getRank()
 *   getRank},</li>
 *   <li>a {@link #getUT() getUT} method has been added,</li>
 *   <li>a {@link #getVT() getVT} method has been added,</li>
 *   <li>a {@link #getSolver() getSolver} method has been added,</li>
 *   <li>a {@link #getCovariance(double) getCovariance} method has been added.</li>
 * </ul>
 * @see <a href="http://mathworld.wolfram.com/SingularValueDecomposition.html">MathWorld</a>
 * @see <a href="http://en.wikipedia.org/wiki/Singular_value_decomposition">Wikipedia</a>
 * @version $Id: SingularValueDecomposition.java 1244107 2012-02-14 16:17:55Z erans $
 * @since 2.0 (changed to concrete class in 3.0)
 */
class SingularValueDecomposition
{
private:
	/** Indicator for transposed matrix. */
	bool		transposed;
	/** max(row dimension, column dimension). */
	int			m;
	/** min(row dimension, column dimension). */
	int			n;

	/** Cached value of U matrix. */
	RealMatrix	cachedU;
	/** Cached value of V matrix. */
	RealMatrix	cachedV;
    /** Cached value of transposed U matrix. */
    RealMatrix	cachedUt;
    /** Cached value of transposed V matrix. */
    RealMatrix	cachedVt;

    /**
     * Tolerance value for small singular values, calculated once we have
     * populated "singularValues".
     **/
	double		tol;

	RealVector	singularValues;

    /**
     * Returns the matrix U of the decomposition.
     * <p>U is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
     * @return the U matrix
     * @see #getUT()
     */
    const RealMatrix& getU()
	{
        // return the cached matrix
        return cachedU;
    }

    /**
     * Returns the transpose of the matrix U of the decomposition.
     * <p>U is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
     * @return the U matrix (or null if decomposed matrix is singular)
     * @see #getU()
     */
    const RealMatrix& getUT()
	{
        if (cachedUt.empty())
            cachedUt = getU().transpose();

        // return the cached matrix
        return cachedUt;
    }

    /**
     * Returns the matrix V of the decomposition.
     * <p>V is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
     * @return the V matrix (or null if decomposed matrix is singular)
     * @see #getVT()
     */
    const RealMatrix& getV()
	{
        // return the cached matrix
        return cachedV;
    }

public:
    /**
     * Calculates the compact Singular Value Decomposition of the given matrix.
     *
     * @param matrix Matrix to decompose.
     */
	SingularValueDecomposition(const RealMatrix& matrix);

	/**
     * Get a solver for finding the A &times; X = B solution in least square sense.
     * @return a solver
     */
    DecompositionSolver getSolver()
	{
        return DecompositionSolver(singularValues, getUT(), getV(), getRank() == m, tol);
    }

    /**
     * Return the effective numerical matrix rank.
     * <p>The effective numerical rank is the number of non-negligible
     * singular values. The threshold used to identify non-negligible
     * terms is max(m,n) &times; ulp(s<sub>1</sub>) where ulp(s<sub>1</sub>)
     * is the least significant bit of the largest singular value.</p>
     * @return effective numerical matrix rank
     */
    int getRank()
	{
        int r = 0;
        for (size_t i = 0; i < singularValues.size(); i++)
            if (singularValues[i] > tol)
                r++;

        return r;
    }

};

/**
 * Class transforming a symmetrical matrix to tridiagonal shape.
 * <p>A symmetrical m &times; m matrix A can be written as the product of three matrices:
 * A = Q &times; T &times; Q<sup>T</sup> with Q an orthogonal matrix and T a symmetrical
 * tridiagonal matrix. Both Q and T are m &times; m matrices.</p>
 * <p>This implementation only uses the upper part of the matrix, the part below the
 * diagonal is not accessed at all.</p>
 * <p>Transformation to tridiagonal shape is often not a goal by itself, but it is
 * an intermediate step in more general decomposition algorithms like {@link
 * EigenDecomposition eigen decomposition}. This class is therefore intended for internal
 * use by the library and is not public. As a consequence of this explicitly limited scope,
 * many methods directly returns references to internal arrays, not copies.</p>
 * @version $Id: TriDiagonalTransformer.java 1244107 2012-02-14 16:17:55Z erans $
 * @since 2.0
 */
class TriDiagonalTransformer
{
private:
    /** Householder vectors. */
    RealMatrix householderVectors;
    /** Main diagonal. */
    RealVector main;
    /** Secondary diagonal. */
    RealVector secondary;
    /** Cached value of Q. */
    RealMatrix cachedQ;
    /** Cached value of Qt. */
    RealMatrix cachedQt;
    /** Cached value of T. */
    RealMatrix cachedT;

public:

	TriDiagonalTransformer()		// not defined in the Java lib
	{}

    /**
     * Build the transformation to tridiagonal shape of a symmetrical matrix.
     * <p>The specified matrix is assumed to be symmetrical without any check.
     * Only the upper triangular part of the matrix is used.</p>
     *
     * @param matrix Symmetrical matrix to transform.
     * @throws NonSquareMatrixException if the matrix is not square.
     */
    TriDiagonalTransformer(const RealMatrix& matrix);

	/**
	  * Transform original matrix to tridiagonal form.
	  * <p>Transformation is done using Householder transforms.</p>
	  */
	void transform();

    /**
     * Get the main diagonal elements of the matrix T of the transform.
     * <p>Note that since this class is only intended for internal use,
     * it returns directly a reference to its internal arrays, not a copy.</p>
     * @return the main diagonal elements of the T matrix
     */
    const RealVector& getMainDiagonalRef()
	{
        return main;
    }

    /**
     * Get the secondary diagonal elements of the matrix T of the transform.
     * <p>Note that since this class is only intended for internal use,
     * it returns directly a reference to its internal arrays, not a copy.</p>
     * @return the secondary diagonal elements of the T matrix
     */
    const RealVector& getSecondaryDiagonalRef()
	{
        return secondary;
    }

    /**
     * Returns the matrix Q of the transform.
     * <p>Q is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
     * @return the Q matrix
     */
    RealMatrix getQ()
	{
        if (cachedQ.empty())
            cachedQ = getQT().transpose();

        return cachedQ;
    }

    /**
     * Returns the transpose of the matrix Q of the transform.
     * <p>Q is an orthogonal matrix, i.e. its transpose is also its inverse.</p>
     * @return the Q matrix
     */
    RealMatrix getQT();
};

/**
 * Calculates the eigen decomposition of a real <strong>symmetric</strong>
 * matrix.
 * <p>The eigen decomposition of matrix A is a set of two matrices:
 * V and D such that A = V &times; D &times; V<sup>T</sup>.
 * A, V and D are all m &times; m matrices.</p>
 * <p>This class is similar in spirit to the <code>EigenvalueDecomposition</code>
 * class from the <a href="http://math.nist.gov/javanumerics/jama/">JAMA</a>
 * library, with the following changes:</p>
 * <ul>
 *   <li>a {@link #getVT() getVt} method has been added,</li>
 *   <li>two {@link #getRealEigenvalue(int) getRealEigenvalue} and {@link #getImagEigenvalue(int)
 *   getImagEigenvalue} methods to pick up a single eigenvalue have been added,</li>
 *   <li>a {@link #getEigenvector(int) getEigenvector} method to pick up a single
 *   eigenvector has been added,</li>
 *   <li>a {@link #getDeterminant() getDeterminant} method has been added.</li>
 *   <li>a {@link #getSolver() getSolver} method has been added.</li>
 * </ul>
 * <p>
 * As of 2.0, this class supports only <strong>symmetric</strong> matrices, and
 * hence computes only real realEigenvalues. This implies the D matrix returned
 * by {@link #getD()} is always diagonal and the imaginary values returned
 * {@link #getImagEigenvalue(int)} and {@link #getImagEigenvalues()} are always
 * null.
 * </p>
 * <p>
 * When called with a {@link RealMatrix} argument, this implementation only uses
 * the upper part of the matrix, the part below the diagonal is not accessed at
 * all.
 * </p>
 * <p>
 * This implementation is based on the paper by A. Drubrulle, R.S. Martin and
 * J.H. Wilkinson "The Implicit QL Algorithm" in Wilksinson and Reinsch (1971)
 * Handbook for automatic computation, vol. 2, Linear algebra, Springer-Verlag,
 * New-York
 * </p>
 * @see <a href="http://mathworld.wolfram.com/EigenDecomposition.html">MathWorld</a>
 * @see <a href="http://en.wikipedia.org/wiki/Eigendecomposition_of_a_matrix">Wikipedia</a>
 * @version $Id: EigenDecomposition.java 1244107 2012-02-14 16:17:55Z erans $
 * @since 2.0 (changed to concrete class in 3.0)
 */
class EigenDecomposition
{
private:
    /** Maximum number of iterations accepted in the implicit QL transformation */
	enum {maxIter = 30};

    /** Main diagonal of the tridiagonal matrix. */
    RealVector	main;
    /** Secondary diagonal of the tridiagonal matrix. */
    RealVector	secondary;
    /**
     * Transformer to tridiagonal (may be null if matrix is already
     * tridiagonal).
     */
    TriDiagonalTransformer transformer;
    /** Real part of the realEigenvalues. */
    RealVector realEigenvalues;
    /** Imaginary part of the realEigenvalues. */
    RealVector imagEigenvalues;
    /** Eigenvectors. */
    RealMatrix	eigenvectors;
    /** Cached value of V. */
    RealMatrix cachedV;
    /** Cached value of D. */
    RealMatrix cachedD;
    /** Cached value of Vt. */
    RealMatrix cachedVt;

public:

    /**
     * Calculates the eigen decomposition of the given symmetric matrix.
     *
     * @param matrix Matrix to decompose. It <em>must</em> be symmetric.
     * @param splitTolerance Dummy parameter (present for backward
     * compatibility only).
     * @throws NonSymmetricMatrixException if the matrix is not symmetric.
     * @throws MaxCountExceededException if the algorithm fails to converge.
     */
	EigenDecomposition(const RealMatrix& matrix, double splitTolerance);

    /**
     * Gets a copy of the real parts of the eigenvalues of the original matrix.
     *
     * @return a copy of the real parts of the eigenvalues of the original matrix.
     *
     * @see #getD()
     * @see #getRealEigenvalue(int)
     * @see #getImagEigenvalues()
     */
    RealVector getRealEigenvalues()
	{
		return realEigenvalues;
    }

    /**
     * Gets a copy of the i<sup>th</sup> eigenvector of the original matrix.
     *
     * @param i Index of the eigenvector (counting from 0).
     * @return a copy of the i<sup>th</sup> eigenvector of the original matrix.
     * @see #getD()
     */
    RealVector getEigenvector(size_t i)
	{
        return eigenvectors[i];
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
    bool isSymmetric(const RealMatrix& matrix, bool raiseException);

    /**
     * Transforms the matrix to tridiagonal form.
     *
     * @param matrix Matrix to transform.
     */
    void transformToTridiagonal(const RealMatrix& matrix)
	{
        // transform the matrix to tridiagonal
        transformer = TriDiagonalTransformer(matrix);
        main = transformer.getMainDiagonalRef();
        secondary = transformer.getSecondaryDiagonalRef();
    }

    /**
     * Find eigenvalues and eigenvectors (Dubrulle et al., 1971)
     *
     * @param householderMatrix Householder matrix of the transformation
     * to tridiagonal form.
     */
	void findEigenVectors(const RealMatrix& householderMatrix);
};

/**
 * Determines the center, radii, eigenvalues and eigenvectors of the ellipse
 * using an algorithm from Yury Petrov's Ellipsoid Fit MATLAB script. The
 * algorithm fits points from an ellipsoid to the polynomial expression Ax^2 +
 * By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1. The polynomial
 * expression is then solved and the center and radii of the ellipse are
 * determined.
 * 
 * Caveat Emptor: The polynomial expression not guaranteed to be one of an
 * ellipsoid. It could result in any quadric (hyperboloid, paraboloid, etc). If
 * the data is to sparse, the values of the eigenvectors could be reversed
 * resulting in a fit that is not an ellipsoid.
 * 
 * Apache License Version 2.0, January 2004.
 * 
 * @author Kaleb
 * @version 1.0
 * @see http://www.mathworks.com/matlabcentral/fileexchange/24693-ellipsoid-fit
 * 
 */
struct EllipsoidFit
{
	Point<double>	center;
	Point<int>		offset;
	double			radii[3];
	double			evecs[3][3];
	double			evals[3];

	// the matrix which transforms raw magnetometer readings onto a unit sphere
	double			calibMatrix[3][3];

	/**
	 * Solve the polynomial expression Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz +
	 * 2Gx + 2Hy + 2Iz from the provided points.
	 * 
	 * @param points
	 *            the points that will be fit to the polynomial expression.
	 * @return the solution vector to the polynomial expression.
	 */
	RealVector solveSystem(const std::vector<mag_point_t>& points);

	/**
	 * Create a matrix in the algebraic form of the polynomial Ax^2 + By^2 +
	 * Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1.
	 * 
	 * @param v
	 *            the vector polynomial.
	 * @return the matrix of the algebraic form of the polynomial.
	 */
	RealMatrix formAlgebraicMatrix(const RealVector& v);

	/**
	 * Find the center of the ellipsoid.
	 * 
	 * @param a
	 *            the algebraic from of the polynomial.
	 * @return a vector containing the center of the ellipsoid.
	 */
	void setCenter(const RealMatrix& a);

	/**
	 * Translate the algebraic form of the ellipsoid to the center.
	 * 
	 * @param center
	 *            vector containing the center of the ellipsoid.
	 * @param a
	 *            the algebraic form of the polynomial.
	 * @return the center translated form of the algebraic ellipsoid.
	 */
	RealMatrix translateToCenter(RealMatrix a);

	// calculates the radii member
	void setRadii();

	/**
	 * Fit points to the polynomial expression Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz
	 * + 2Fyz + 2Gx + 2Hy + 2Iz = 1 and determine the center and radii of the
	 * fit ellipsoid.
	 * 
	 * @param points
	 *            the points to be fit to the ellipsoid.
	 */
	void fitEllipsoid(const std::vector<mag_point_t>& points);

	void calcOffsets(const std::vector<mag_point_t>& points);

	/**
	 * puts the radii and evals in the correct order
	 */
	void fixOrder();

	/**
	 * calculates a matrix which puts the magnetometer measurements on a sphere
	 */
	void calcCalibMatrix(double scale);
};
