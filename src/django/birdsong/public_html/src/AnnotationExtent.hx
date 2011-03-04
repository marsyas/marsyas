import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.events.Event;
import flash.net.URLRequest;
import flash.geom.Point;

class AnnotationExtent extends Sprite {

	var _well:Sprite;
	var _rect:Sprite;
	var _drag_icon:Sprite;
	
	// Is the mouse currently down in the drag_icon?
	public static var _drag_icon_mouse_down:Bool; 

	// Is the mouse currently over the drag_icon?
	var _drag_icon_mouse_over:Bool; 

	// The direction of this annotation : 0 - start, 1 - end
	var _direction:Int; 

	var _height:Float;

	// The parent Annotation of this AnnotationExtent
	var _annotation:Annotation;

	public function new (annotation_:Annotation,direction_:Int,height_:Float) {
		super();

		_annotation = annotation_;
		_direction = direction_;
		_height = height_;

		_drag_icon_mouse_down = false;
		_drag_icon_mouse_over = false;

		_well = new Sprite();
		this.addChild(_well);

		_rect = new Sprite();
		_well.addChild(_rect);

		_drag_icon = new Sprite();
		_well.addChild(_drag_icon);

 		_drag_icon.addEventListener(MouseEvent.MOUSE_DOWN, dragIconMouseDownListener);
 		_drag_icon.addEventListener(MouseEvent.MOUSE_UP, dragIconMouseUpListener);
 		_drag_icon.addEventListener(MouseEvent.MOUSE_OVER, dragIconMouseOverListener);
 		_drag_icon.addEventListener(MouseEvent.MOUSE_OUT, dragIconMouseOutListener);

		redraw();
	}

	private function initListener (e:Event):Void {
	}

 	public function redraw () {
		var position:Float;
		if (_direction == 0) {
			position = _annotation._start;
		} else {
			position = _annotation._end;
		}

		_rect.graphics.clear();
		_rect.graphics.lineStyle(1,0x333333);
		_rect.graphics.beginFill(0x0000FF,1);
		_rect.graphics.moveTo(position,0);
		_rect.graphics.lineTo(position,_annotation._y-10);
		_rect.graphics.moveTo(position,_annotation._y+10);
		_rect.graphics.lineTo(position,_height);

		_drag_icon.graphics.clear();
		_drag_icon.graphics.lineStyle(1,0x333333);

		if (_drag_icon_mouse_over) {
			_drag_icon.graphics.beginFill(0xFFFFFF,1);
		} else {
			_drag_icon.graphics.beginFill(0xDDDDFF,1);
		}

		if (_direction == 0) {
			_drag_icon.graphics.moveTo(position,_annotation._y-7);
			_drag_icon.graphics.lineTo(position+7,_annotation._y);
			_drag_icon.graphics.lineTo(position,_annotation._y+7);
			_drag_icon.graphics.lineTo(position,_annotation._y-7);
		} else {
			_drag_icon.graphics.moveTo(position,_annotation._y-7);
			_drag_icon.graphics.lineTo(position-7,_annotation._y);
			_drag_icon.graphics.lineTo(position,_annotation._y+7);
			_drag_icon.graphics.lineTo(position,_annotation._y-7);
		}

 	}

 	function dragIconMouseDownListener (e:MouseEvent):Void {
	    _annotation._annotator.setCurrentAnnotationExtent(this);

		_drag_icon_mouse_down = true;

		// We're changing one of the ends of this Annotation, so make sure to
		// save it when we're done.
		_annotation._annotator.setSaved(false);

		// Stop the propogation here so that the Annotator doesn't create
		// another Annotation.
 		e.stopPropagation();
		redraw();

		OrcaAnnotator.setUserCurrentlyInputtingText(true);
	}

 	function dragIconMouseUpListener (e:MouseEvent):Void {
		_drag_icon_mouse_down = false;
		redraw();

		OrcaAnnotator.setUserCurrentlyInputtingText(false);
	}

 	function dragIconMouseOverListener (e:MouseEvent):Void {
		_drag_icon_mouse_over = true;
		redraw();
	}

 	function dragIconMouseOutListener (e:MouseEvent):Void {
		_drag_icon_mouse_over = false;
		redraw();
	}

	public function setPosition(position:Float) {
		if (_direction == 0) {
			_annotation.setStart(position);
			// 			_annotation._start = position;
			if (_annotation._start > _annotation._end) {
			// 				_annotation._end = _annotation._start;
				_annotation.setEnd(_annotation._start);
			}
		} else {
			_annotation.setEnd(position);
			// 			_annotation._end = position;
			if (_annotation._end < _annotation._start) {
			_annotation.setStart(_annotation._end);
			// 				_annotation._start = _annotation._end;
			}
		}
	}
}
