import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.events.KeyboardEvent;
import flash.events.Event;
import flash.net.URLRequest;
import flash.geom.Point;
import flash.text.TextField;
import flash.events.TextEvent;
import flash.text.TextFieldType;
import flash.text.TextFieldAutoSize;
import flash.text.TextFormat;

class Prediction extends Sprite {

	var _well:Sprite;
	var _rect:Sprite;

	// Primary properties
	public var _id:Float;
	public var _start_ms:Float;
	public var _end_ms:Float;
	var _name : String;
	var _confidence : Float;
	var _color : Int;

	public var _index : Int; // The _index of this item in the Prediction

	public var _start:Float;
	public var _end:Float;
	var _height : Float;
	var _width  : Float;

	// The parent annoator
	public var _annotator : Annotator;

	public function new (id_:Int,start_ms_:Float,end_ms_:Float, name_:String, confidence_:Float, color_:Int, annotator_:Annotator) {
		super();

		_id = id_;
		_start_ms = start_ms_;
		_end_ms = end_ms_;
		_name = name_;
		_confidence = confidence_;
		_color = color_;
		_annotator = annotator_;

		// Derived properties
		_start = annotator_.ms2pix(_start_ms); 
		_end = annotator_.ms2pix(_end_ms); 
 		_width = _end - _start;
 		_height = annotator_.getHeight();

		// A well to hold everything
		_well = new Sprite();
		this.addChild(_well);
		
		_rect = new Sprite();
		_well.addChild(_rect);
		
		redraw();
	}

 	public function redraw () {
		// Update the start and end ends based on the current zooming factor
		// in the Annotator
		_start = _annotator.ms2pix(_start_ms); 
		_end = _annotator.ms2pix(_end_ms); 

		_rect.graphics.clear();
		// Scale down the _confidence by 0.5 to get the alpha value
		//
		// sness - This is just an empirical guess for something that
		// looks good.  Perhaps we should find the maximum confidence
		// of all the predictions and then scale that to an alpha
		// value of 0.5.  
		_rect.graphics.beginFill(_color,(_confidence * 0.5));
		_rect.graphics.drawRect(_start,0,_end-_start,_height);

 	}

	public function setStart(start:Float):Void {
		_start_ms = _annotator.pix2ms(start); 
	}

	public function setEnd(end:Float):Void {
		_end_ms = _annotator.pix2ms(end); 
	}

 	function mouseDownListener(e:MouseEvent):Void {
 	}

 	function mouseReleasedListener(e:MouseEvent):Void {
 	}

 	function keyDownListener(e:KeyboardEvent):Void {
 	}

}
