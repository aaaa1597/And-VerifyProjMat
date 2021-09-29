#include <stdlib.h>
#include <map>
#include <jni.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include "CppSurfaceView.h"
#include "Matrix.h"

std::map<int, CppSurfaceView*> gpSufacesLists;

#ifdef __cplusplus
extern "C" {
#endif

void Java_com_test_ntpdrw_NativeFunc_create(JNIEnv *pEnv, jclass type, jint id) {
    gpSufacesLists[id] = new CppSurfaceView(id);
}

void Java_com_test_ntpdrw_NativeFunc_surfaceCreated(JNIEnv *pEnv, jclass type, jint id, jobject surface) {
    gpSufacesLists[id]->createThread(pEnv, surface);
}

void Java_com_test_ntpdrw_NativeFunc_surfaceChanged(JNIEnv *pEnv, jclass type, jint id, jint width, jint height) {
    gpSufacesLists[id]->isSurfaceCreated = true;
    gpSufacesLists[id]->DspW = width;
    gpSufacesLists[id]->DspH = height;
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "[%d](DspW,DspH)=(%d,%d)", id,gpSufacesLists[id]->DspW, gpSufacesLists[id]->DspH);
}

void Java_com_test_ntpdrw_NativeFunc_surfaceDestroyed(JNIEnv *pEnv, jclass type, jint id) {
    gpSufacesLists[id]->mStatus = CppSurfaceView::STATUS_FINISH;
    gpSufacesLists[id]->destroy();
}

#ifdef __cplusplus
}
#endif

/******************/
/* CppSurfaceView */
/******************/
CppSurfaceView::CppSurfaceView(int id) : mId(id), mStatus(CppSurfaceView::STATUS_NONE), mThreadId(-1) {
}

void CppSurfaceView::createThread(JNIEnv *pEnv, jobject surface) {
    mStatus = CppSurfaceView::STATUS_INITIALIZING;
    mWindow = ANativeWindow_fromSurface(pEnv, surface);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mThreadId, &attr, CppSurfaceView::draw_thread, (void*)this);
}

void *CppSurfaceView::draw_thread(void *pArg) {
    if(pArg == NULL) return NULL;
    CppSurfaceView *pSurface = (CppSurfaceView*)pArg;

    pSurface->initEGL();
    pSurface->initGL();
    pSurface->mStatus = CppSurfaceView::STATUS_DRAWING;

    pSurface->predrawGL();
    struct timespec req = {0, (int)(16.66*1000000)};
    for(;pSurface->mStatus==CppSurfaceView::STATUS_DRAWING;) {

        /* SurfaceCreated()が動作した時は、画面サイズ変更を実行 */
        if(pSurface->isSurfaceCreated) {
            pSurface->isSurfaceCreated = false;
            glViewport(0,0,pSurface->DspW,pSurface->DspH);
            __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "glViewport(%d,%d)", pSurface->DspW, pSurface->DspH);
        }

        /* 通常の描画処理 */
        pSurface->drawGL();
        nanosleep(&req, NULL);
    }

    pSurface->finEGL();

    return NULL;
}

void CppSurfaceView::initEGL() {
	EGLint major, minor;
	mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(mEGLDisplay, &major, &minor);

	/* 設定取得 */
	const EGLint configAttributes[] = {
		EGL_LEVEL, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		/* 透過設定 */
		EGL_ALPHA_SIZE, EGL_OPENGL_BIT,
		/*EGL_BUFFER_SIZE, 32 */  /* ARGB8888用 */
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};

	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(mEGLDisplay, configAttributes, &config, 1, &numConfigs);

	/* context生成 */
	const EGLint contextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* ウィンドウバッファサイズとフォーマットを設定 */
	EGLint format;
	eglGetConfigAttrib(mEGLDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(mWindow, 0, 0, format);

	/* surface生成 */
	mEGLSurface = eglCreateWindowSurface(mEGLDisplay, config, mWindow, NULL);
	if(mEGLSurface == EGL_NO_SURFACE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "surface生成 失敗!!");
		return;
	}

	/* context再生成 */
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* 作成したsurface/contextを関連付け */
	if(eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext) == EGL_FALSE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "surface/contextの関連付け 失敗!!");
		return;
	}

	/* チェック */
	EGLint w,h;
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_WIDTH, &w);
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_HEIGHT,&h);
	glViewport(0,0,w,h);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "glViewport(%d,%d) 1", w,h);
}

void CppSurfaceView::initGL() {
    mProgram = createProgram(0, VERTEXSHADER, FRAGMENTSHADER);
    mLineProgram = createProgram(1, LINEVSHADER, LINEFSHADER);
}

GLuint CppSurfaceView::createProgram(int seqno, const char *vertexshader, const char *fragmentshader) {
    GLuint vhandle = loadShader(GL_VERTEX_SHADER, vertexshader);
    if(vhandle == GL_FALSE) return GL_FALSE;

    GLuint fhandle = loadShader(GL_FRAGMENT_SHADER, fragmentshader);
    if(fhandle == GL_FALSE) return GL_FALSE;

    GLuint programhandle = glCreateProgram();
    if(programhandle == GL_FALSE) {
        checkGlError("glCreateProgram");
        return GL_FALSE;
    }

    glAttachShader(programhandle, vhandle);
    checkGlError("glAttachShader(VERTEX_SHADER)");
    glAttachShader(programhandle, fhandle);
    checkGlError("glAttachShader(FRAGMENT_SHADER)");

    if(seqno == 0)
        glBindAttribLocation( programhandle, seqno, "vPosition");
    else if(seqno == 1)
        glBindAttribLocation( programhandle, seqno, "vLinePosition");

    glLinkProgram(programhandle);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(programhandle, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE) {
        GLint bufLen = 0;
        glGetProgramiv(programhandle, GL_INFO_LOG_LENGTH, &bufLen);
        if(bufLen) {
            char *logstr = (char*)malloc(bufLen);
            glGetProgramInfoLog(mProgram, bufLen, NULL, logstr);
            __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "%d glLinkProgram() Fail!!\n %s", mId, logstr);
            free(logstr);
        }
        glDeleteProgram(programhandle);
        programhandle = GL_FALSE;
    }

    /* 後処理 */
    glDeleteShader(vhandle);
    glDeleteShader(fhandle);

    return programhandle;
}

