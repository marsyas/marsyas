//
// Test the integration of PaneWindows, MenuBars, ControlBars and
// AnalysisPopupWindows.
//

import flash.display.Sprite;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.events.KeyboardEvent;
import flash.events.ProgressEvent;
import flash.events.IOErrorEvent;
import flash.media.Sound;
import flash.media.SoundChannel;
import flash.net.URLRequest;
import flash.external.ExternalInterface;

class OrcaAnnotator
{

	static var _well : Sprite;
	static var _controlbar : ControlBar;
	static var _annotator : Annotator;

	static var _sound_player : SoundPlayer;
	static var _volume_slider : VolumeSlider;
 	static var _time_indicator : TimeIndicator;
	static var _progress_bar : ProgressBar;
	static var _xml_loader : XMLLoader;

	// sness - Shouldn't this be part of SoundPlayer, or perhaps GlobalSettings?
	static var _sound_playing : Bool; // Are we currently playing the sound?

	static var _sound_position : Int; // The current playback position (0 - length_of_song in milliseconds)
	static var _sound_loaded : Int; // The amount of data that has been loaded (0 - length_of_song in milliseconds)

	// sness - Shouldn't this be part of ProgressBar, or perhaps GlobalSettings?
	static var _progress_bar_down : Bool; // Is the mouse down in the progress_bar?

	static var _r : OrcaAnnotator;

	static var _index : Int = 0;

	static var _current_time : Int = 0;

	static var _use_external_interface : Bool;

	static var _user_currently_inputting_text : Bool;

	function new(root) {
	}

	public static var _recording_url : String; // The path to Recording assets
	public static var _annotation_url : String; // The path to Annotation assets
	public static var _prediction_url : String; // The path to Prediction assets
	public static var _visualization_url : String; // The path to Visualization assets

	public static var _recording_id : String; // The ID of the current Recording in the database


	static function main ()
	{
		GlobalSettings.setDefaults();

		// Figure out the prefix of the filename we are looking at
		if (flash.Lib.current.loaderInfo.parameters.recording_url != null) {
			_recording_url = flash.Lib.current.loaderInfo.parameters.recording_url;
			_annotation_url = flash.Lib.current.loaderInfo.parameters.annotation_url;
			_prediction_url = flash.Lib.current.loaderInfo.parameters.prediction_url;
			_visualization_url = flash.Lib.current.loaderInfo.parameters.visualization_url;
			_recording_id = flash.Lib.current.loaderInfo.parameters.recording_id;
			// sness - Hack for now, since at the start we don't know the audio length
			// until we've loaded it, but we need to know it to set the start and
			// end of the Prediction rectangles
			GlobalSettings.setAudioLength(2878920.0);
			_use_external_interface = true;
		} else {
			// If is undefined, we are in the standalone player, so just choose
			// a favorite file for debugging.
			_recording_url = "/assets/recordings/2005/449A";
  			_annotation_url = "/web/annotations/show/2.txt";
 			_prediction_url = "/web/predictions/show/2.txt";
 			_visualization_url = "/assets/visualizations/2005/449A/spectrogram";
			//_visualization_url = "/assets/visualizations/2005/449A/waveform";
			GlobalSettings.setAudioLength(2878920.0);
			_use_external_interface = false;
		}

		// Setup the main Timer object 
		var t = new haxe.Timer(5);
		t.run = onTimer;

		// Listen to events on the stage
		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_DOWN, mainMouseDown);
		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_UP, mainMouseUp);
		flash.Lib.current.stage.addEventListener(KeyboardEvent.KEY_DOWN, mainKeyDown);
		flash.Lib.current.stage.addEventListener(KeyboardEvent.KEY_UP, mainKeyUp);

		// Load the configuration file
		_xml_loader = new XMLLoader();
		flash.Lib.current.addChild(_xml_loader);

		// The SoundPlayer that loads and plays the MP3 file
		_sound_player = new SoundPlayer();
		_sound_playing = false;
		_sound_position = 0;
		_sound_loaded = 0;

		// Is the user currently inputting text into a text box?
		_user_currently_inputting_text = false;

		// Create a well to hold everything
		_well = new Sprite();
		_well.name = "main._well";

		// The control bar
   		_controlbar = new ControlBar(130,5,1000,30);
