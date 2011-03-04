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

class Annotation extends Sprite {

 	public static var DELETE_ANNOTATION_EVENT:String = "DELETE_ANNOTATION_EVENT";

	var _well:Sprite;
	var _rect:Sprite;

	var _start_extent:AnnotationExtent;
	var _end_extent:AnnotationExtent;

	// Primary properties
	public var _id:Float;
	public var _start_ms:Float;
	public var _end_ms:Float;
	var _name : String;

	public var _index : Int; // The _index of this item in the Annotation

	public var _start:Float;
	public var _end:Float;


	var _height : Float;
	var _width  : Float;

	public var _row : Float; // The row in the Annotator for this Annotation
	public var _y : Float;

	private var _text:TextField;
	private var _format:TextFormat;

	// Has the user already pressed delete in an empty text field?
	private var _last_char_deleted : Bool;

	public var _annotator : Annotator;

	public function new (id_:Int,start_ms_:Float,end_ms_:Float, name_:String, annotator_:Annotator) {
		super();

		_id = id_;
		_start_ms = start_ms_;
		_end_ms = end_ms_;
		_name = name_;
		_annotator = annotator_;

		// Derived properties
		_start = annotator_.ms2pix(_start_ms); 
		_end = annotator_.ms2pix(_end_ms); 
 		_width = _end - _start;
 		_height = annotator_.getHeight();

		// Set a default value for where the label will go
		_y = _height/2 - 1;

		_last_char_deleted = false;

		// A well to hold everything
		_well = new Sprite();
		this.addChild(_well);
		
		_rect = new Sprite();
		_well.addChild(_rect);

		_start_extent = new AnnotationExtent(this,0,_height);
		_well.addChild(_start_extent);

		_end_extent = new AnnotationExtent(this,1,_height);
		_well.addChild(_end_extent);

		_format = new TextFormat();
		_format.font = "Arial";
		_format.size = 12;
 		_format.bold = true;

		// Create a new text field for the title
		_text = new TextField();
		flash.Lib.current.stage.focus = _text;  // Set the focus of the stage to this text field
 		redrawText();
		_well.addChild(_text);

		addEventListener(KeyboardEvent.KEY_DOWN, keyDownListener);
		redraw();
	}

	// The middle of this annotation
	public function centerX():Int {
		return Std.int(_start + ((_end - _start) / 2.0));
	}

	private function initListener (e:Event):Void {
	}

 	public function redraw () {
		// Update the start and end ends based on the current zooming factor
		// in the Annotator
		_start = _annotator.ms2pix(_start_ms); 
		_end = _annotator.ms2pix(_end_ms); 

		_rect.graphics.clear();
		_rect.graphics.lineStyle(1,0x333333);
		_rect.graphics.beginFill(0xAAAAFF,1);
		_rect.graphics.drawRect(_start+10,_y,_end-_start-20,2);

		_start_extent.redraw();
		_end_extent.redraw();

		redrawText();
 	}

	public function setStart(start:Float):Void {
		_start_ms = _annotator.pix2ms(start); 
	}

	public function setEnd(end:Float):Void {
		_end_ms = _annotator.pix2ms(end); 
	}

	function redrawText():Void {
		_text.background = true;
		_text.backgroundColor = 0xffffff;
		_text.border = true;
		_text.borderColor = 0x000000;
		_text.height = 15; 
   		_text.x = (((_end - _start) / 2.0) + _start) - _text.width / 2;
 		_text.y = _y - 18;
		_text.text = _name;
		_text.type = TextFieldType.INPUT;
		_text.autoSize = TextFieldAutoSize.CENTER;
		
		// Add a listener that is called whenever new characters are added
		_text.addEventListener(TextEvent.TEXT_INPUT, textInputListener);
		_text.addEventListener(Event.CHANGE, textChangeListener);

		// The default format for new text
 		_text.defaultTextFormat = _format;

	}

	private function textInputListener(e:TextEvent):Void {
		_name = _text.text;
		_last_char_deleted = false;
		OrcaAnnotator.setUserCurrentlyInputtingText(true);
	}

	private function textChangeListener(e:Event):Void {
		_name = _text.text;
		// Remember when the user deletes the last character, so when they press
		// "backspace" again, we can delete the whole annotation.
		if (e.target.text == "") {
			_last_char_deleted = true;
		}
		OrcaAnnotator.setUserCurrentlyInputtingText(true);
	}

 	function mouseDownListener(e:MouseEvent):Void {
 	}

 	function mouseReleasedListener(e:MouseEvent):Void {
 	}

 	function keyDownListener(e:KeyboardEvent):Void {
		// If the user already deleted the last character and then presses
		//  "backspace", then she wants to remove the entire annotation.
		if (e.keyCode == 8 && _last_char_deleted) {
			dispatchEvent(new Event(Annotation.DELETE_ANNOTATION_EVENT, true));			
		}
		OrcaAnnotator.setUserCurrentlyInputtingText(true);
 	}

	public function to_string():String {
		return Std.string(_id) + "," + Std.string(_start_ms) + "," + Std.string(_end_ms) + "," + _name;
	}

}
