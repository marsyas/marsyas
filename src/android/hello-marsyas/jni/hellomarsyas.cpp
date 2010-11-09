#include <jni.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <android/log.h>

#include "marsyas/MarSystemManager.h"

// using namespace std;
using namespace Marsyas;

extern "C" {
  JNIEXPORT jstring JNICALL
  Java_com_example_marsyas_HelloMarsyas_setupMarsyasNetwork( JNIEnv* env, jobject obj );

  JNIEXPORT jstring JNICALL
  Java_com_example_marsyas_HelloMarsyas_tickMarsyasNetwork( JNIEnv* env, jobject obj, jshortArray arr );

  MarSystemManager mng;
  MarSystem* net;
}

JNIEXPORT jstring JNICALL
Java_com_example_marsyas_HelloMarsyas_setupMarsyasNetwork( JNIEnv* env, jobject obj )
{
  const static int numObservations = 5;
  const static int numSamples = 1;
  
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("RealvecSource", "src"));
  net->addMarSystem(mng.create("Gain", "gain"));

  net->updControl("Gain/gain/mrs_real/gain", 2.0);
  
  net->updControl("mrs_natural/inObservations", numObservations);
  net->updControl("mrs_natural/inSamples", numSamples);

}

JNIEXPORT jstring JNICALL
Java_com_example_marsyas_HelloMarsyas_tickMarsyasNetwork( JNIEnv* env, jobject obj, jshortArray arr )
// Java_com_example_marsyas_HelloMarsyas_tickMarsyasNetwork( JNIEnv* env, jobject obj, jshortArray arr )
// Java_com_example_marsyas_HelloMarsyas_tickMarsyasNetwork( JNIEnv* env, jobject obj)
{
  const static int numObservations = 5;
  const static int numSamples = 1;

  jshort buf[10];
  env->GetShortArrayRegion(arr, 0, 10, buf);

  realvec in;
  in.create(0.0,numObservations,numSamples);
  in(0,0) = (double) buf[0] / 32767.;
  in(1,0) = (double) buf[1] / 32767.;
  in(2,0) = (double) buf[2] / 32767.;
  in(3,0) = (double) buf[3] / 32767.;
  in(4,0) = (double) buf[4] / 32767.;

  net->updControl("RealvecSource/src/mrs_realvec/data", in);

  net->tick();

  realvec out;
  out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  std::ostringstream oss;
  std::string str;

  oss << "Marsyas - Microphone + Gain" << "\n";

  oss << "in = ";
  for (int i = 0; i < 5; i++) {
  	oss << in(i,0) << " ";
  }
  oss << "\n";

  oss << "out = ";
  for (int i = 0; i < 5; i++) {
  	oss << out(i,0) << " ";
  }
  oss << "\n";
  // // oss << *net;

  __android_log_write(ANDROID_LOG_ERROR,"hellomarsyas","Message");
  __android_log_write(ANDROID_LOG_ERROR,"hellomarsyas",oss.str().c_str());

  str += oss.str();
  jstring jstr = env->NewStringUTF(str.c_str());


  // jstring jstr = env->NewStringUTF("Marsyas - SineSource + Gain");
  return jstr;
}
