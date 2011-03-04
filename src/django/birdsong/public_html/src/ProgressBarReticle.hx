import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;
import flash.filters.ColorMatrixFilter;
import flash.geom.Point;


class ProgressBarReticle extends Sprite {

	var _well : Sprite;     // A well to hold everything
	var _box : Shape;       // The reticule box

	var _height : Int;
	var _width  : Int;

	public function new() {
		super();

		// Well to hold everything
		_well = new Sprite();

		// The background of the loaded bar
		_box = new Shape();
		_well.addChild(_box);

		addChild(_well);
	}

	public function redraw():Void {
		
 		_box.graphics.clear();

		_box.graphics.lineStyle(1);
   		_box.graphics.moveTo(0,0);
   		_box.graphics.lineTo(_width,0);
   		_box.graphics.lineTo(_width,_height);
   		_box.graphics.lineTo(0,_height);
   		_box.graphics.lineTo(0,0);
	}

	public function setBox(x_:Int, y_:Int, width_:Int, height_:Int) {

		_width = width_;
		_height = height_;
		x = x_;
		y = y_;

		redraw();

	}


}