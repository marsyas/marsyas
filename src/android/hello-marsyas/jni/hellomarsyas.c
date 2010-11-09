#include <jni.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cpu-features.h>
#include "sness-jni-tools.h"

jstring
Java_com_example_marsyas_HelloMarsyas_stringFromJNI( JNIEnv* env,
                                               jobject thiz )
{
    char*  str;
    char buffer[512];

    asprintf(&str, "!sness123!!\n");
    strlcpy(buffer, str, sizeof buffer);
    free(str);

EXIT:
	return (*env)->NewStringUTF(env, buffer);
}
