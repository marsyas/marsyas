import edu.uvic.marsyas.*;

class Test {
	static {
		System.loadLibrary("marsyas");
	}
	public static void main (String [] args){
		System.out.println("Hello World!");
		MarSystemManager msm = new MarSystemManager();

		MarSystem file = msm.create("SoundFileSource","file");
		MarSystem sink = msm.create("AudioSink","sink");
		MarSystem gain = msm.create("Gain", "gain");
		MarSystem pipe = msm.create("Series","pipe");

		pipe.addMarSystem(file);
		pipe.addMarSystem(gain);
		pipe.addMarSystem(sink);

		MarControlPtr filename = pipe.getControl("SoundFileSource/file/mrs_string/filename");
		MarControlPtr notempty = pipe.getControl("SoundFileSource/file/mrs_bool/hasData");
		MarControlPtr iniAudio = pipe.getControl("AudioSink/sink/mrs_bool/initAudio");

		filename.setValue_string("test.ogg");
		iniAudio.setValue_bool(true);

		while (notempty.to_bool()) pipe.tick();
	}
}
