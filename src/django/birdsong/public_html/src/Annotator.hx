import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.events.KeyboardEvent;
import flash.events.Event;
import flash.events.ProgressEvent;
import flash.events.HTTPStatusEvent;
import flash.events.IOErrorEvent;
import flash.events.SecurityErrorEvent;
import flash.events.EventPhase;
import flash.net.URLLoader;
import flash.net.URLRequest;
import flash.net.URLRequestMethod;
import flash.net.URLLoaderDataFormat;
import flash.geom.Point;
import flash.geom.Rectangle;

class Annotator extends Sprite {

	var _well:Sprite;
	var _background:Sprite;
	var _image_container:Sprite;
	var _prediction_container:Sprite;
	var _annotation_container:Sprite;
	var _playback_bar:Sprite;
	// 	var _center_bar:Sprite;

	// The dragger shows the current temporary selected region from the user and
	// an annotation can be created from it.
	var _dragger:Sprite;
	var _dragger_visible:Bool;
	var _dragger_start:Float;
	var _dragger_end:Float;
	var _dragger_start_ms:Float;
	var _dragger_end_ms:Float;

	var _height : Float;
	var _width  : Float;

	var _moving_dragger : Bool;

	// The array of all annotations in this Annotator
	var _annotation_array : Array<Annotation>;

	// The array of all predictions in this Annotator
	var _prediction_array : Array<Prediction>;

	// The current AnnotationExtent that we are dragging
	var _current_ae : AnnotationExtent;

	// The current visualization for this Annotator (e.g. waveform/spectrogram)
	var _visualization_id : String;

	// Loaders to load the index.txt files
	var _visualization_textfile_loader:URLLoader;
	var _annotation_textfile_loader:URLLoader;
	var _prediction_textfile_loader:URLLoader;

	// An array of an array of strings that hold the URLs to the different
	// images for the different zoom levels
	var _image_urls : Array<Array<String>>;

	// The images for the different zoom levels
	var _images : Array<Array<Loader>>;

	// The current zoom level.  1.0 is fully zoomed out.
	var _zoom:Float;
	var _zoom_increment:Float;
	var _zoom_level:Int;
	var _zoom_value:Float;
	var _sections:Float;
	var _audio_length:Float;
	var _section_size_ms:Float;

	var _max_zoom:Int;

	// Has everything been loaded?
	var _total_loaded : Int; // The total number that have been loaded so far
	var _max_loaded : Int; // The maximum number that will be loaded
 	var _loaded : Bool;

	// The area that we can drag the contents of the Annotator in
	var _trackRect : Rectangle; 	

	// The original x value, for figuring out the ends of the visible region
	var _orig_x : Int;

	// The start, end and center of the current view
	var _view_start_ms:Float;
	var _view_end_ms:Float;
	var _view_center_ms:Float;

	var _zoom_ms_scale_factor:Float;  // number that you can multiply ms by to get pixels
	var _zoom_pix_scale_factor:Float; //  number that you can multiply pixels by to get ms

	var _images_ready : Bool;

	// Where the playback bar should be
	var _playback_bar_ms : Float;

	// Have we saved all the changes to the Annotations?
	var _saved : Bool;

	private var _sound_player : SoundPlayer;

