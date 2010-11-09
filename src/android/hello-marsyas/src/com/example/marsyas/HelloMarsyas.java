package com.example.marsyas;

import android.app.Activity;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.widget.TextView;

import android.app.Activity; 
import android.graphics.Color; 
import android.media.AudioFormat; 
import android.media.AudioRecord; 
import android.os.Bundle; 
import android.util.Log;
import android.view.MotionEvent; 
import android.widget.TextView; 

public class HelloMarsyas extends Activity
{
	
	public AudioRecord audioRecord; 
	public int mSamplesRead; //how many samples read 
	public int buffersizebytes; 
	public int buflen; 
	public int channelConfiguration = AudioFormat.CHANNEL_CONFIGURATION_MONO; 
	public int audioEncoding = AudioFormat.ENCODING_PCM_16BIT; 
	public static short[] buffer; //+-32767 
	public static final int SAMPPERSEC = 8000; //samp per sec 8000, 11025, 22050 44100 or 48000 

	public static int[] test_buf; //+-32767    	 
	
	public class MyCount extends CountDownTimer {
        public MyCount(long millisInFuture, long countDownInterval) {
          super(millisInFuture, countDownInterval);
        }
        public void onFinish() {
          timeDisplay.setText("Done!");
        }
        public void onTick(long millisUntilFinished) {
        	trigger();
        	//timeDisplay.setText( tickMarsyasNetwork() );
           //timeDisplay.setText("Left: " + millisUntilFinished);
        }
       }
	
	 TextView timeDisplay;
	 
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
	    
	    setupMarsyasNetwork();
	    
        //TextView  tv = new TextView(this);
        //tv.setText( stringFromJNI() );
        //tv.setText( "test" );
        //setContentView(tv);
        
        MyCount counter = new MyCount(1000000, 1000);
        counter.start();
    
        buffersizebytes = AudioRecord.getMinBufferSize(SAMPPERSEC,channelConfiguration,audioEncoding); //4096 on ion 
        buffer = new short[buffersizebytes]; 

		// sness
		test_buf = new int[512];
		for (int i = 0; i < 512; i++) {
			test_buf[i] = i*32767;
		}

        buflen=buffersizebytes/2; 
        audioRecord = new AudioRecord(android.media.MediaRecorder.AudioSource.MIC,SAMPPERSEC, 
        channelConfiguration,audioEncoding,buffersizebytes); //constructor 
        trigger(); 
        
    }
    
  //-------------------------------------- 
    public void trigger(){ 
    	acquire(); 
    	dump(); 
    } 

    public void acquire(){ 
    	try { 
    		audioRecord.startRecording(); 
    		mSamplesRead = audioRecord.read(buffer, 0, buffersizebytes); 
    		audioRecord.stop(); 
  	  } catch (Throwable t) { 
  		  Log.e("AudioRecord", "Recording Failed"); 
  	  } 
    } 

    public void dump(){ 
   	 TextView tv = new TextView(this); 
   	 setContentView(tv); 
   	 tv.setTextColor(Color.WHITE); 
   	 tv.setText("buffersizebytes "+buffersizebytes+"\n"); 
   	 //for(int i = 0; i < 256; i++){ 
   	//	 tv.append(" "+buffer[i]); 
   	 //} 

   	 tv.append(tickMarsyasNetwork(buffer));
   	 //tv.append(tickMarsyasNetwork());
   	 tv.invalidate(); 
    } 
    
    /* A native method that is implemented by the
     * 'hellomarsyas' native library, which is packaged
     * with this application.
     */
    //public native String  stringFromJNI();
    public native String setupMarsyasNetwork();
    //public native String tickMarsyasNetwork(int[] buffer2);
    public native String tickMarsyasNetwork(short[] buffer2);
    // public native String tickMarsyasNetwork();
    
    /* this is used to load the 'hellomarsyas' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.marsyas/lib/libhellomarsyas.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("hellomarsyas");
    }
}
