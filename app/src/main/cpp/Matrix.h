#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
public:
	typedef struct {
		float x;
		float y;
	} GPointf2;
	typedef struct {
		float x;
		float y;
		float z;
	} GPointf3;
	typedef struct {
		int x;
		int y;
	} GPoint;
	typedef struct {
		int x;
		int y;
		int z;
	} GPoint3;
	typedef struct {
		int left;
		int top;
		int right;
		int bottom;
	} GRect;
	typedef struct {
		float left;
		float top;
		float right;
		float bottom;
	} GRectf;
public:
	static void LoadIdentity(float mat[4][4]);
    static void preTranslateMatrix(float x, float y, float z, float iomat[4][4]);
    static void preTranslateMatrix(float x, float y, float z, const float lhsm[4][4], float retmat[4][4]);
	static void TranslateMatrix(float x, float y, float z, float iomat[4][4]);
	static void TranslateMatrix(float x, float y, float z, const float rhsm[4][4], float retmat[4][4]);
	static void ScaleMatrix(float x, float y, float z, float iomat[4][4]);
	static void ScaleMatrix(float x, float y, float z, const float rhsm[4][4], float retmat[4][4]);
	static void RotateMatrix(int angle, float x, float y, float z, float iomat[4][4]);
	static void RotateMatrix(int angle, float x, float y, float z, const float rhsm[4][4], float retmat[4][4]);
	static void prePerspectiveMatrix2(float viewpoint, float iomat[4][4]);
	static void prePerspectiveMatrix2(float viewpoint, const float lhsm[4][4], float retmat[4][4]);
	static void PerspectiveMatrix2(float viewpoint, float iomat[4][4]);
	static void PerspectiveMatrix2(float viewpoint, const float rhsm[4][4], float retmat[4][4]);
//	static void PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar, float iomat[4][4]);
//	static void PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar, const float lhsm[4][4], float retmat[4][4]);
//	static void SetFrustumMatrix(float left, float right, float bottom, float top, float zNear, float zFar, float retmat[4][4]);
	static void preMultiMatrix( const float rhsm[4][4], float iomat[4][4]);
	static void MultiMatrix( const float lhsm[4][4], float iomat[4][4]);
	static void MultiMatrix( const float lhs[4][4], const float rhs[4][4], float retmat[4][4]);
//	static void TransPoseMatrix(float iomat[4][4]);
//	static void TransPoseMatrix(const float lhsm[4][4], float retmat[4][4]);
	static void ConvRect(const GRect &SrcRect, const float mat[4][4], GRect &DstRect);
	static void ConvRectf(const GRectf &SrcRect, const float mat[4][4], GRectf &DstRect);
	static void ConvVertex(int nPoint, const GPoint *pSrcVertex, const float mat[4][4], GPoint *pDstVertex);
	static void ConvVertexf3(int nPoint, const GPointf3 *pSrcVertex, const float mat[4][4], GPointf3 *pDstVertex);

//	static void TranslatePoints(int x, int y, int nPoint, const GPoint *srcp, GPoint *dstp);
//	static void ScalePoints(int scale, int nPoint, const GPoint *srcp, GPoint *dstp);
//	static void RotateZPoints(int angle, int nPoint, const GPoint *srcp, GPoint *dstp);

//	static void PreTranslateMatrix(int x, int y, int z, float iomat[4][4]);
//	static void PreTranslateMatrix(int x, int y, int z, const float lhs[4][4], float retM[4][4]);
};

#endif //MATRIX_H
