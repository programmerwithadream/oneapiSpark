#include <stdio.h>
#include <stdlib.h>
#include "org_apache_spark_jni_NativeHelloWorld.h"

JNIEXPORT jint JNICALL Java_org_apache_spark_jni_NativeHelloWorld_nativeMethod(JNIEnv *env, jobject obj, jint value) {
    printf("Native method called with value %d\n", value);

    return value;
}