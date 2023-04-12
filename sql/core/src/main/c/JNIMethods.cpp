#include <iostream>
#include "org_apache_spark_JNI_JNIMethods.h"

JNIEXPORT jint JNICALL Java_org_apache_spark_jni_JNIMethods_nativeMethod(JNIEnv *env, jobject obj, jint value) {
    std::cout << "Native method called with value " << value << std::endl;

    return value;
}

JNIEXPORT jobject JNICALL Java_org_apache_spark_jni_JNIMethods_returnInternalRow(JNIEnv *env, jobject obj1, jobject obj2) {
    return obj2;
}

JNIEXPORT jobject JNICALL Java_org_apache_spark_jni_JNIMethods_returnUnsafeROw(JNIEnv *env, jobject obj1, jobject obj2) {
    return obj2;
}