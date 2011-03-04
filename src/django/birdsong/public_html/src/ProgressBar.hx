import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;
import flash.filters.ColorMatrixFilter;
import flash.geom.Point;


class ProgressBar extends Sprite {

 	public static var NEWPOSITION_EVENT:String = "NEWPOSITION_EVENT";

	public var _well : Sprite;     // A holder for everything
	private var _slider : Loader;  // The slider indicator
	private var _bar_slider : Loader;  // The bar_slider indicator
	public var _waveform : Loader;     // The waveform picture
	var _loaded_background : Shape; // The background of the loaded bar
	var _loaded_indicator : Shape;  // The progress indicator of the loaded bar

	// A mask to hide any part of the reticles that is outside the ProgressBar
	var _progress_bar_mask : Shape;
	
	public var _reticle : ProgressBarReticle;

	private var _position : Float;  // Position that we are at in the sound file
	public var _loaded : Float;    // Amount of the sound file that is loaded
	private var _slider_ready : Bool;      // Has the slider loader finished loading?
	private var _waveform_ready : Bool;      // Has the waveform loader finished loading?
	private var _ready : Bool;      // Have all the Loaders finished loading?

	public var _mouse_down : Bool; // Is the mouse currently down?
	static var _point : Point;
	static var _local_point : Point;

	static var _width  : Int;
	static var _height : Int;

	private var _image_displayed : Int; // Are we displaying the waveform (0) or spectogram (1)

	private var _sound_player : SoundPlayer;

	public function new(x_:Int, y_:Int, width_:Int, height_:Int, sound_player_:SoundPlayer) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;
		_sound_player = sound_player_;

		_position = 0;
		_loaded = 0;
		_ready = false;
		_image_displayed = 0;

		addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
		addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
  		addEventListener(MouseEvent.MOUSE_UP, mouseUp);

		// Well to hold everything
		_well = new Sprite();

		// Create the mask
 		_progress_bar_mask = new Shape();
   		_progress_bar_mask.graphics.beginFill(0xFFFFFF,1);
   		_progress_bar_mask.graphics.moveTo(0,0);
   		_progress_bar_mask.graphics.lineTo(_width,0);
   		_progress_bar_mask.graphics.lineTo(_width,_height);
   		_progress_bar_mask.graphics.lineTo(0,_height);
   		_progress_bar_mask.graphics.lineTo(0,0);
		_well.addChild(_progress_bar_mask);
		_well.mask = _progress_bar_mask;

		// Slider
		_slider = new Loader();
		_slider.visible = false;
		_slider.contentLoaderInfo.addEventListener(Event.INIT, initSliderListener);
		_slider.load(new URLRequest("/src/library/slider.png"));

		// Waveform
		_waveform = new Loader();
		_waveform.visible = false;
		_waveform.contentLoaderInfo.addEventListener(Event.INIT, initWaveformListener);
		_waveform.load(new URLRequest(OrcaAnnotator._recording_url + "/globalview.jpg"));

		// The background of the loaded bar
		_loaded_background = new Shape();
		_loaded_background.graphics.lineStyle(0);
		_loaded_background.graphics.beginFill(0xFFFFFF,0.2);
		_loaded_background.graphics.drawRect(0,0,_width,40);
		_loaded_background.x = 0;
		_loaded_background.y = 0;
		_well.addChild(_loaded_background);

		// The indicator of the loaded bar
		_loaded_indicator = new Shape();
		_loaded_indicator.graphics.lineStyle(0);
		_loaded_indicator.graphics.beginFill(0xFFEEEE,0.2);
		_loaded_indicator.graphics.drawRect(0,0,1,40);
		_loaded_indicator.x = 0;
		_loaded_indicator.y = 0;
		_well.addChild(_loaded_indicator);

		// The reticles
		_reticle = new ProgressBarReticle();

		addChild(_well);
	}

	private function initSliderListener (e:Event):Void {
		_slider.content.x = -3;
		_slider_ready = true;
		checkIfAllReady();
		redraw();
 	}

	private function initWaveformListener (e:Event):Void {
		_waveform_ready = true;
		checkIfAllReady();
		redraw();
 	}

	function checkIfAllReady():Void {
		if (_slider_ready && _waveform_ready) {
			_ready = true;
			_well.addChild(_waveform.content);
			_well.addChild(_reticle);
			_well.addChild(_slider.content);
		}
	}

	function mouseDown (e:MouseEvent):Void {
		// sness - Not sure why this isn't needed anymore
		// 		if (e.target == _well) {
// 		trace("down");
		_mouse_down = true;
		_position = e.localX / _width * _sound_player.length();
		OrcaAnnotator.setPosition(Std.int(_position));
		redraw();
		// 		}
 	}

 	function mouseUp (e:MouseEvent):Void {
		// trace("mouseup");
		_mouse_down = false;
 		this.dispatchEvent(new Event(ProgressBar.NEWPOSITION_EVENT, true));
 	}

 	function mouseMove (e:MouseEvent):Void {
		if (_mouse_down == true) {
			if (e.target == _well) {
				_position = e.localX / _width * _sound_player.length();
			}
			if (e.target == this.stage) {
				_point.x = e.localX;
				_point.y = e.localY;
				_local_point = _well.globalToLocal(_point);
				if (_local_point.x > _width) {
					_position = _sound_player.length();
				} else if (_local_point.x < 0) {
					_position = 0.0;
				} else {
					_position = _local_point.x / _width * _sound_player.length();
				}
			}
			this.dispatchEvent(new Event(ProgressBar.NEWPOSITION_EVENT, true));
			redraw();
		}
	}

	public function redraw():Void {

		if (_ready) {
 			_slider.content.x = (_position / _sound_player.length()) * _width;
		}

		// sness - Not sure why I have to subtract 1 here
 		_loaded_indicator.width = (_loaded * _width / _sound_player.length()) - 1;

		// Redraw the reticles
		_reticle.redraw();

	}

	public function showSlider():Void {
		if (_ready) {
			_slider.content.visible = true;
		}
	}

	public function showBarSlider():Void {
		if (_ready) {
			_slider.content.visible = false;
		}
	}
	

	// Set left and right edges of the i'th reticule.  The units are
	// SoundPlayer units
	public function setReticle(l_:Float, r_:Float) {

		var l:Float = (l_ / SoundPlayer.static_length()) * _width;
		var r:Float = (r_ / SoundPlayer.static_length()) * _width;

		var rx:Int;
		var ry:Int;
		var rheight:Int;
		var rwidth:Int;

		ry = 5;
		rheight = 30;

		rx = Std.int(l);
		rwidth = Std.int(r - l);
		
//    		trace(" l=" + l + " r=" + r + " rx=" + rx + " ry=" + ry + " rwidth=" + rwidth + " rheight=" + rheight );

		_reticle.setBox(rx,ry,rwidth,rheight);

		
	}

	//
	// Getters and setters
	//

	public function getPosition():Int {
		return Std.int(_position);
	}

	public function setPosition(i:Int):Void {
		_position = i;
	}

}