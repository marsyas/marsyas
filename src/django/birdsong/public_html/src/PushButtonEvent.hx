import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;


class PushButtonEvent extends Event {
	// A constant for the "pushbutton" event type
	public static var PUSHBUTTON:String = "pushbutton";

	// The name of the button that triggered the event
	public var button_name:String;

	// Constructor
	public function new (type:String, bubbles:Bool = false, cancelable:Bool = false, button_name:String = "default") {

		// Pass constructor parameters to the superclass constructor
		super(type, bubbles, cancelable);

		// Remember the pushbutton switch's state so it can be accessed within
		// PushbuttonEvent.PUSHBUTTON listeners
		this.button_name = button_name;
	}

	// Every custom event class must override clone()
	public override function clone():Event {
		return new PushButtonEvent(type, bubbles, cancelable, button_name);
	}

	// Every custom event class must override toString().  Note that
	// "eventPhase" is an instance variable relating to the event flow.
	public override function toString():String {
		return formatToString("PushbuttonEvent", "type", "bubbles", "cancelable", "eventPhase", "button_name");
	}
}
