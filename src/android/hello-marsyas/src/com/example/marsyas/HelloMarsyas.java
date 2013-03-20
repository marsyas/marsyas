package com.example.marsyas;

import android.app.Activity;
import android.os.Bundle;
import android.os.CountDownTimer;
//import android.os.Handler;
import android.widget.TextView;

import android.graphics.Color; 
import android.media.AudioFormat; 
import android.media.AudioRecord; 
//import android.os.Bundle; 
import android.util.Log;
//import android.view.MotionEvent; 
//import android.widget.TextView; 

public class HelloMarsyas extends Activity
{

	public AudioRecord audioRecord; 
	public int mSamplesRead; //how many samples read 
	public int buffersizebytes; 
	public int buflen; 
	public int channelConfiguration = AudioFormat.CHANNEL_IN_MONO; 
	public int audioEncoding = AudioFormat.ENCODING_PCM_16BIT; 
	public static short[] buffer; //+-32767 
	public static final int SAMPPERSEC = 8000; //samp per sec 8000, 11025, 22050 44100 or 48000 
	public TextView timeDisplay;

	public int dumpcount = 0;
	public static int[] test_buf; //+-32767    	 

	public class MyCount extends CountDownTimer {
		public MyCount(long millisInFuture, long countDownInterval) {
			super(millisInFuture, countDownInterval);
		}
		public void onFinish() {
			done();
		}
		public void onTick(long millisUntilFinished) {
			trigger();
		}
	}


	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		/* Create a TextView and set its content.
		 * the text is retrieved by calling a native
		 * function.
		 */
		timeDisplay = new TextView(this);
		this.setContentView(timeDisplay);
		setContentView(timeDisplay);

		setupMarsyasNetwork(); // actual marsyas

		MyCount counter = new MyCount(10000, 500);
		counter.start();

		buffersizebytes = AudioRecord.getMinBufferSize(SAMPPERSEC,
				channelConfiguration, audioEncoding); //4096 on ion 
		buffer = new short[buffersizebytes]; 

		buflen=buffersizebytes/2; 
		audioRecord = new AudioRecord(
				android.media.MediaRecorder.AudioSource.MIC, SAMPPERSEC, 
				channelConfiguration, audioEncoding, buffersizebytes); //constructor 
		audioRecord.startRecording(); 
	}

	//-------------------------------------- 
	public void trigger(){ 
		acquire(); 
		dump(); 
	} 

	public void acquire(){ 
		try { 
			mSamplesRead = audioRecord.read(buffer, 0, buffersizebytes);
			for (int i=0; i < 10; i++) {
				System.out.format("%d\t", buffer[i]);
			}
			System.out.println("");
		} catch (Throwable t) { 
			Log.e("AudioRecord", "Recording problem"); 
		} 
	}

	public void dump(){ 
		TextView tv = new TextView(this); 
		setContentView(tv); 
		tv.setTextColor(Color.WHITE); 
		tv.setText("buffersizebytes "+buffersizebytes
				+"   count: "+dumpcount
				+"   buflen: " + mSamplesRead
				+ "\n");
		dumpcount ++;

		tickMarsyasNetwork(buffer, mSamplesRead);
		tv.invalidate(); 
	} 

	public void done(){ 
		audioRecord.stop();
		TextView tv = new TextView(this); 
		setContentView(tv); 
		tv.setTextColor(Color.WHITE); 
		tv.setText("done\n");
		tv.invalidate();
	} 

	/* A native method that is implemented by the
	 * 'hellomarsyas' native library, which is packaged
	 * with this application.
	 */
	public native boolean setupMarsyasNetwork();
	public native boolean tickMarsyasNetwork(short[] buffer2, int numSamples);

	/* this is used to load the 'hellomarsyas' library on application
	 * startup. The library has already been unpacked into
	 * /data/data/com.example.marsyas/lib/libhellomarsyas.so at
	 * installation time by the package manager.
	 */
	static {
		System.loadLibrary("hellomarsyas");
	}
}
