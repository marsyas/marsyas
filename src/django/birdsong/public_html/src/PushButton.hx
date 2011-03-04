import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;


class PushButton extends Sprite {

	private var loader:Loader;
	private var active_loader:Loader;
	private var disabled_loader:Loader;
	private var over_loader:Loader;

	private var all_loaded:Bool;

	// _state 
	// -1 -> Button disabled	
	// 0 -> Button inactive	
	// 1 -> Button active          	
	public var _state:Int;  

	var _height : Int;
	var _width  : Int;

	// The name of the image we are using for this button
	// sness - This is also the name we pass back to ControlBar for our name
	public var _image_name : String;

	var over_highlight : Sprite;
	var toggle_highlight : Sprite;

	public function new(x_:Int, y_:Int, width_:Int, height_:Int, image_name_:String) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;
		_image_name = image_name_;

		_state = 0;

		all_loaded = false;

		// Event Listeners
		addEventListener(MouseEvent.MOUSE_OVER, mouseOver);
		addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
		addEventListener(MouseEvent.CLICK, mouseClick);

		// Add a listener for PUSHBUTTON events so we can stop
		// propagation if we are in the disabled state
  		addEventListener(PushButtonEvent.PUSHBUTTON,pushButtonEventListener);

		// Button image
		loader = new Loader();
		loader.visible = true;
		loader.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		loader.load(new URLRequest("/src/library/" + _image_name + ".png"));

		// Active button image
		active_loader = new Loader();
		active_loader.visible = true;
		active_loader.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		active_loader.load(new URLRequest("/src/library/" + _image_name + "-active.png"));

		// Over button image
		over_loader = new Loader();
		over_loader.visible = true;
		over_loader.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		over_loader.load(new URLRequest("/src/library/" + _image_name + "-over.png"));

		// Disabled button image
		disabled_loader = new Loader();
		disabled_loader.visible = true;
		disabled_loader.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		disabled_loader.load(new URLRequest("/src/library/" + _image_name + "-disabled.png"));

	}

	private function pushButtonEventListener(e:PushButtonEvent):Void {
// 		trace("click = " + e.button_name);
		if (_state == -1) {
			e.stopImmediatePropagation();
		}
	}

 	function mouseOver (e:MouseEvent):Void {
		if (_state == 0 || _state == 1) {
			loader.content.visible = false;
			active_loader.content.visible = false;
			over_loader.content.visible = true;
		}
	}

 	function mouseOut (e:MouseEvent):Void {
		over_loader.content.visible = false;
		redraw();
	}

	private function mouseClick(e:Event):Void {
		dispatchEvent(new PushButtonEvent(PushButtonEvent.PUSHBUTTON,true,false,_image_name));
	}

	private function initListener (e:Event):Void {
		addChild(loader.content);
		addChild(active_loader.content);
		addChild(over_loader.content);
		addChild(disabled_loader.content);
		over_loader.content.visible = false;
		active_loader.content.visible = false;
		disabled_loader.content.visible = false;
		all_loaded = true;
		redraw();
	}

	private function redraw ():Void {
		if (all_loaded) {
			// Hide all the buttons first
			// sness - Hopefully this doesn't cause flicker
			// sness - Not sure if this will do bad things if redrawing when in
			// a mouseover state
			loader.content.visible = false;
			active_loader.content.visible = false;
			disabled_loader.content.visible = false;
			over_loader.content.visible = false;

			if (_state == -1) {
				disabled_loader.content.visible = true;
			} else if (_state == 1) {
				active_loader.content.visible = true;
			} else {
				loader.content.visible = true;
			}
		}
	}

	public function setDisabled():Void {
		setState(-1);
		over_loader.content.visible = false;
		redraw();
	}

	public function setNormal():Void {
		setState(0);
		redraw();
	}

	public function setActive():Void {
		setState(1);
		redraw();
	}

 	public function setState(s:Int):Void {
		_state = s;
 		redraw();
 	}

	public function getState():Int {
		return _state;
	}

}