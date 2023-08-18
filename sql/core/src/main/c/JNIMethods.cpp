#include <iostream>
#include "org_apache_spark_JNI_JNIMethods.h"
#include <CL/sycl.hpp>

#if FPGA || FPGA_EMULATOR
#include <sycl/ext/intel/fpga_extensions.hpp>
#endif

using namespace sycl;

// Create an exception handler for asynchronous SYCL exceptions
static auto exception_handler = [](sycl::exception_list e_list) {
  for (std::exception_ptr const &e : e_list) {
    try {
      std::rethrow_exception(e);
    }
    catch (std::exception const &e) {
#if _DEBUG
      std::cout << "Failure" << std::endl;
#endif
      std::terminate();
    }
  }
};

JNIEXPORT jint JNICALL Java_org_apache_spark_JNI_JNIMethods_nativeMethod(JNIEnv *env, jobject obj, jint value) {
    std::cout << "Native method called with value " << value << std::endl;

    return value;
}

JNIEXPORT jobject JNICALL Java_org_apache_spark_JNI_JNIMethods_returnInternalRow(JNIEnv *env, jobject obj1, jobject obj2) {
    jclass cls = (env)->GetObjectClass(obj2);

    // Get the Method ID for method "callback", which takes no arg and return void
    //jmethodID midGetDouble = (env)->GetMethodID(cls, "getDouble", "(I)D");
    //if (NULL == midGetDouble) return obj2;

    //jdouble getDoubleResult = (env)->CallDoubleMethod(obj2, midGetDouble, 16);

    //std::cout << "total amount for this trip is: " << getDoubleResult << std::endl;

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


JNIEXPORT jbooleanArray JNICALL Java_org_apache_spark_JNI_JNIMethods_JNIOneapiCompareGreaterFloatArray(JNIEnv *env, jobject obj1, jfloatArray arr, jfloat jthreshold, jint localEnd, jint batchIdx) {

    // Create device selector for the device of your interest.
    #if FPGA_EMULATOR
      // Intel extension: FPGA emulator selector on systems without FPGA card.
      auto selector = sycl::ext::intel::fpga_emulator_selector_v;
    #elif FPGA_SIMULATOR
      // Intel extension: FPGA simulator selector on systems without FPGA card.
      auto selector = sycl::ext::intel::fpga_simulator_selector_v;
    #elif FPGA_HARDWARE
      // Intel extension: FPGA selector on systems with FPGA card.
      auto selector = sycl::ext::intel::fpga_selector_v;
    #else
      // The default device selector will select the most performant device.
      auto selector = default_selector_v;
    #endif

       queue q(selector, exception_handler);

       float *a;
       float *threshold;
       bool *mask_arr;

       // std::cout << "check nullptr\n";
       if ((a == nullptr) || (mask_arr == nullptr)) {
       if (a != nullptr) free(a, q);
       if (mask_arr != nullptr) free(mask_arr, q);

       std::cout << "Shared memory allocation failure.\n";
       }

       // Initialize input arrays with values from 0 to array_size - 1
       jfloat o_a[localEnd];
       env->GetFloatArrayRegion(arr, 0, localEnd, o_a);

       *threshold = static_cast<float>(jthreshold);

       for (int i=0; i<localEnd; i++) {
          a[i] = static_cast<float>(o_a[i]);
       }


       int *le = new int(static_cast<int>(localEnd));
       int *bi = new int(static_cast<int>(batchIdx));

       auto R = range<1>(le);
       buffer buf_a(a, R);
       buffer buf_threshold(threshold, R);
       buffer buf_mask_arr(mask_arr, R);
       buffer buf_bi(bi, R);

       q.submit([&](handler &h)){
            accessor A(buf_a, h, read_only);
            accessor T(buf_threshold, h, read_only);
            accessor M(buf_mask_arr, h, write_only);
            accessor B(buf_bi, h, read_only);

            h.parallel_for(range<1>(le), [=](auto i){
                M[i] = A[B[0] + i] > T[0];
            });
       });

       e.wait();

       int ret_size = localEnd;
       jboolean j_ret[ret_size];
       for (int i=0; i<ret_size; i++) {
          j_ret[i] = mask_arr[i];
       }

       jbooleanArray results = env->NewBooleanArray(ret_size);
       env->SetBooleanArrayRegion(results, 0, ret_size, j_ret);

       delete le;

       return results;
}