	public function new (x_:Int, y_:Int, width_:Int, height_:Int, visualization_id_:String, sound_player_:SoundPlayer) {
		super();

		x = x_;
		y = y_;
		_width = width_;
		_height = height_;
		_orig_x = x_;
		_visualization_id = visualization_id_;
		_sound_player = sound_player_;

		_annotation_array = [];
		_prediction_array = [];

		_moving_dragger = false;
		_saved = true;

		// Start off with a zoom that transforms the whole 2400 pixels into 800
		// visible pixels.
		_zoom = 0.0;
 		_zoom_increment = 0.1;
		doZoom();

		// sness - Just setting these to known values for now, should change
		// this to get the values from GlobalSettings.
		_view_start_ms = 0;
		_view_end_ms = 2881801;
		_view_center_ms = 1440900.5;

		// Set defaults
		_total_loaded = 0;
		_max_loaded = 0;
		_loaded = false;
		_images_ready = false;

		_well = new Sprite();
		_well.graphics.beginFill(0xFFFFFF,1);
		_well.graphics.moveTo(-10000,0);
		_well.graphics.lineTo(10000,0);
		_well.graphics.lineTo(10000,_height);
		_well.graphics.lineTo(-10000,_height);
		_well.graphics.lineTo(-10000,0);
		this.addChild(_well);

 		// The current playback position
 		_playback_bar = new Sprite();
 		_playback_bar.graphics.lineStyle(1,0x333333);		
 		_playback_bar.graphics.moveTo(0,0);
 		_playback_bar.graphics.lineTo(2,0);
 		_playback_bar.graphics.lineTo(2,_height);
 		_playback_bar.graphics.lineTo(0,_height);
 		_playback_bar.graphics.lineTo(0,0);
 		_playback_bar.x = _width / 2;
 		this.addChild(_playback_bar);

		// 		// The center of the Annotator
		// 		_center_bar = new Sprite();
		// 		_center_bar.graphics.lineStyle(1,0x000000);		
		// 		_center_bar.graphics.beginFill(0x000000,1);
		// 		var triangle_size:Float = 2;
		// 		// Top triangle
		// 		_center_bar.graphics.moveTo((_width/2)-triangle_size,0);
		// 		_center_bar.graphics.lineTo((_width/2)+triangle_size,0);
		// 		_center_bar.graphics.lineTo((_width/2)+0.5,triangle_size);
		// 		// Vertical line
		//  		_center_bar.graphics.moveTo(_width/2,0);
		//  		_center_bar.graphics.lineTo((_width/2),_height);
		// 		// Bottom triangle
		// 		_center_bar.graphics.moveTo((_width/2)-triangle_size,_height);
		// 		_center_bar.graphics.lineTo((_width/2)+triangle_size,_height);
		// 		_center_bar.graphics.lineTo((_width/2)+0.5,_height-triangle_size);
		// 		this.addChild(_center_bar);

		// A container for all the images we will load
		_image_container = new Sprite();
		_well.addChild(_image_container);

		// A container for all the predictions we will load
		_prediction_container = new Sprite();
		_well.addChild(_prediction_container);

		// A container for all the annotations we will load
		_annotation_container = new Sprite();
		_well.addChild(_annotation_container);

		// sness - This is needed so that we can detect when the user releases the mouse
		// button when they are outside the window, otherwise, if they are dragging the mouse
		// and then release the mouse outside, the window continues to scroll.
		// 		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownListener);
		// 		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveListener);
  		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_UP, mouseUpListener);
		flash.Lib.current.stage.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelListener);

		_image_container.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownListener);
		_image_container.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveListener);
 		_image_container.addEventListener(MouseEvent.MOUSE_UP, mouseUpListener);
   		_image_container.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelListener);

 		_prediction_container.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownListener);
 		_prediction_container.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveListener);
  		_prediction_container.addEventListener(MouseEvent.MOUSE_UP, mouseUpListener);
		_prediction_container.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelListener);

 		_annotation_container.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownListener);
 		_annotation_container.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveListener);
  		_annotation_container.addEventListener(MouseEvent.MOUSE_UP, mouseUpListener);
		_annotation_container.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelListener);

 		_well.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownListener);
 		_well.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveListener);
 		_well.addEventListener(MouseEvent.MOUSE_UP, mouseUpListener);
		_well.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelListener);

		flash.Lib.current.stage.addEventListener(KeyboardEvent.KEY_DOWN, mainKeyDownListener);
  		flash.Lib.current.stage.addEventListener(KeyboardEvent.KEY_UP, mainKeyUpListener);


		// The grey dragger that allows the user to create new annotations
		_dragger = new Sprite();
		_well.addChild(_dragger);
		
		_dragger_visible = false;

 		// Get the text file for the visualization
		loadVisualization();

 		// Get the text file for the annotations
		var url:String = OrcaAnnotator._annotation_url;
		_annotation_textfile_loader = new URLLoader();
		var request:URLRequest = new URLRequest(url);
		_annotation_textfile_loader.load(request);
		_annotation_textfile_loader.addEventListener(Event.COMPLETE, annotationTextFileLoaderCompleteHandler);

 		// Get the text file for the predictions
		var url:String = OrcaAnnotator._prediction_url;
		_prediction_textfile_loader = new URLLoader();
		var request:URLRequest = new URLRequest(url);
		_prediction_textfile_loader.load(request);
		_prediction_textfile_loader.addEventListener(Event.COMPLETE, predictionTextFileLoaderCompleteHandler);
		
		// Create the rectangle that constrains where we can drag the contents
		createTrackRect();

		redraw();

	}

	////////////////////////////////////////
	//
	// Listeners
	//
	////////////////////////////////////////

	// Zoom in and out when the user scrolls the mouse wheel
 	function mouseWheelListener(e:MouseEvent):Void {

		var z:Float;

		if (e.delta < 0) {
 			z = _zoom - ( _zoom_increment / (_zoom + 1));
		} else {
 			z = _zoom + ( _zoom_increment / (_zoom + 1));
		}
		if (z < 0.0) {
			z = 0.0;
		}
 		if (z > 4.9) {
 			z = 4.9;
 		}
		setZoom(z);
		doZoom();

		redraw();

	}

 	function mouseDownListener(e:MouseEvent):Void {
		//trace("down");
		// 		if (_well != null) {
		// 			trace("_well.x=" + _well.x);
		// 		}

		// If the ctrl key is pressed, show the dragger, which starts 
		// the process of adding a new annotation
		if (e.ctrlKey) {
			_dragger_visible = true;
			_dragger_start = e.localX;
			_dragger_end = e.localX;
			_moving_dragger = true;
			// sness - This was there from when I ported the code over, not 
			// sure why it was needed.
			// _well.setChildIndex(_dragger, _well.numChildren - 1);
			redraw();
			return;
		} 

		// Find the position of the mouse in ms and et the current playback
		// position to this point
		//
		// sness - FIXME GETTER
		if (!e.shiftKey) {
			calcEdges();
			var pos:Float = e.stageX / _width;
			var mspos:Int = Std.int(_view_start_ms + ((_view_end_ms - _view_start_ms) * pos));
			OrcaAnnotator.setPosition(mspos,true);
			redraw();
		}
		
		// Allow the user to drag the Annotator
		_well.startDrag(false,_trackRect);
 	}

 	function mouseUpListener(e:MouseEvent):Void {
		_moving_dragger = false;
		AnnotationExtent._drag_icon_mouse_down = false;
		this.stopDrag();
		redraw();
 	}

 	function mouseMoveListener(e:MouseEvent):Void {

		// If we are currently dragging the AnnotationExtent drag icon
		if (AnnotationExtent._drag_icon_mouse_down) {
			_current_ae.setPosition(e.stageX - _well.x);
			redrawAllAnnotations();
			return;
		} 

		// Adjust the size of the dragger.  
		//
		// sness - We cheat here a bit by just adjusting the start edge of the
		// dragger, but since it's just a rectangle, it looks the same.  When we
		// turn it into an Annotation later, we check to see if the start and
		// end ends are swapped.
		//
		// sness - FIXME GETTER
		if (_moving_dragger) {
			_dragger_start = e.stageX - _well.x;
			calcEdges();
			loadNecessaryImages();
			redraw();
		}
		// 		calcEdges();
		// 		loadNecessaryImages();
	}

	private function mainKeyDownListener(e:KeyboardEvent):Void {
		// If the dragger is visible, create a new annotation.
		// For its name, give it the character the user just typed
		var key:Int = e.keyCode;
		if (_dragger_visible && key != 16 && key != 17 && key != 18 ) {
			// Check to see if we need to swap _dragger_start and _dragger_end
			// 			if (_dragger_start > _dragger_end) {
			// 				var tmp:Float = _dragger_end;
			// 				_dragger_end = _dragger_start;
			// 				_dragger_start = tmp;
			// 			}
			fixDraggerEnds();

			createAnnotation(0,pix2ms(_dragger_start),pix2ms(_dragger_end),"");
			_dragger_visible = false;
			_saved = false;
			redraw();
		}
	}

	private function mainKeyUpListener(e:KeyboardEvent):Void {
	}

	private function annotationDeleteEventListener(e:Event):Void {
		_annotation_array.remove(e.target);
		_well.removeChild(e.target);
		redrawAllAnnotations();
	}


	////////////////////////////////////////
	//
	// File load completion handlers
	//
	////////////////////////////////////////

	private function loadImageListener(e:Event):Void {
// 		trace("e.target" + e.target + " e.target.loader=" + e.target.loader);
 		var l:Loader = e.target.loader;
//    		_image_container.addChildAt(l.content,0);
//  		l.content.visible = true;
 		l.content.x = Std.parseInt(l.name);
// 		trace("l.content.x=" + l.content.x);
		_total_loaded += 1;
		_loaded = true;
		// sness - We probably don't need to do this, not sure why it was here
		// 
		// sness - Ah... This is probably setting the scaleX property correctly
 		doZoom();
 	}

	private function visualizationTextFileLoaderCompleteHandler(event:Event):Void {
		_image_urls = new Array();
		for (i in 0...6) {
			_image_urls[i] = new Array();
		}

 		var elements:Array<String> = new Array();
 		var lines:Array<String> = _visualization_textfile_loader.data.split('\n');
		
  		// Split the input data file into lines and fill up the _image_urls data
  		// structure with URLs for the different zoom levels
  		for (i in 0...lines.length) {
  			if (lines[i].charAt(0) != "#") {
   				elements = lines[i].split(',');
   				var zoomlevel = Std.parseInt(elements[0]);
   				var index = Std.parseInt(elements[1]);
   				var url = elements[2];
 				if (zoomlevel != null && index != null) {
 					_image_urls[zoomlevel][index] = url;
 					_max_loaded += 1;
 				}
  			}
  		}

 		// Now that we are done loading the text file, load the image files.
 		// Make arrays to contain the loaders
 		_images = new Array();
 		for (i in 0...6) {
 			_images[i] = new Array();
			var sections:Int = Std.int(Math.pow(3,i));
			for (j in 0...sections) {
				_images[i].push(null);
			}
 		}
		_images_ready = true;

		loadNecessaryImages();

	}

	// Both called when the application is initializing and loads the
	// annotations, and when the web server returns a response to us trying to
	// save the annotations.  The web server should return the string "update
	// complete " if everything worked.
	private function annotationTextFileLoaderCompleteHandler(event:Event):Void {
		_annotation_array = [];
		for(i in 0..._annotation_container.numChildren) {
			_annotation_container.removeChildAt(0);
		}

		if (event.target.data.charAt(0) == "e") {
			trace("error in saving annotations.  tell sness@sness.net");
		} else {
			var elements:Array<String> = new Array();
			var lines:Array<String> = _annotation_textfile_loader.data.split('\n');

			// Split the input data file into lines and fill up the _image_urls data
			// structure with URLs for the different zoom levels
			for (i in 0...lines.length) {
				if (lines[i].charAt(0) != "#") {
					elements = lines[i].split(',');
					var id = Std.parseInt(elements[0]);
					var start_ms = Std.parseInt(elements[1]);
					var end_ms = Std.parseInt(elements[2]);
					var name = elements[3];
					if (id != null && start_ms != null && end_ms != null && name != null) { 
						createAnnotation(id,start_ms,end_ms,name);
					}
				}
			}
			redrawAllAnnotations();
		}
		_saved = true;
	}


	private function predictionTextFileLoaderCompleteHandler(event:Event):Void {
 		var elements:Array<String> = new Array();
 		var lines:Array<String> = _prediction_textfile_loader.data.split('\n');

 		// Split the input data file into lines and fill up the _image_urls data
 		// structure with URLs for the different zoom levels
 		for (i in 0...lines.length) {
 			if (lines[i].charAt(0) != "#") {
  				elements = lines[i].split(',');
  				var id = Std.parseInt(elements[0]);
  				var start_ms = Std.parseInt(elements[1]);
  				var end_ms = Std.parseInt(elements[2]);
  				var name = elements[3];
  				var confidence = Std.parseFloat(elements[4]);
  				var color = Std.parseInt(elements[5]);
				if (id != null && start_ms != null && end_ms != null && name != null && color != null) { 
					createPrediction(id,start_ms,end_ms,name,confidence,color);
				}
			}
		}
	}


	public function loadNecessaryImages():Void {
//   		trace("load");
		if (_images_ready) {

			// sness - FIXME GETTER
			calcEdges();
// 			// How many different images are at this zoom level
// 			var sections:Float = Math.pow(3,_zoom_level);
// 			var audio_length:Float = GlobalSettings.getAudioLength();
		
			// Figure out which images have their start or end edges within _view_start_ms and _view_end_ms
			var start:Float = 0;
			var end:Float = _section_size_ms;

// 			trace("vs=" + _view_start_ms + " ve=" + _view_end_ms);

			for (i in 0..._image_urls[_zoom_level].length) {
//   				trace("iu[z]=" + _image_urls[_zoom_level][i] + " s=" + start + " e=" + end);
				if (((start >= _view_start_ms) && (start <= _view_end_ms)) ||
					((end >= _view_start_ms) && (end <= _view_end_ms)) ||
					((start >= _view_start_ms) && (end >= _view_end_ms))) {
//  					trace("zl=" + _zoom_level + " i=" + i + " s=" + ii(start) + " vs=" + ii(_view_start_ms) + " e=" + ii(end) + " ve=" + ii(_view_end_ms));
					if (_images[_zoom_level][i] == null) {
						loadImage(_zoom_level,i);
					}
				}
				start = end;
				end += _section_size_ms;
			}
		}
	}

	public function loadImage(zoom_level:Int,index:Int):Void {
// 		trace("loadImage zoom_level=" + zoom_level + " index=" + index);
// 		trace("_image_urls[" + zoom_level + "][" + index + "]=" + _image_urls[zoom_level][index]);
		_images[zoom_level][index] = new Loader();

   		_image_container.addChildAt(_images[zoom_level][index],0);

//  		_images[zoom_level][index].visible = false;
		_images[zoom_level][index].name = Std.string(index * 2400);
		_images[zoom_level][index].contentLoaderInfo.addEventListener(Event.INIT, loadImageListener);
		_images[zoom_level][index].load(new URLRequest(_image_urls[zoom_level][index]));

		// sness - Debugging
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(Event.OPEN, handleOpen);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, handleProgress);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(Event.COMPLETE, handleComplete);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(HTTPStatusEvent.HTTP_STATUS, handleHttpStatus);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, handleIoError);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(SecurityErrorEvent.SECURITY_ERROR, handleSecurityError);
// 		_images[zoom_level][index].contentLoaderInfo.addEventListener(Event.UNLOAD, handleUnload);
	}

	private function handleOpen(e:Event):Void {
		trace("open");
	}

	private function handleProgress(e:Event):Void {
		trace("progress");
	}

	private function handleComplete(e:Event):Void {
		trace("complete");
	}

	private function handleHttpStatus(e:Event):Void {
		trace("httpstatus");
	}

	private function handleIoError(e:Event):Void {
		trace("ioerror");
	}

	private function handleSecurityError(e:Event):Void {
		trace("securityerror");
	}

	private function handleUnload(e:Event):Void {
		trace("unload");
	}



	public function doZoom():Void {
// 		trace("doZoom");
		// 		var old_view_center_ms:Float = _view_center_ms;


		setZoom(_zoom);

		
		// Calculate the positions of the start and edges that are currently visible
		loadNecessaryImages();
		// sness - FIXME GETTER
		// 		calcEdges();

		//
		// Center the view on the current playback position
		//
		setViewCenter_ms(getPlaybackBar());
		

// 		// Hide all the images
//  		if (_loaded) {
// 			for (i in 0..._images.length) {
// 				for (j in 0..._images[i].length) {
// 					if (_images[i][j] != null && _images[i][j].content != null) {
// 						_images[i][j].content.visible = false;
// 					}
// 				}
// 			}
// 		}

// 		// Just show the images in the current zoom_level
//  		if (_loaded) {
//  			for (j in 0..._images[_zoom_level].length) {
// 				if (_images[_zoom_level][j] != null && _images[_zoom_level][j].content != null) {
// 					var shift_x:Float = j * (2400 * _zoom_value);
// 					_images[_zoom_level][j].content.visible = true;
// 					_images[_zoom_level][j].content.scaleX = _zoom_value;
// 					_images[_zoom_level][j].content.x = shift_x;
// 					// 					trace("zoom_level= " + zoom_level + " j= " + j);
// 				}
//  			}
//  		}

		// Get each annotation to update it's start and end ends
		for (n in _annotation_array) {
			n.redraw();
		}

		// Get each prediction to update it's start and end ends
		for (n in _prediction_array) {
			n.redraw();
		}
	}

 	public function redraw():Void {

		// 		trace("redraw()");
		// 		trace("_well.x=" + _well.x);
		//   		trace("s=" + Std.int(_view_start_ms) + " c=" + Std.int(_view_center_ms) + " e=" + Std.int(_view_end_ms));

		// sness - In here we should recenter the view based on the current
		// playback slider position as obtained from OrcaAnnotator.getPosition()

		if (_dragger != null) {
			_dragger.graphics.clear();
			if (_dragger_visible) {
				_dragger.alpha = 0.5;
				_dragger.graphics.beginFill(0xCCCCCC,1);
				_dragger.graphics.moveTo(_dragger_start,0);
				_dragger.graphics.lineTo(_dragger_end,0);
				_dragger.graphics.lineTo(_dragger_end,_height);
				_dragger.graphics.lineTo(_dragger_start,_height);
				_dragger.graphics.lineTo(_dragger_start,0);
			}
		}

		setPlaybackBar(OrcaAnnotator.getPosition());
		// 		_playback_bar.x = ms2pix(OrcaAnnotator.getPosition()) + _well.x;
		
		// sness - Not sure if this is necessary to do this on redraw, and
		// anyways should only do when dragging, when start/end has changed,
		// zoom has changed or center has changed.
// 		loadNecessaryImages();

 		if (_loaded && _images != null) {

			// Hide all the images
			for (i in 0..._images.length) {
				for (j in 0..._images[i].length) {
					if (_images[i][j] != null && _images[i][j].content != null) {
						_images[i][j].content.visible = false;
					}
				}
			}

			// Just show the images in the current zoom_level
 			for (j in 0..._images[_zoom_level].length) {
				if (_images[_zoom_level][j] != null && _images[_zoom_level][j].content != null) {
					var shift_x:Float = j * (2400 * _zoom_value);
					_images[_zoom_level][j].content.visible = true;
					_images[_zoom_level][j].content.scaleX = _zoom_value;
					_images[_zoom_level][j].content.x = shift_x;
					// 					trace("zoom_level= " + zoom_level + " j= " + j);
				}
 			}
 		}
		
 	}

	public function newAnnotation() {
		if (_dragger_visible) {
			fixDraggerEnds();
			createAnnotation(0,pix2ms(_dragger_start),pix2ms(_dragger_end),"");
		}
	}

	// Create a new annotation
	function createAnnotation(id:Int,start_ms:Float,end_ms:Float,title:String) {
 		var annotation = new Annotation(id,start_ms,end_ms,title,this);
 		var i:Int = _annotation_array.push(annotation);
 		annotation._index = i;
 		annotation.addEventListener(Annotation.DELETE_ANNOTATION_EVENT,annotationDeleteEventListener);
 		_annotation_container.addChild(annotation);
	}

	// Create a new prediction
	function createPrediction(id:Int,start_ms:Float,end_ms:Float,title:String,confidence:Float,color:Int) {
 		var prediction = new Prediction(id,start_ms,end_ms,title,confidence,color,this);
 		var i:Int = _prediction_array.push(prediction);
 		prediction._index = i;
 		_prediction_container.addChild(prediction);
	}

	// Redraw all the annotations in _annotation_array
	function redrawAllAnnotations() {
		determineAnnotationYPositions();
		for (n in _annotation_array) {
			n.redraw();
		}
	}

	// Figure out y position of all annotations
	//
	// sness - This code was lifted from Audacity.  I don't really like how it
	// behaves, it would be nice to do it properly later, but there are a lot of
	// special cases to handle.
	function determineAnnotationYPositions() {
		var MAX_NUM_ROWS = 10;
		var nRows:Int = MAX_NUM_ROWS;
		
		var xUsed : Array<Float> = new Array();
		for (i in 0...MAX_NUM_ROWS) {
			xUsed.push(-1.0);
		}
		
		var iRow:Int;
		var nRowsUsed:Int = 0;
		var yRowHeight:Float = 10.0;
		for (i in 0..._annotation_array.length) {
			iRow = 0;
			_annotation_array[i]._y=-1.0;
			while( (iRow<nRowsUsed) && (xUsed[iRow] != _annotation_array[i]._start )) {
				iRow++;
			}
			if( iRow >= nRowsUsed ) {
				iRow=0;
				while( (iRow<nRows) && (xUsed[iRow] > _annotation_array[i]._start ))
					iRow++;
			}
			
			if( iRow<nRows ) {
				if( iRow >= nRowsUsed )
					nRowsUsed=iRow+1;
				_annotation_array[i]._row=iRow;
				xUsed[iRow]=_annotation_array[i]._start;

				if(xUsed[iRow] < _annotation_array[i]._end) 
					xUsed[iRow]=_annotation_array[i]._end;
			}
		}

		var row_height:Float = _height / nRowsUsed;
		var start = row_height / 2;
		for (n in _annotation_array) {
			n._y = start + (n._row * row_height);
		}

	}

	// Create the rectangle that constrains where we can drag the contents
	private function createTrackRect():Void {
		// Set the width to zero for strictly vertical dragging
 		_trackRect = new Rectangle(-1000000.0, 0, _width + 1000000, 0); 
	}


	//
	// Getters and setters
	//
	public function setCurrentAnnotationExtent(ae:AnnotationExtent):Void {
		_current_ae = ae;
	}

	public function getHeight():Float {
		return _height;
	}
	
	//
	// Save all annotations back to the web server using a POST request to 
	// the /annotations/update method.
	//
	// We package up all the annotations in a text file of the form:
	// id,start_ms,end_ms,name
	//
	public function saveAnnotations():Void {

		// Build a string of the data in all the annotations
		var output:String = "";
		output += "recording_id=" + OrcaAnnotator._recording_id + "&";
		output += "annotations=";
		for (n in _annotation_array) {
			output += n.to_string() + "\n";
		}
		// Send the string to the web server
		var request:URLRequest = new URLRequest("/annotations/update");
		request.method = URLRequestMethod.POST;
		request.data = output;

		// 		var loader:URLLoader = new URLLoader();
		// 		loader.dataFormat = URLLoaderDataFormat.TEXT;
		// 		loader.addEventListener(Event.COMPLETE, updateLoaderCompleteHandler);
		_annotation_textfile_loader.load(request);

	}