//   		_controlbar = new ControlBar(0,5,1000,30);
 		_well.addChild(_controlbar);

		// Listen for control bar events
		_controlbar.addEventListener(ControlBar.REWINDSTART_EVENT,rewindstartEventListener);
		_controlbar.addEventListener(ControlBar.REWIND_EVENT,rewindEventListener);
		_controlbar.addEventListener(ControlBar.PLAYPAUSE_EVENT,playpauseEventListener);
		_controlbar.addEventListener(ControlBar.FFWD_EVENT,ffwdEventListener);
		_controlbar.addEventListener(ControlBar.FFWDEND_EVENT,ffwdendEventListener);
		_controlbar.addEventListener(ControlBar.ZOOMIN_EVENT,zoomInEventListener);
		_controlbar.addEventListener(ControlBar.ZOOMOUT_EVENT,zoomOutEventListener);
		_controlbar.addEventListener(ControlBar.ZOOMFULL_EVENT,zoomFullEventListener);
		_controlbar.addEventListener(ControlBar.ZOOMSELECTION_EVENT,zoomSelectionEventListener);
		_controlbar.addEventListener(ControlBar.ANNOTATION_EVENT,annotationEventListener);
		_controlbar.addEventListener(ControlBar.INFO_EVENT,infoEventListener);
		_controlbar.addEventListener(ControlBar.SAVE_EVENT,saveEventListener);

		// The annotator
//  		var ann_x:Int = 200;
 		var ann_x:Int = 0;
		var ann_y:Int = 30;
		var ann_width:Int = 800;
		var ann_height:Int = 150;
		_annotator = new Annotator(ann_x,ann_y,ann_width,ann_height,_visualization_url,_sound_player);
 		_well.addChild(_annotator);

		// The progress bar
//    		_progress_bar = new ProgressBar(200,180,600,40,_sound_player);
		_progress_bar = new ProgressBar(0,200,600,40,_sound_player);
   		_well.addChild(_progress_bar);

		// Listen for progress bar events
		_progress_bar.addEventListener(ProgressBar.NEWPOSITION_EVENT,newPositionListener);

   		// The volume slider
//    		_volume_slider = new VolumeSlider(840,200,100,20);
		_volume_slider = new VolumeSlider(670,200,100,20);
  		_well.addChild(_volume_slider);

  		// The time indicator