//JNIEXPORT jbooleanArray JNICALL Java_org_apache_spark_JNI_JNIMethods_JNIOneapiCompareGreaterFloatArray(JNIEnv *env, jobject obj1, jfloatArray arr, jfloat jthreshold, jint localEnd, jint batchIdx) {
//
//    // Create device selector for the device of your interest.
//    #if FPGA_EMULATOR
//      // Intel extension: FPGA emulator selector on systems without FPGA card.
//      auto selector = sycl::ext::intel::fpga_emulator_selector_v;
//    #elif FPGA_SIMULATOR
//      // Intel extension: FPGA simulator selector on systems without FPGA card.
//      auto selector = sycl::ext::intel::fpga_simulator_selector_v;
//    #elif FPGA_HARDWARE
//      // Intel extension: FPGA selector on systems with FPGA card.
//      auto selector = sycl::ext::intel::fpga_selector_v;
//    #else
//      // The default device selector will select the most performant device.
//      auto selector = default_selector_v;
//    #endif
//
////       jsize lengthA = env->GetArrayLength(arr);
////
////       if (lengthA == 0) {
////          return NULL;
////       }
//
//
//       queue q(selector, exception_handler);
//
//       // Print out the device information used for the kernel code.
////       std::cout << "Running on device: "
////                << q.get_device().get_info<info::device::name>() << "\n";
////       std::cout << "Array size: " << localEnd << "\n";
//
//       // Create arrays with "array_size" to store input and output data. Allocate
//       // unified shared memory so that both CPU and device can access them.
//       // std::cout << "malloc\n";
//       float *a =         malloc_shared<float>(localEnd, q);
//       float *threshold = malloc_shared<float>(1, q);
//       bool *mask_arr =  malloc_shared<bool>(localEnd, q);
//
//
//       // std::cout << "check nullptr\n";
//       if ((a == nullptr) || (mask_arr == nullptr)) {
//       if (a != nullptr) free(a, q);
//       if (mask_arr != nullptr) free(mask_arr, q);
//
//       std::cout << "Shared memory allocation failure.\n";
//       }
//
//       // Initialize input arrays with values from 0 to array_size - 1
//       // std::cout << "GetInt\n";
//       jfloat o_a[localEnd];
//       env->GetFloatArrayRegion(arr, 0, localEnd, o_a);
//
//       *threshold = static_cast<int>(jthreshold);
//
//       for (int i=0; i<localEnd; i++) {
//          a[i] = static_cast<float>(o_a[i]);
//       }
//
//       // std::cout << "before parallel_for\n";
//       auto e = q.parallel_for(range<1>(localEnd), [=](auto i){
//          // mask_arr[i] = a[i] > *threshold;
////          if (a[i] > *threshold) {
////             mask_arr[i] = 1;
////          } else if (a[i] < *threshold) {
////             mask_arr[i] = -1;
////          } else {
////             mask_arr[i] = 0;
////          }
//            mask_arr[i] = a[batchIdx + i] > *threshold;
//       });
//       e.wait();
//       // std::cout << "after parallel_for\n";
//
//       // std::vector<int> ret_vec;
//
//       // for (int i=0; i<lengthA; i++) {
//       //    if(mask_arr[i]) {
//       //       ret_vec.push_back(a[i]);
//       //    }
//       // }
//
//       // int ret_size = ret_vec.size();
//       int ret_size = localEnd;
//       jboolean j_ret[ret_size];
//       for (int i=0; i<ret_size; i++) {
//        //   j_ret[i] = static_cast<jboolean>(mask_arr[i]);
//          j_ret[i] = mask_arr[i];
//       }
//
//       jbooleanArray results = env->NewBooleanArray(ret_size);
//       env->SetBooleanArrayRegion(results, 0, ret_size, j_ret);
//       // std::cout << "SetInt\n";
//
//
//       return results;
//}