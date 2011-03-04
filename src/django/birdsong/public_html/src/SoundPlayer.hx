import flash.display.Sprite;
import flash.media.Sound;
import flash.media.SoundChannel;
import flash.media.SoundTransform;
import flash.net.URLRequest;

class SoundPlayer extends Sprite {
	static var sound:Sound;

	static var sc:SoundChannel;
	static var st:SoundTransform;

	var playing : Bool;

	static var current_file : Int;

	public function new() {
		super();

		current_file = 1;

		var song:URLRequest = new URLRequest(OrcaAnnotator._recording_url + "/audio.mp3");

 		sound = new Sound();
 		sound.load(song);

 		st = new SoundTransform();
		
		playing = false;
	}

	public function getLoaded():Int {
		return Std.int(sound.bytesLoaded / sound.bytesTotal * sound.length);
	}

	// The length of the sound file
	public function length():Int {
		return Std.int(sound.length);
	}

	// The length of the sound file as a static function.
	//
	// sness - This is kind of cheating, we should just get it from the length()
	// function above, but it's tricky to get it all the way down to LayerView
	public static function static_length():Float {
		return sound.length;
	}

	// The position that we are playing in the sound file
	public function getPosition():Int {
  		if (sc != null && sound.bytesTotal != 0 && sound.bytesLoaded == sound.bytesTotal) {
  			return Std.int(sc.position);
  		} else {
 			return 0;
  		}
	}

	public function setPosition(i:Int):Void {
		play(i);
	}

	//
	// Transport controls
	//
	public function play(position:Int):Void {
		sc = sound.play(position);
		playing = true;
	}

	public function stop():Void {
		if (playing == true) {
			sc.stop();
		}
		playing = false;
	}

	//
	// Volume
	//
	public function getVolume():Float {
		return sc.soundTransform.volume;
	}

	public function setVolume(v:Float):Void {
		st.volume = v;
		sc.soundTransform = st;
	}

}