//    		_time_indicator = new TimeIndicator(860,180,100,20);
		_time_indicator = new TimeIndicator(670,220,100,10);
  		_well.addChild(_time_indicator);

		flash.Lib.current.addChild(_well);

		if (_use_external_interface) {
			ExternalInterface.addCallback("visualizationChange", visualizationChange);
			ExternalInterface.addCallback("followModeChange", followModeChange);
		}

	}

	private static function newPositionListener(e:Event):Void {
		_sound_player.stop();
// 		_sound_position = _progress_bar.getPosition();
		setPosition(_progress_bar.getPosition());
// 		playSound();
	}
 
	private static function rewindstartEventListener(e:Event):Void {
		doRewindStart();
	}

	private static function rewindEventListener(e:Event):Void {
		doRewind(30000);
	}

	private static function playpauseEventListener(e:Event):Void {
		doPlayPause();
	}

	private static function ffwdEventListener(e:Event):Void {
		doFfwd(30000);
	}

	static private function ffwdendEventListener(e:Event):Void {
		doFfwdEnd();
	}

	static private function zoomInEventListener(e:Event):Void {
		_annotator.zoomIn();
	}

	static private function zoomOutEventListener(e:Event):Void {
		_annotator.zoomOut();
	}

	static private function zoomFullEventListener(e:Event):Void {
		_annotator.zoomFull();
	}

	static private function zoomSelectionEventListener(e:Event):Void {
		_annotator.zoomSelection();
	}

	static private function annotationEventListener(e:Event):Void {
		_annotator.newAnnotation();
	}

	static private function infoEventListener(e:Event):Void {
		trace("info");
	}

	static private function saveEventListener(e:Event):Void {
		setUserCurrentlyInputtingText(false);
		if (_use_external_interface) {
			saveAnnotations();
		}
	}

	static private function mainMouseDown(e:Event):Void {
	}

	static private function mainMouseUp(e:Event):Void {
	}

	static public function saveAnnotations():Void {
		if (!getUserCurrentlyInputtingText()) {
			_controlbar.activateSaveButton();
			haxe.Timer.delay(deactivateSaveButton,500);
			_annotator.saveAnnotations();
			flash.Lib.current.stage.focus = null;
		}
	}
	static private function mainKeyDown(e:KeyboardEvent):Void {
//  		trace("key= " + e.keyCode);
		if (e.keyCode == 83) {  // "s"
			if (_use_external_interface) {
				saveAnnotations();
			}
		}
		
		// Change follow mode
		if (e.keyCode == 81) { // "q"
// 			trace("follow mode = 0 (page)");
			GlobalSettings.setFollowMode(0);
		}
		if (e.keyCode == 87) { // "w"
// 			trace("follow mode = 1 (scroll)");
			GlobalSettings.setFollowMode(1);
		}
		if (e.keyCode == 69) { // "e"
// 			trace("follow mode = 2 (off)");
			GlobalSettings.setFollowMode(2);
		}

		// Spectrogram or waveform
		if (e.keyCode == 90) { // "z"
			_visualization_url = "/assets/visualizations/2005/449A/waveform";
			_annotator.reloadVisualization();
		}
		if (e.keyCode == 88) { // "x"
			_visualization_url = "/assets/visualizations/2005/449A/spectrogram";
			_annotator.reloadVisualization();
		}

		// Ffwd and rewind
		if (e.keyCode == 37) { // "back arrow"
			doRewind(30000);
		}

		if (e.keyCode == 39) { // "forward arrow"
			doFfwd(30000);
		}


		if (e.keyCode == 192) { // "backtick"
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
			trace(".");
		}

		if (e.keyCode == 18) { // "alt"
			_annotator.tracer();
		}

		if (e.keyCode == 13) {  // "return"
			setUserCurrentlyInputtingText(false);
			saveAnnotations();
			flash.Lib.current.stage.focus = null;
		}
	}


	static private function mainKeyUp(e:KeyboardEvent):Void {
	}


	private static function redraw ():Void {
	}

	/***********************************************************************/
	/*  The main coordinating function that looks at the status of all the */
	/*  objects and updates the other objects based on the current state.  */
	/***********************************************************************/
	static function onTimer() {
		// Current position in the song
		if (_sound_playing) {
			_sound_position = _sound_player.getPosition();
		}
		_sound_loaded = _sound_player.getLoaded();

 		// Volume
 		if (_sound_playing) {
 			_sound_player.setVolume(_volume_slider.getValue());
 		}

 		// Time indicator
 		_time_indicator.value = _sound_position;
 		if (_progress_bar._mouse_down) {
 			_time_indicator.value = _progress_bar.getPosition();
 		}
 		_time_indicator.redraw();

 		// Progress bar
		if (!_progress_bar._mouse_down) {
			_progress_bar.setPosition(_sound_position);
		}
 		_progress_bar._loaded = _sound_loaded;
		_progress_bar.redraw();

		if (_sound_position > _sound_player.length()) {
			stopSound();
			_sound_position = 0;
		}

		// Update the ProgressBar reticules based on the x position of each of
		// the LayerViews.
		//
		// sness - This seems to me to be inefficient.  Could we instead do this
		// when the view in the Annotator changes?
  		_progress_bar.setReticle(_annotator.getViewStart_ms(),_annotator.getViewEnd_ms());

		// Update the position of the annotator for the different kinds of
		// follow playback modes
		_annotator.redraw();

 		// Every 1000 ticks, check to see if we need to save the annotations
 		if (_current_time % 500 == 0) {
 			if (_annotator.getSaved() == false) {
				if (_use_external_interface) {
					saveAnnotations();
				}
 			}
 		}

		_current_time += 1;
	}

	static function deactivateSaveButton():Void {
		_controlbar.deactivateSaveButton();
	}

	//////////////////////////////////////////////////////////////////////////////////
	//
	// Song interaction
	//
	//////////////////////////////////////////////////////////////////////////////////

 	public static function playSound():Void {
		_sound_player.play(_sound_position);
		_sound_playing = true;
	}

	public static function stopSound():Void {
		_sound_player.stop();
		_sound_playing = false;
	}
	

	// Get the current playback position in ms
	// 
	// sness - Right now we just get the value from _progress_bar, which I think
	// is the right place to store it.
	static public function getPosition():Int {
		if (_progress_bar != null) {
			_sound_position = _progress_bar.getPosition();
		}

		return _sound_position;
	}

	// Set the current playback position in ms
	static public function setPosition(s:Int, ?from_annotator : Bool = false):Void {
		_sound_position = s;
		_progress_bar.setPosition(_sound_position);
		if (!from_annotator) {
			_annotator.setPosition(_sound_position);
		}
		if (_sound_playing) {
			_sound_player.stop();
			_sound_player.play(_sound_position);
		}
	}


	//////////////////////////////////////////////////////////////////////////////////
	//
	// Transport functions
	//
	//////////////////////////////////////////////////////////////////////////////////

	static public function doPlayPause():Void {
		if (_sound_playing) {
  			stopSound();
		} else {
  			playSound();
		}
	}


	static public function doRewindStart():Void {
		_sound_player.stop();
		_sound_position = 0;
	}

	static public function doRewind(v:Int):Void {
		_sound_player.stop();
		_sound_position -= v;
	}

	static public function doFfwd(v:Int):Void {
		_sound_player.stop();
		_sound_position += v;
	}

	static public function doFfwdEnd():Void {
		_sound_player.stop();
		_sound_position = _sound_player.length();
	}


	//////////////////////////////////////////////////////////////////////////////////
	//
	// Javascript callback functions
	//
	//////////////////////////////////////////////////////////////////////////////////

	// sness - Remember to add an ExternalInterface.addCallback() to the main loop 
	// for all new functions you add here

	//
	static public function visualizationChange(s:String):Void {
		_visualization_url = s;
		_annotator.reloadVisualization();
	}

	static public function followModeChange(s:String):Void {
		GlobalSettings.setFollowMode(Std.parseInt(s));
	}

	static public function setUserCurrentlyInputtingText(b:Bool):Void {
		_user_currently_inputting_text = b;
	}

	static public function getUserCurrentlyInputtingText():Bool {
		return _user_currently_inputting_text;
	}



}


