import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.geom.Point;
import flash.events.Event;

class Slider extends Sprite {

	public static var SLIDER_EVENT:String = "slider_event";

	private var _well:Sprite;
	private var _selector:Sprite;

	private var _selector_height:Float;
	private var _selector_width:Float;

	private var _down : Bool;

	private var _point : Point;
	private var _local_point : Point;

 	public var _value : Float;

	private var _height : Float;
	private var _width  : Float;

	public function new (x_:Int, y_:Int, width_:Int, height_:Int) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;

		_selector_height = _height;
		_selector_width = _width;

 		_value = 1.0;	

 		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
 		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
 		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_UP, mouseReleased);

		_point = new Point();
		_local_point = new Point();
		
		_well = new Sprite();
  		_well.graphics.beginFill(0xEEEEEE,0);
  		_well.graphics.drawRect(0,0,_selector_width,_selector_height);
		this.addChild(_well);

		_selector = new Sprite();
		this.addChild(_selector);

		_down = false;

		redraw();
	}

 	function redraw () {

 		_selector.graphics.clear();

		// Full selector
  		_selector.graphics.beginFill(0xAAAAAA,1);
  		_selector.graphics.moveTo(0,_selector_height);
  		_selector.graphics.lineTo(_selector_width,0);
  		_selector.graphics.lineTo(_selector_width,_selector_height);

 		// Value
 		_selector.graphics.beginFill(0x777777,1);
 		_selector.graphics.moveTo(0,_selector_height);
  		_selector.graphics.lineTo(_value * _selector_width,_selector_height - (_value * _selector_height));
  		_selector.graphics.lineTo(_value * _selector_width,_selector_height);

 	}

 	function mouseDown (e:MouseEvent):Void {
		if (e.target == _well || e.target == _selector) {
			_down = true;
			_value = e.localX / _selector_width;
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
				dispatchEvent(new Event(Slider.SLIDER_EVENT));
				redraw();
			}
			// sness - Fix this later.  We should be able to move anywhere on
			// the screen and convert those coordinates into local coordinates.
			// Not work doing right now.


//  			if (e.target == this.stage) {
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
		}
	}

	public function getValue():Float {
		return _value;
	}
}
