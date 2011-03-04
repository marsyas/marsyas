import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;
import flash.text.TextField;
import flash.text.TextFormat;
import flash.xml.XML;


class XMLLoader extends Sprite {

	//
	// Read the global XML configuration file
	//
	public function new() {
		super();

 		var http = new haxe.Http(OrcaAnnotator._recording_url + "/index.xml"); 
		http.onData = function(d) { 
			var src : Xml = Xml.parse(d).firstChild(); 
			fastIt(src);
		} 
		http.request(false);

	}

	public static function fastIt(src:Xml) {
  		var fast = new haxe.xml.Fast(src);
		
		// sness - FIXME : It would be nice to not have to iterate through the
		// elements of fast, and just to get the configuration block.
		for (i in fast.elements) {
			GlobalSettings.setAudioLength(Std.parseInt(i.node.length.innerData));
			GlobalSettings.setMaxZoom(Std.parseInt(i.node.max_zoom.innerData));
		}


	}


}
