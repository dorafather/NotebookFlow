#include "JniApp.h"
#include <jni.h>

extern JavaVM * g_jvm;
extern jobject g_callbackObj;
extern jmethodID g_onFlowEventMethod;

namespace nsUtil
{
JniApp::JniApp(){}
JniApp::~JniApp(){}

void JniApp::ACTION(QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _msg)
{
	if(g_jvm == NULL || g_callbackObj == NULL) return;

	// Flow 엔진 자신의 워커 스레드(QTHREAD, RUNFLOW 내부 RUN()이 생성)에서
	// 호출되므로 호출마다 JVM에 attach 필요 - 이미 attach된 스레드에 다시
	// 호출해도 안전(기존 JNIEnv*를 그대로 반환). 이 워커 스레드는 프로세스
	// 생존 기간 내내 유지되는 엔진 전용 스레드라 detach는 하지 않는다
	// (2026-10-01, Android JNI 래퍼 1차 프로토타입).
	JNIEnv * env = NULL;
	if(g_jvm->AttachCurrentThread(&env,NULL) != JNI_OK || env == NULL) return;

	jstring jJson = env->NewStringUTF((const char *)_msg.STR());
	env->CallVoidMethod(g_callbackObj, g_onFlowEventMethod, jJson);
	env->DeleteLocalRef(jJson);
}
}
