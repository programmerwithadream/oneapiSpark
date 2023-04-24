package org.apache.spark.JNI;

import org.apache.spark.sql.catalyst.InternalRow;
import org.apache.spark.sql.catalyst.expressions.UnsafeRow;

public class JNIMethods {

    public native int nativeMethod(int value);

    public native InternalRow returnInternalRow(InternalRow row);

    public native UnsafeRow returnUnsafeRow(UnsafeRow row);

    public native int JNICompareDouble(double value, double threshold);

    //pass two ints: columnartorow_localEnd_0, columnartorow_batchIdx_0
    // and an InternalRow: columnartorow_mutableStateArray_2
    public native int[] JNIOneapiCompareGreaterFloatArray(float[] arr, float threshold, int localEnd, int batchIdx);


}
