#include <iostream>
#include "org_apache_spark_jni_NativeHelloWorld.h"

JNIEXPORT jint JNICALL Java_org_apache_spark_jni_NativeHelloWorld_nativeMethod(JNIEnv *env, jobject obj, jint value) {
    std::cout << "Native method called with value " << value << std::endl;

    return value;
}