GLuint CppSurfaceView::loadShader(int shadertype, const char *sourcestring) {
    GLuint shaderhandle = glCreateShader(shadertype);
    if(!shaderhandle) return GL_FALSE;

    glShaderSource(shaderhandle, 1, &sourcestring, NULL);
    glCompileShader(shaderhandle);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen) {
            char *logbuf = (char*)malloc(infoLen);
            if(logbuf) {
                glGetShaderInfoLog(shaderhandle, infoLen, NULL, logbuf);
                __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "%d shader failuer!!\n%s", mId, logbuf);
                free(logbuf);
            }
        }
        glDeleteShader(shaderhandle);
        shaderhandle = GL_FALSE;
    }

    return shaderhandle;
}

void CppSurfaceView::checkGlError(const char *argstr) {
    for(GLuint error = glGetError(); error; error = glGetError())
        __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "%d after %s errcode=%d", mId, argstr, error);
}

void CppSurfaceView::predrawGL() {
    GLuint a_LinePositionHandle = glGetAttribLocation(mLineProgram, "vLinePosition");
    GLenum errcode=glGetError();
    if(errcode!=GL_NO_ERROR){
        printf("%s\n","aaaa");
    }
	static const GLfloat linevertexes[] = {
		/* 上辺 */-1, 1, 0, 1, 1, 0,
		/* 右辺 */ 1, 1, 0, 1,-1, 0,
		/* 左辺 */-1, 1, 0,-1,-1, 0,
		/* 下辺 */-1,-1, 0, 1,-1, 0,

		/* 上辺 */-0.5, 0.5, 0, 0.5, 0.5, 0,
		/* 右辺 */ 0.5, 0.5, 0, 0.5,-0.5, 0,
		/* 左辺 */-0.5, 0.5, 0,-0.5,-0.5, 0,
		/* 下辺 */-0.5,-0.5, 0, 0.5,-0.5, 0,
    };
    glVertexAttribPointer(a_LinePositionHandle, 3, GL_FLOAT, GL_FALSE, 0, linevertexes);
    glEnableVertexAttribArray(a_LinePositionHandle);

    GLuint a_PositionHandle = glGetAttribLocation(mProgram, "vPosition");
    mu_rotMatrixHandle = glGetUniformLocation(mProgram, "u_rotMatrix");
    static const GLfloat vertexes[] = {
          0,   0, 0,
        0.5,   0, 0,
          0,-0.5, 0,

          0,-0.5, 0,
        0.5,   0, 0,
        0.5,-0.5, 0,
    };
    glVertexAttribPointer(a_PositionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertexes);
    glEnableVertexAttribArray(a_PositionHandle);

    mxPos = 0;
    myPos = 0;
    glClearColor(0, 0, 0, 0);
}

void CppSurfaceView::drawGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mLineProgram);
    glLineWidth(10);
    glDrawArrays(GL_LINES, 0, 16);

    if(DspW != 0 && DspH != 0) {
        mxPos += mMoveX;
        myPos += mMoveY;
        if((mxPos > (2*DspW)) || (mxPos < 0)) mMoveX = -mMoveX;
        if((myPos > (2*DspH)) || (myPos < 0)) mMoveY = -mMoveY;
        float translateMatrix[4][4];
        Matrix::LoadIdentity(translateMatrix);
//        Matrix::TranslateMatrix(mxPos/DspW-1, myPos/DspH-1, 0, translateMatrix);
		Matrix::TranslateMatrix(DspW/2.f/DspW, DspH/2.f/DspH, 0, translateMatrix);	/* 1st ドット座標中心へ移動 */
		Matrix::ScaleMatrix(1, -1, 1, translateMatrix);								/* 2nd Y軸反転 */
		Matrix::ScaleMatrix(1.1, 1, 0, translateMatrix);							/* 3rd アスペクト比 */
		Matrix::RotateMatrix(36,0,0,1,translateMatrix);								/* 4th 回転 */

		/* OpenGL用に転置 */
        float glM[] = {
            translateMatrix[0][0],translateMatrix[1][0],translateMatrix[2][0],translateMatrix[3][0],
            translateMatrix[0][1],translateMatrix[1][1],translateMatrix[2][1],translateMatrix[3][1],
            translateMatrix[0][2],translateMatrix[1][2],translateMatrix[2][2],translateMatrix[3][2],
            translateMatrix[0][3],translateMatrix[1][3],translateMatrix[2][3],translateMatrix[3][3]
        };
        glUseProgram(mProgram);
        glUniformMatrix4fv(mu_rotMatrixHandle, 1, false, glM);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    eglSwapBuffers(mEGLDisplay, mEGLSurface);
}

void CppSurfaceView::finEGL() {
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa%d CppSurfaceView::finEGL()", mId);
    ANativeWindow_release(mWindow);
    mWindow = NULL;
}

void CppSurfaceView::destroy() {
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa%d CppSurfaceView::destroy()", mId);
    pthread_join(mThreadId, NULL);
}

CppSurfaceView::~CppSurfaceView() {
}
