#include <iostream>
#include "org_apache_spark_JNI_JNIMethods.h"

JNIEXPORT jint JNICALL Java_org_apache_spark_JNI_JNIMethods_nativeMethod(JNIEnv *env, jobject obj, jint value) {
    std::cout << "Native method called with value " << value << std::endl;

    return value;
}

JNIEXPORT jobject JNICALL Java_org_apache_spark_JNI_JNIMethods_returnInternalRow(JNIEnv *env, jobject obj1, jobject obj2) {
    jclass cls = (env)->GetObjectClass(obj2);

    // Get the Method ID for method "callback", which takes no arg and return void
    jmethodID midGetDouble = (env)->GetMethodID(cls, "getDouble", "(I)D");
    if (NULL == midGetDouble) return obj2;

    jdouble getDoubleResult = (env)->CallDoubleMethod(obj2, midGetDouble, 16);

    std::cout << "total amount for this trip is: " << getDoubleResult << std::endl;

    return obj2;
}

JNIEXPORT jobject JNICALL Java_org_apache_spark_JNI_JNIMethods_returnUnsafeRow(JNIEnv *env, jobject obj1, jobject obj2) {
    return obj2;
}

JNIEXPORT jint JNICALL Java_org_apache_spark_JNI_JNIMethods_JNICompareDouble(JNIEnv *env, jobject obj1, jdouble input, jdouble threshold) {
    if (input > threshold) {
        std::cout << "cpp comparedouble returned 1" << std::endl;
        return 1;
    } else if (input < threshold) {
        std::cout << "cpp comparedouble returned -1" << std::endl;
        return -1;
    } else {
        std::cout << "cpp comparedouble returned 0" << std::endl;
        return 0;
    }
}