import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.geom.Point;

class VolumeSlider extends Sprite {

	static var _well:Sprite;
	static var _selector:Sprite;

	static var _selector_height;
	static var _selector_width;

// 	static var _selector_height : Int = 20;
// 	static var _selector_width  : Int = 100; 

	static var _down : Bool;

	static var _point : Point;
	static var _local_point : Point;

 	public var _value : Float;
	public var _mute : Bool;

	public var _volume_button:VolumeButton;

	static var _width  : Float;
	static var _height : Float;

	public function new (x_:Int, y_:Int, width_:Int, height_:Int) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;

		_selector_height = _height;
		_selector_width = _width;

 		_value = 1.0;	
		_mute = false;

  		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
  		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
  		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_UP, mouseReleased);

		_point = new Point();
		_local_point = new Point();
		
		_well = new Sprite();
//  		_well.graphics.beginFill(0xEEEEEE,1);
 		_well.graphics.beginFill(0xFFFFFF,1);
		_well.graphics.drawRect(0,0,_selector_width,_selector_height);
		this.addChild(_well);

		_selector = new Sprite();
		this.addChild(_selector);

		_down = false;

		// Volume Button
		_volume_button = new VolumeButton();
 		_volume_button.x = _width + 2;
 		_volume_button.y = -2;
		this.addChild(_volume_button);

		redraw();
	}

 	function redraw () {

 		_selector.graphics.clear();

		// Full selector
//  		_selector.graphics.beginFill(0xFFFFFF,1);
  		_selector.graphics.beginFill(0xAAAAAA,1);
 		_selector.graphics.moveTo(0,_selector_height);
 		_selector.graphics.lineTo(_selector_width,0);
 		_selector.graphics.lineTo(_selector_width,_selector_height);
 		_selector.graphics.lineTo(0,_selector_height);

		// Value
		_selector.graphics.beginFill(0x777777,1);
		_selector.graphics.moveTo(0,_selector_height);
 		_selector.graphics.lineTo(_value * _selector_width,_selector_height - (_value * _selector_height));
 		_selector.graphics.lineTo(_value * _selector_width,_selector_height);

 	}

 	function mouseDown (e:MouseEvent):Void {
// 		trace("e.target=" + e.target);
  		if (e.target == _well || e.target == _selector) {
// 		trace("e.localX=" + e.localX + " _selector_width=" + _selector_width);
			_down = true;
			_value = e.localX / _selector_width;

			if (_value < 0.0) {
				_value = 0.0;
			}
			if (_value > 1.0) {
				_value = 1.0;
			}

			redraw();
  		}
 	}


 	function mouseReleased (e:MouseEvent):Void {
		_down = false;
 	}

 	function mouseMove (e:MouseEvent):Void {
		if (_down == true) {
 			if (e.target == _well || e.target == _selector) {
				_value = e.localX / _selector_width;
 			}
// 			if (e.target == this.stage) {
// 				_point.x = e.localX;
// 				_point.y = e.localY;
// 				_local_point = _well.globalToLocal(_point);
// 				if (_local_point.x > _selector_width) {
// 					_value = 1.0;
// 				} else if (_local_point.x < 0) {
// 					_value = 0.0;
// 				} else {
// 					_value = _local_point.x / _selector_width;
// 				}
// 			}
// 			if (_value < 0.0) {
// 				_value = 0.0;
// 			}
// 			if (_value > 1.0) {
// 				_value = 1.0;
// 			}
			redraw();
		}
	}

	public function getValue():Float {
		if (_volume_button.toggle_state == 0) {
			return 0.0;
		} else {
			return _value;
		}
	}
}