// 	// Called when the web server returns a response to us trying to save the
// 	// annotations.  The web server should return the string "update complete "
// 	// if everything worked.
// 	private function updateLoaderCompleteHandler(event:Event):Void {
// 	}
	
	// Have all the annotations been saved back to the web server?
	public function setSaved(b:Bool):Void {
		_saved = b;
	}

	public function getSaved():Bool {
		return _saved;
	}

	// Load a specific visualization from the web server, for
	// example, the waveform or spectrogram view
	public function loadVisualization() {
		_images_ready = false;
		var url:String = OrcaAnnotator._visualization_url + "/index.txt";
 		_visualization_textfile_loader = new URLLoader();
		var request:URLRequest = new URLRequest(url);
		_visualization_textfile_loader.load(request);
		_visualization_textfile_loader.addEventListener(Event.COMPLETE, visualizationTextFileLoaderCompleteHandler);
	}

	// Called from the main OrcaAnnotator class by Javascript when the 
	// user requests to see a different visualization
	public function reloadVisualization() {
		for(i in 0..._image_container.numChildren) {
			_image_container.removeChildAt(0);
		}

		_images = new Array();
		_image_urls = new Array();
		loadVisualization();
	}

 	public function zoomIn():Void {
		var z:Float;
		
		z = _zoom + (_zoom_increment * 10) / (_zoom + 1);
 		if (z > 4.9) {
 			z = 4.9;
 		}
		setZoom(z);
		doZoom();
	}

 	public function zoomOut():Void {
		var z:Float;

		z = _zoom - (_zoom_increment * 10) / (_zoom + 1) ;
		if (z < 0.0) {
			z = 0.0;
		}
		setZoom(z);
		doZoom();
	}

 	public function zoomFull():Void {
		setZoom(0.0);
		doZoom();
		_well.x = 0;
	}
	
	// Zoom to show the current selection
	//
	// sness - This is really difficult because we have to figure out what the
	// _zoom would be given ending points in ms.  Tricky parts include having to
	// take the log base 3 of a quantity halfway though.  Ouch.
 	public function zoomSelection():Void {
		if (_dragger_visible) {
			// Figure out the start and end points of the dragger in ms
			fixDraggerEnds();
			var start_ms:Float = pix2ms(_dragger_start);
			var end_ms:Float = pix2ms(_dragger_end);
			// _view_center_ms = (_view_start_ms + _view_end_ms) / 2;

			setViewStartEnd_ms(start_ms,end_ms);

			doZoom();

			// Change the dragger coordinates to fill the screen
			_dragger_start = ms2pix(_view_start_ms);
			_dragger_end = ms2pix(_view_end_ms);

			redraw();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// 
	// Getters and setters for how the visualization images are displayed on the
	// screen
	// 
	////////////////////////////////////////////////////////////////////////////////

	public function setZoom(z:Float):Void {

		// Remember what the extents of the dragger were before the zoom
		_dragger_start_ms = pix2ms(_dragger_start);
		_dragger_end_ms = pix2ms(_dragger_end);

		_zoom = z;
		calcEdges();
		loadNecessaryImages();

		// Set the extents of the dragger after the zoom
		_dragger_start = ms2pix(_dragger_start_ms);
		_dragger_end = ms2pix(_dragger_end_ms);

	}

	
	public function setViewStartEnd_ms(start_ms:Float, end_ms:Float):Void {
		_view_start_ms = start_ms;
		_view_end_ms = end_ms;

		// Figure out what zoom level we would need to show those end points
		_zoom_pix_scale_factor = Math.abs((_view_start_ms - _view_end_ms) / _width);

		// The length of the audio file in ms
		var audio_length:Float = GlobalSettings.getAudioLength();

		var o:Float = audio_length / (_zoom_pix_scale_factor * 2400.0);
		var sections:Int;
		if (o < 1) {
			_zoom_level = 0;
			sections = 1;
		} else {
			_zoom_level = Std.int(Math.log(o) / Math.log(3.0))+1;
			sections = Std.int(Math.pow(3,_zoom_level));
		}
		
	    _zoom_value = audio_length / (_zoom_pix_scale_factor * sections * 2400);
		var zoom_frac:Float = (_zoom_value - 0.3333) / 0.6666;
		_zoom = zoom_frac + _zoom_level;

		_zoom_ms_scale_factor = _zoom_value / (audio_length / (sections * 2400.0));

		// Figure out the x-offset
		_well.x = -1.0 * ms2pix(_view_start_ms);
		loadNecessaryImages();
	}

	public function setPosition(pos:Float):Void {
// 		trace("setPosition");
		setViewCenter_ms(pos);
		// sness - This should all be done by a redraw()
		loadNecessaryImages();
		redraw();
	}

	// Recenter the view on center_ms
	public function setViewCenter_ms(center_ms:Float):Void {
		if (_well != null) {
			var center_pix = ms2pix(center_ms);
			var start_pix = center_pix - (_width / 2);
			_well.x = -1.0 * start_pix;
		}
		loadNecessaryImages();
	}

	// Return the leftmost position of the currently visible window
	// in milliseconds
	public function getViewStart_ms():Float {
		calcEdges();
		return _view_start_ms;
	}

	// Return the rightmost position of the currently visible window
	// in milliseconds 
	public function getViewEnd_ms():Float {
		calcEdges();
		return _view_end_ms;
	}
	
	// Convert a value in pixels from the current view into milliseconds
	public function pix2ms(pix:Float):Float {
		calcEdges();
		return pix * _zoom_pix_scale_factor;
	}

	// Convert a value in milliseconds into pixels from the current view
	public function ms2pix(ms:Float):Float {
		calcEdges();
		return ms * _zoom_ms_scale_factor;
	}

	// From the zoom level and _well.x, determine the start, end and center of
	// the current view in ms
	public function calcEdges():Void {
		if (_well != null) {
			_view_start_ms = (_orig_x - _well.x) * _zoom_pix_scale_factor;
			_view_end_ms = _view_start_ms + (_width * _zoom_pix_scale_factor);
			_view_center_ms = _view_start_ms + ((_view_end_ms - _view_start_ms) / 2);

			// The zoom level, an int between 0 and 5
			_zoom_level = Std.int(_zoom);
			if (_zoom_level > GlobalSettings.getMaxZoom()) {
				_zoom_level = GlobalSettings.getMaxZoom();
			}

			// Just the fractional part of zoom_level
			var zoom_frac:Float = (_zoom - _zoom_level);

			// The actual amount we are zooming the images by
			_zoom_value = (zoom_frac * 0.666) + 0.333;

			// The length of the audio file in ms
			_audio_length = GlobalSettings.getAudioLength();
		
			// How many different images are at this zoom level
			_sections = Math.pow(3,_zoom_level);
			
			// How many ms per section
 			_section_size_ms = _audio_length / _sections;

			// _zoom_ms_scale_factor puts all these together into a single number
			// that you can multiply ms by to get pixels
			_zoom_ms_scale_factor = _zoom_value / (_audio_length / (_sections * 2400.0));

			// _zoom_pix_scale_factor puts all these together into a single number
			// that you can multiply pixels by to get ms
			_zoom_pix_scale_factor = _audio_length / (_sections * 2400.0 * _zoom_value);
		}
	}

	// Set the position of the playback bar, the rectangle that shows where
	// audio is currently playing
	public function setPlaybackBar(f:Float):Void {
		if (_playback_bar != null) {
			_playback_bar_ms = f;
			_playback_bar.x = ms2pix(f) + _well.x;
		}
	}

	public function getPlaybackBar():Float {
		return _playback_bar_ms;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	// Utility functions
	//
	////////////////////////////////////////////////////////////////////////////////
	
	// A "r"easonable number of decimal points for trace()
	public function r(f:Float):Float {
		var d:Float = 10000;
		var n:Int = Std.int(f * d);
		return n / d;
	}
	
	// Change a Float to an Int for trace()
	public function ii(f:Float):Int {
		return Std.int(f);
	}

	public function tracer():Void {
		var audio_length:Float = GlobalSettings.getAudioLength();
		// 		trace("wx=" + _well.x + " ds=" + _dragger_start + " de=" + _dragger_end);
		// 		trace("vs=" + Std.int(_view_start_ms) + " ve=" + Std.int(_view_end_ms));
		//  		trace("fvs=" + _view_start_ms / audio_length + " fve=" + _view_end_ms / audio_length);
		// 		if (_well != null) {
		// 			trace("wx=" + _well.x + " ds=" + r(_dragger_start) + " de=" + r(_dragger_end) + " fvs=" + r(_view_start_ms / audio_length) + " fve=" + r(_view_end_ms / audio_length));
		// 		}
// 		trace("bar=" + r(getPlaybackBar() / audio_length));
// 		trace("ds=" + ii(_dragger_start) + " de=" + ii(_dragger_end) + "dsms=" + ii(_dragger_start_ms) + " dems=" + ii(_dragger_end_ms)); 
	}

	// If the start of the dragger is bigger than the end, swap them
	public function fixDraggerEnds():Void {
		if (_dragger_start > _dragger_end) {
			var tmp:Float = _dragger_start;
			_dragger_start = _dragger_end;
			_dragger_end = tmp;
		}
	}

}
