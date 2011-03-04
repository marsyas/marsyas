import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;
import flash.text.TextField;
import flash.text.TextFormat;


class TimeIndicator extends Sprite {

	public var value : Float;

	var t:TextField;
	
	var hours:Int;
	var minutes:Int;
	var seconds:Int;
	var milliseconds:Int;

	var hours_string:String;
	var minutes_string:String;
	var seconds_string:String;
	var milliseconds_string:String;

	var format:TextFormat;

	static var _height : Float;
	static var _width  : Float;

	public function new(x_:Int, y_:Int, width_:Int, height_:Int) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;

		value = 0;

		t = new TextField();
		t.width = _width; 
		t.selectable = false;

		addChild(t);

		redraw();
	}

	public function redraw():Void {
		hours = Math.floor(value / 3600000);
		minutes = Math.floor((value - hours * 3600000)/60000);
 		seconds = Math.floor((value - hours * 3600000 - minutes * 60000)/1000);
 		milliseconds = Math.floor(value - hours * 3600000 - minutes * 60000 - seconds * 1000);

		hours_string = Std.string(hours);
		minutes_string = Std.string(minutes);
 		seconds_string = Std.string(seconds);
 		milliseconds_string = Std.string(milliseconds);

		hours_string = StringTools.lpad(hours_string,"0",2);
		minutes_string = StringTools.lpad(minutes_string,"0",2);
 		seconds_string = StringTools.lpad(seconds_string,"0",2);
 		milliseconds_string = StringTools.lpad(milliseconds_string,"0",3);

		t.text = hours_string + ":" + minutes_string + ":" + seconds_string + "." + milliseconds_string;

		format = new TextFormat();
		format.font = "Arial";
		format.size = 15;
		format.bold = true;
		
		t.setTextFormat(format);
	}


}