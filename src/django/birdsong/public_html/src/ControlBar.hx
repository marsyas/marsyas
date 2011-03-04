import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;

// sness - This is how you enable and disable the buttons. 
//
// 		rewindstart_button.setDisabled();
// 		rewind_button.setNormal();
// 		playpause_button.setActive();

class ControlBar extends Sprite {

 	public static var REWINDSTART_EVENT:String = "REWINDSTART_EVENT";
 	public static var REWIND_EVENT:String = "REWIND_EVENT";
 	public static var PLAYPAUSE_EVENT:String = "PLAYPAUSE_EVENT";
 	public static var FFWD_EVENT:String = "FFWD_EVENT";
 	public static var FFWDEND_EVENT:String = "FFWDEND_EVENT";
 	public static var ZOOMIN_EVENT:String = "ZOOMIN_EVENT";
 	public static var ZOOMOUT_EVENT:String = "ZOOMOUT_EVENT";
 	public static var ZOOMFULL_EVENT:String = "ZOOMFULL_EVENT";
 	public static var ZOOMSELECTION_EVENT:String = "ZOOMSELECTION_EVENT";
 	public static var ANNOTATION_EVENT:String = "ANNOTATION_EVENT";
 	public static var INFO_EVENT:String = "INFO_EVENT";
 	public static var SAVE_EVENT:String = "SAVE_EVENT";

	var _well : Sprite;
	var _background : Shape;

	private var loader:Loader;

	static var playpause_button:PushButton;
	static var rewind_button:PushButton;
	static var rewindstart_button:PushButton;
	static var ffwd_button:PushButton;
	static var ffwdend_button:PushButton;
	static var zoomin_button:PushButton;
	static var zoomout_button:PushButton;
	static var zoomfull_button:PushButton;
	static var zoomselection_button:PushButton;
	static var annotation_button:PushButton;
	static var info_button:PushButton;
	static var save_button:PushButton;

	var _height : Int;
	var _width  : Int;

	public function new(x_:Int, y_:Int, width_:Int, height_:Int) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;

		// Create a well to hold everything
		_well = new Sprite();

		// Create the background
 		_background = new Shape();
   		_background.graphics.beginFill(0xFFFFFF,1);
//    		_background.graphics.beginFill(0xEEEEFF,1);
   		_background.graphics.moveTo(0,0);
   		_background.graphics.lineTo(_width,0);
   		_background.graphics.lineTo(_width,_height);
   		_background.graphics.lineTo(0,_height);
   		_background.graphics.lineTo(0,0);
 		_well.addChild(_background);

		rewindstart_button = new PushButton(0,0,30,30,"rewindstart");
		_well.addChild(rewindstart_button);
  		rewindstart_button.addEventListener(PushButtonEvent.PUSHBUTTON,rewindStartListener);

		rewind_button = new PushButton(50,0,30,30,"rewind");
		_well.addChild(rewind_button);
  		rewind_button.addEventListener(PushButtonEvent.PUSHBUTTON,rewindListener);

		playpause_button = new PushButton(100,0,30,30,"playpause");
		_well.addChild(playpause_button);
  		playpause_button.addEventListener(PushButtonEvent.PUSHBUTTON,playListener);

		ffwd_button = new PushButton(150,0,30,30,"ffwd");
		_well.addChild(ffwd_button);
  		ffwd_button.addEventListener(PushButtonEvent.PUSHBUTTON,ffwdListener);

		ffwdend_button = new PushButton(200,0,30,30,"ffwdend");
		_well.addChild(ffwdend_button);
  		ffwdend_button.addEventListener(PushButtonEvent.PUSHBUTTON,ffwdEndListener);

		zoomin_button = new PushButton(250,0,30,30,"zoomin");
		_well.addChild(zoomin_button);
  		zoomin_button.addEventListener(PushButtonEvent.PUSHBUTTON,zoomInListener);

		zoomout_button = new PushButton(300,0,30,30,"zoomout");
		_well.addChild(zoomout_button);
  		zoomout_button.addEventListener(PushButtonEvent.PUSHBUTTON,zoomOutListener);

		zoomfull_button = new PushButton(350,0,30,30,"zoomfull");
		_well.addChild(zoomfull_button);
  		zoomfull_button.addEventListener(PushButtonEvent.PUSHBUTTON,zoomFullListener);

		zoomselection_button = new PushButton(400,0,30,30,"zoomselection");
		_well.addChild(zoomselection_button);
  		zoomselection_button.addEventListener(PushButtonEvent.PUSHBUTTON,zoomSelectionListener);

		annotation_button = new PushButton(450,-3,30,30,"annotation");
		_well.addChild(annotation_button);
  		annotation_button.addEventListener(PushButtonEvent.PUSHBUTTON,annotationListener);

		info_button = new PushButton(500,-3,30,30,"info");
		_well.addChild(info_button);
  		info_button.addEventListener(PushButtonEvent.PUSHBUTTON,infoListener);

		save_button = new PushButton(630,-3,30,30,"save");
		_well.addChild(save_button);
  		save_button.addEventListener(PushButtonEvent.PUSHBUTTON,saveListener);

 		rewindstart_button.setNormal();
 		rewind_button.setNormal();
 		playpause_button.setNormal();
 		ffwd_button.setNormal();
 		ffwdend_button.setNormal();
 		zoomin_button.setNormal();
 		zoomout_button.setNormal();
 		zoomfull_button.setNormal();
 		zoomselection_button.setNormal();
 		annotation_button.setNormal();
 		info_button.setNormal();
 		save_button.setNormal();

		this.addChild(_well);

	}

	private function pushButtonClickListener(e:PushButtonEvent):Void {
		// trace("click = " + e.button_name);
	}

	private function rewindStartListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.REWINDSTART_EVENT, true));
	}

	private function rewindListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.REWIND_EVENT, true));
	}

	private function playListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.PLAYPAUSE_EVENT, true));
	}

	private function ffwdListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.FFWD_EVENT, true));
	}

	private function ffwdEndListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.FFWDEND_EVENT, true));
	}

	private function zoomInListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.ZOOMIN_EVENT, true));
	}

	private function zoomOutListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.ZOOMOUT_EVENT, true));
	}

	private function zoomFullListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.ZOOMFULL_EVENT, true));
	}

	private function zoomSelectionListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.ZOOMSELECTION_EVENT, true));
	}

	private function annotationListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.ANNOTATION_EVENT, true));
	}

	private function infoListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.INFO_EVENT, true));
	}

	private function saveListener(e:Event):Void {
 		this.dispatchEvent(new Event(ControlBar.SAVE_EVENT, true));
	}

 	function mouseOver (e:MouseEvent):Void {
	}

 	function mouseOut (e:MouseEvent):Void {
	}

	private function initListener (e:Event):Void {
		redraw();
	}

	private function redraw ():Void {
	}

	public function trace_value():Void {
		// trace("hi");
	}

	public function activateSaveButton():Void {
		save_button.setActive();
	}

	public function deactivateSaveButton():Void {
		save_button.setNormal();
	}

}