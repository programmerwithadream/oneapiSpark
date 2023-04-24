#include <iostream>
#include "org_apache_spark_JNI_JNIMethods.h"
#include <sycl/sycl.hpp>

#if FPGA || FPGA_EMULATOR
#include <sycl/ext/intel/fpga_extensions.hpp>
#endif

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


JNIEXPORT jboolean[] JNICALL Java_org_apache_spark_JNI_JNIMethods_JNIOneapiCompareGreaterFloatArray(JNIEnv *env, jobject obj1, jfloat[] arr, jfloat jthreshold, jint localEnd, jint batchIdx) {

    #if FPGA_EMULATOR
       // Intel extension: FPGA emulator selector on systems without FPGA card.
       ext::intel::fpga_emulator_selector d_selector;
       #elif FPGA
       // Intel extension: FPGA selector on systems with FPGA card.
       ext::intel::fpga_selector d_selector;
       #else
       // The default device selector will select the most performant device.
       auto d_selector{default_selector_v};
       #endif

       jsize lengthA = env->GetArrayLength(ja);

       if (lengthA == 0) {
          return NULL;
       }


       queue q(d_selector, exception_handler);

       // Print out the device information used for the kernel code.
       std::cout << "Running on device: "
                << q.get_device().get_info<info::device::name>() << "\n";
       std::cout << "Array size: " << lengthA << "\n";

       // Create arrays with "array_size" to store input and output data. Allocate
       // unified shared memory so that both CPU and device can access them.
       // std::cout << "malloc\n";
       int *a =         malloc_shared<float>(lengthA, q);
       int *threshold = malloc_shared<float>(1, q);
       int *mask_arr =  malloc_shared<bool>(lengthA, q);


       // std::cout << "check nullptr\n";
       if ((a == nullptr) || (mask_arr == nullptr)) {
       if (a != nullptr) free(a, q);
       if (mask_arr != nullptr) free(mask_arr, q);

       std::cout << "Shared memory allocation failure.\n";
       }

       // Initialize input arrays with values from 0 to array_size - 1
       // std::cout << "GetInt\n";
       jfloat o_a[lengthA];
       env->GetFloatArrayRegion(ja, 0, lengthA, o_a);

       *threshold = static_cast<int>(jthreshold);

       for (int i=0; i<lengthA; i++) {
          a[i] = static_cast<int>(o_a[i]);
       }

       // std::cout << "before parallel_for\n";
       auto e = q.parallel_for(range<1>(lengthA), [=](auto i){
          // mask_arr[i] = a[i] > *threshold;
//          if (a[i] > *threshold) {
//             mask_arr[i] = 1;
//          } else if (a[i] < *threshold) {
//             mask_arr[i] = -1;
//          } else {
//             mask_arr[i] = 0;
//          }
            mask_arr[i] = a[i] > *threshold;
       });
       e.wait();
       // std::cout << "after parallel_for\n";

       // std::vector<int> ret_vec;

       // for (int i=0; i<lengthA; i++) {
       //    if(mask_arr[i]) {
       //       ret_vec.push_back(a[i]);
       //    }
       // }

       // int ret_size = ret_vec.size();
       int ret_size = lengthA;
       jint j_ret[ret_size];
       for (int i=0; i<ret_size; i++) {
          j_ret[i] = static_cast<jint>(mask_arr[i]);
       }

       jfloatArray results = env->NewFloatArray(ret_size);
       env->SetIntArrayRegion(results, 0, ret_size, j_ret);
       // std::cout << "SetInt\n";


       return results;
}

