#include <cmath>
#include <string.h>
#include <android/log.h>
#include "Matrix.h"

void Matrix::LoadIdentity(float mat[4][4]) {
	memset(mat, 0x00, sizeof(float[4][4]));
	mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0f;
	return;
}

void Matrix::preTranslateMatrix(float x, float y, float z, float iomat[4][4]) {
    const float lhs[4][4] = {
            iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
            iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
            iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
            iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
    };
    Matrix::preTranslateMatrix(x, y, z, lhs, iomat);
    return;
}

void Matrix::preTranslateMatrix(float x, float y, float z, const float lhsm[4][4], float retmat[4][4]) {
    const float translateM[4][4] = {
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1,
    };
    Matrix::MultiMatrix(lhsm, translateM, retmat);
    return;
}

void Matrix::TranslateMatrix(float x, float y, float z, float iomat[4][4]) {
	const float rhs[4][4] = {
		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::TranslateMatrix(x, y, z, rhs, iomat);
	return;
}

void Matrix::TranslateMatrix(float x, float y, float z, const float rhsm[4][4], float retmat[4][4]) {
	const float translateM[4][4] = {
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1,
	};
	Matrix::MultiMatrix(translateM, rhsm, retmat);
	return;
}

void Matrix::ScaleMatrix(float x, float y, float z, float iomat[4][4]) {
	const float rhsm[4][4] = {
		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::ScaleMatrix(x, y, z, rhsm, iomat);
	return;
}

void Matrix::ScaleMatrix(float x, float y, float z, const float rhsm[4][4], float retmat[4][4]) {
	const float scaleM[4][4] = {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1,
	};
	Matrix::MultiMatrix(scaleM, rhsm, retmat);
	return;
}

void Matrix::RotateMatrix(int angle, float x, float y, float z, float iomat[4][4]) {
	const float rhs[4][4] = {
		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::RotateMatrix(angle, x, y, z, rhs, iomat);
	return;
}

#define PI 3.141592653589793
void Matrix::RotateMatrix(int angle, float x, float y, float z, const float rhsm[4][4], float retmat[4][4]) {
	double theta = angle*PI/180;
	/* x軸回転 */
	float sx = std::sin(theta*x);
	float cx = std::cos(theta*x);
	const float xrotateM[4][4] = {
		1,  0,   0, 0,
		0, cx, -sx, 0,
		0, sx,  cx, 0,
		0,  0,   0, 1,
	};
	Matrix::MultiMatrix(xrotateM, rhsm, retmat);

	/* y軸回転 */
	float sy = std::sin(theta*y);
	float cy = std::cos(theta*y);
	const float yrotateM[4][4] = {
		 cy,  0, sy, 0,
		  0,  1,  0, 0,
		-sy,  0, cy, 0,
		  0,  0,  0, 1,
	};
	Matrix::MultiMatrix((const float(*)[4])yrotateM, retmat);

	/* z軸回転 */
	float sz = std::sin(theta*z);
	float cz = std::cos(theta*z);
	const float zrotateM[4][4] = {
		cz, -sz, 0, 0,
	    sz,  cz, 0, 0,
		 0,   0, 1, 0,
		 0,   0, 0, 1,
	};
	Matrix::MultiMatrix((const float(*)[4])zrotateM, retmat);
	return;
}

//void Matrix::PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar, float iomat[4][4]) {
//	const float lhs[4][4] = {
//		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
//		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
//		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
//		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
//	};
//	Matrix::PerspectiveMatrix(fovy, aspect, zNear, zFar, lhs, iomat);
//	return;
//}

//void Matrix::PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar, const float lhsm[4][4], float retmat[4][4]) {
//	double top = zNear * std::tan(fovy * M_PI / 360.0);
//	double bottom = -top;
//	double left = bottom * aspect;
//	double right = top * aspect;
//	float retFrustumMatrix[4][4];
//	Matrix::SetFrustumMatrix((float)left, (float)right, (float)bottom, (float)top, (float)zNear, (float)zFar, retFrustumMatrix);
//	Matrix::MultiMatrix((const float(*)[4])retFrustumMatrix, lhsm, retmat);
//	return;
//}

void Matrix::prePerspectiveMatrix2(float viewpoint, float iomat[4][4]) {
    const float lhs[4][4] = {
            iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
            iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
            iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
            iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
    };
    Matrix::prePerspectiveMatrix2(viewpoint, lhs, iomat);
    return;
}
void Matrix::prePerspectiveMatrix2(float viewpoint, const float lhsm[4][4], float retmat[4][4]) {
    const float perspectiveM[4][4] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 0,
            0, 0, -1/viewpoint, 1,
    };
    Matrix::MultiMatrix(lhsm, perspectiveM, retmat);
    return;
}

void Matrix::PerspectiveMatrix2(float viewpoint, float iomat[4][4]) {
	const float rhs[4][4] = {
		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::PerspectiveMatrix2(viewpoint, rhs, iomat);
	return;
}
void Matrix::PerspectiveMatrix2(float viewpoint, const float rhsm[4][4], float retmat[4][4]) {
	const float perspectiveM[4][4] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 0,
		0, 0, -1/viewpoint, 1,
	};
	Matrix::MultiMatrix(perspectiveM, rhsm, retmat);
	return;
}

//void Matrix::SetFrustumMatrix(float left, float right, float bottom, float top, float near, float far, float retmat[4][4]) {
//	float r_width  = 1.0f / (right - left);
//	float r_height = 1.0f / (top - bottom);
//	float r_depth  = 1.0f / (near - far);
//	float x = 2.0f * (near * r_width);
//	float y = 2.0f * (near * r_height);
//	float A = (right + left) * r_width;
//	float B = (top + bottom) * r_height;
//	float C = (far + near) * r_depth;
//	float D = 2.0f * (far * near * r_depth);
//
//	retmat[0][0] = x;		retmat[0][1] = 0.0f;	retmat[0][2] = 0.0f;	retmat[0][3] = 0.0f;
//	retmat[1][0] = 0.0f;	retmat[1][1] = y;		retmat[1][2] = 0.0f;	retmat[1][3] = 0.0f;
//	retmat[2][0] = A;		retmat[2][1] = B;		retmat[2][2] = C;		retmat[2][3] = -1.0f;
//	retmat[3][0] = 0.0f;	retmat[3][1] = 0.0f;	retmat[3][2] = D;		retmat[3][3] = 0.0f;
//}

void Matrix::preMultiMatrix( const float rhsm[4][4], float iomat[4][4]) {
	const float lhsm[4][4] = {
			iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
			iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
			iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
			iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::MultiMatrix(lhsm, rhsm, iomat);
}

void Matrix::MultiMatrix( const float lhsm[4][4], float iomat[4][4]) {
	const float rhsm[4][4] = {
		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
	};
	Matrix::MultiMatrix(lhsm, rhsm, iomat);
	return;
}

void Matrix::MultiMatrix( const float lhs[4][4], const float rhs[4][4], float retmat[4][4]) {
    retmat[0][0] = lhs[0][0]*rhs[0][0] + lhs[0][1]*rhs[1][0] + lhs[0][2]*rhs[2][0] + lhs[0][3]*rhs[3][0];
    retmat[0][1] = lhs[0][0]*rhs[0][1] + lhs[0][1]*rhs[1][1] + lhs[0][2]*rhs[2][1] + lhs[0][3]*rhs[3][1];
    retmat[0][2] = lhs[0][0]*rhs[0][2] + lhs[0][1]*rhs[1][2] + lhs[0][2]*rhs[2][2] + lhs[0][3]*rhs[3][2];
    retmat[0][3] = lhs[0][0]*rhs[0][3] + lhs[0][1]*rhs[1][3] + lhs[0][2]*rhs[2][3] + lhs[0][3]*rhs[3][3];

    retmat[1][0] = lhs[1][0]*rhs[0][0] + lhs[1][1]*rhs[1][0] + lhs[1][2]*rhs[2][0] + lhs[1][3]*rhs[3][0];
    retmat[1][1] = lhs[1][0]*rhs[0][1] + lhs[1][1]*rhs[1][1] + lhs[1][2]*rhs[2][1] + lhs[1][3]*rhs[3][1];
    retmat[1][2] = lhs[1][0]*rhs[0][2] + lhs[1][1]*rhs[1][2] + lhs[1][2]*rhs[2][2] + lhs[1][3]*rhs[3][2];
    retmat[1][3] = lhs[1][0]*rhs[0][3] + lhs[1][1]*rhs[1][3] + lhs[1][2]*rhs[2][3] + lhs[1][3]*rhs[3][3];

    retmat[2][0] = lhs[2][0]*rhs[0][0] + lhs[2][1]*rhs[1][0] + lhs[2][2]*rhs[2][0] + lhs[2][3]*rhs[3][0];
    retmat[2][1] = lhs[2][0]*rhs[0][1] + lhs[2][1]*rhs[1][1] + lhs[2][2]*rhs[2][1] + lhs[2][3]*rhs[3][1];
    retmat[2][2] = lhs[2][0]*rhs[0][2] + lhs[2][1]*rhs[1][2] + lhs[2][2]*rhs[2][2] + lhs[2][3]*rhs[3][2];
    retmat[2][3] = lhs[2][0]*rhs[0][3] + lhs[2][1]*rhs[1][3] + lhs[2][2]*rhs[2][3] + lhs[2][3]*rhs[3][3];

    retmat[3][0] = lhs[3][0]*rhs[0][0] + lhs[3][1]*rhs[1][0] + lhs[3][2]*rhs[2][0] + lhs[3][3]*rhs[3][0];
    retmat[3][1] = lhs[3][0]*rhs[0][1] + lhs[3][1]*rhs[1][1] + lhs[3][2]*rhs[2][1] + lhs[3][3]*rhs[3][1];
    retmat[3][2] = lhs[3][0]*rhs[0][2] + lhs[3][1]*rhs[1][2] + lhs[3][2]*rhs[2][2] + lhs[3][3]*rhs[3][2];
    retmat[3][3] = lhs[3][0]*rhs[0][3] + lhs[3][1]*rhs[1][3] + lhs[3][2]*rhs[2][3] + lhs[3][3]*rhs[3][3];

	return;
}

//void Matrix::TransPoseMatrix(float iomat[4][4]) {
//	const float rhsm[4][4] = {
//		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
//		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
//		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
//		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
//	};
//	Matrix::TransPoseMatrix(rhsm, iomat);
//	return;
//}

//void Matrix::TransPoseMatrix(const float lhsm[4][4], float retmat[4][4]) {
//	float tmp[] = {
//		lhsm[0][0],lhsm[1][0],lhsm[2][0],lhsm[3][0],
//		lhsm[0][1],lhsm[1][1],lhsm[2][1],lhsm[3][1],
//		lhsm[0][2],lhsm[1][2],lhsm[2][2],lhsm[3][2],
//		lhsm[0][3],lhsm[1][3],lhsm[2][3],lhsm[3][3]
//	};
//	memcpy(retmat, tmp, sizeof(float[4][4]));
//	return;
//}

void Matrix::ConvRectf(const GRectf &SrcRect, const float mat[4][4], GRectf &DstRect) {
	const GPointf3 SrcVertex[4] = {{SrcRect.left, SrcRect.top,0},{SrcRect.right, SrcRect.top,0},{SrcRect.right, SrcRect.bottom,0},{SrcRect.left, SrcRect.bottom,0}};
	GPointf3 DstVertex[4] = {0};
	Matrix::ConvVertexf3(4, SrcVertex, mat, DstVertex);
	DstRect.left	= DstVertex[0].x;
	DstRect.right	= DstVertex[1].x;
	DstRect.top		= DstVertex[0].y;
	DstRect.bottom	= DstVertex[2].y;
	return;
}

void Matrix::ConvRect(const GRect &SrcRect, const float mat[4][4], GRect &DstRect) {
	const GPoint SrcVertex[4] = {{SrcRect.left, SrcRect.top},{SrcRect.right, SrcRect.top},{SrcRect.right, SrcRect.bottom},{SrcRect.left, SrcRect.bottom}};
	GPoint DstVertex[4] = {0};
	Matrix::ConvVertex(4, SrcVertex, mat, DstVertex);
	DstRect.left	= DstVertex[0].x;
	DstRect.right	= DstVertex[1].x;
	DstRect.top		= DstVertex[0].y;
	DstRect.bottom	= DstVertex[2].y;
	return;
}

void Matrix::ConvVertex(int nPoint, const GPoint *pSrcVertex, const float mat[4][4], GPoint *pDstVertex) {
	for(size_t lpct = 0; lpct < nPoint; lpct++) {
//		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa pDstVertex[%d].x(%d)=pSrcVertex[%d].x(%d)*mat[0][0](%f) + pSrcVertex[%d].y(%d)*mat[1][0](%f) + mat[3][0](%f)", lpct, pDstVertex[lpct].x, lpct,pSrcVertex[lpct].x, mat[0][0], lpct,pSrcVertex[lpct].y, mat[1][0],mat[3][0]);
//		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa pDstVertex[%d].y(%d)=pSrcVertex[%d].x(%d)*mat[0][1](%f) + pSrcVertex[%d].y(%d)*mat[1][1](%f) + mat[3][1](%f)", lpct, pDstVertex[lpct].y, lpct,pSrcVertex[lpct].x, mat[0][1], lpct,pSrcVertex[lpct].y, mat[1][1],mat[3][1]);
		pDstVertex[lpct].x = pSrcVertex[lpct].x*mat[0][0] + pSrcVertex[lpct].y*mat[0][1] + mat[0][3];
		pDstVertex[lpct].y = pSrcVertex[lpct].x*mat[1][0] + pSrcVertex[lpct].y*mat[1][1] + mat[1][3];
	}
	return;
}

void Matrix::ConvVertexf3(int nPoint, const GPointf3 *pSrcVertex, const float mat[4][4], GPointf3 *pDstVertex) {
	for(size_t lpct = 0; lpct < nPoint; lpct++) {
		pDstVertex[lpct].x = pSrcVertex[lpct].x*mat[0][0] + pSrcVertex[lpct].y*mat[0][1] + pSrcVertex[lpct].z*mat[0][2] + mat[0][3];
		pDstVertex[lpct].y = pSrcVertex[lpct].x*mat[1][0] + pSrcVertex[lpct].y*mat[1][1] + pSrcVertex[lpct].z*mat[1][2] + mat[1][3];
		pDstVertex[lpct].z = pSrcVertex[lpct].x*mat[2][0] + pSrcVertex[lpct].y*mat[2][1] + pSrcVertex[lpct].z*mat[2][2] + mat[2][3];

		float w = pSrcVertex[lpct].x*mat[3][0] + pSrcVertex[lpct].y*mat[3][1] + pSrcVertex[lpct].z*mat[3][2] + mat[3][3];

		pDstVertex[lpct].x /= w;
		pDstVertex[lpct].y /= w;
		pDstVertex[lpct].z /= w;
	}
	return;
}

//void Matrix::TranslatePoints(int x, int y, int nPoint, const GPoint *srcp, GPoint *dstp) {
//	for(int lpct = 0; lpct < nPoint; lpct++) {
//		dstp[lpct].x = srcp[lpct].x + x;
//		dstp[lpct].y = srcp[lpct].y + y;
//	}
//	return;
//}

//void Matrix::ScalePoints(int scale, int nPoint, const GPoint *srcp, GPoint *dstp) {
//	for(int lpct = 0; lpct < nPoint; lpct++) {
//		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa srcp[%d]=(%d,%d)", lpct, srcp[lpct].x, srcp[lpct].y);
//		dstp[lpct].x = srcp[lpct].x * scale;
//		dstp[lpct].y = srcp[lpct].y * scale;
//	}
//	return;
//}

//void Matrix::RotateZPoints(int angle, int nPoint, const GPoint *srcp, GPoint *dstp) {
//	double theta = angle*PI/180;
//	float sinval = std::sin(theta);
//	float cosval = std::cos(theta);
//
//	for(int lpct = 0; lpct < nPoint; lpct++) {
////		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa dstp[%d].x(%d)=cosval(%f)*srcp[%d].x(%d) + (-sinval(%f))*srcp[%d].y(%d)", lpct, dstp[lpct].x, cosval, lpct, srcp[lpct].x, -sinval, lpct, srcp[lpct].y);
////		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa dstp[%d].y(%d)=sinval(%f)*srcp[%d].x(%d) +   cosval(%f) *srcp[%d].y(%d)", lpct, dstp[lpct].y, sinval, lpct, srcp[lpct].x,  cosval, lpct, srcp[lpct].y);
//		dstp[lpct].x = cosval*srcp[lpct].x + (-sinval)*srcp[lpct].y;
//		dstp[lpct].y = sinval*srcp[lpct].x +   cosval *srcp[lpct].y;
//	}
//	return;
//}

//void Matrix::PreTranslateMatrix(int x, int y, int z, float iomat[4][4]) {
//	const float lhs[4][4] = {
//		iomat[0][0],iomat[0][1],iomat[0][2],iomat[0][3],
//		iomat[1][0],iomat[1][1],iomat[1][2],iomat[1][3],
//		iomat[2][0],iomat[2][1],iomat[2][2],iomat[2][3],
//		iomat[3][0],iomat[3][1],iomat[3][2],iomat[3][3],
//	};
//	Matrix::PreTranslateMatrix(x, y, z, lhs, iomat);
//	return;
//}

//void Matrix::PreTranslateMatrix(int x, int y, int z, const float lhsm[4][4], float retmat[4][4]) {
//	const float translateM[4][4] = {
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		(float)x, (float)y, (float)z, 1,
//	};
//	Matrix::MultiMatrix(lhsm, translateM, retmat);
//	return;
//}
