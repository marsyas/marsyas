import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;


class GlobalSettings {

	// Audio length (in ms)
	static var _audio_length:Float;

	// Follow mode - based on sonic-visualiser behaviour
	// 0 - page
	// 1 - scroll
	// 2 - off
	static var _follow_mode:Int;

	// Maximum zoom
	static var _max_zoom:Int;

	public function new() {
	}

	public static function setDefaults():Void {
		_audio_length = 0.;
		_follow_mode = 1;
		_max_zoom = 4;
	}

	//
	// Getters and setters
	//
 	public static function setAudioLength(f:Float):Void {
		_audio_length = f;
	}

	public static function getAudioLength():Float {
		return _audio_length;
	}

	public static function setFollowMode(i:Int):Void {
		_follow_mode = i;
	}

	public static function getFollowMode():Int {
		return _follow_mode;
	}

	public static function setMaxZoom(i:Int):Void {
		_max_zoom = i;
	}

	public static function getMaxZoom():Int {
		return _max_zoom;
	}


}