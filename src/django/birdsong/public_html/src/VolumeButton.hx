import flash.display.Loader;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.net.URLRequest;


class VolumeButton extends Sprite {

	private var loader1:Loader;
	private var loader2:Loader;

	public var toggle_state:Int;

	public function new() {
		super();

		toggle_state = 1;

		// Volume High
		loader1 = new Loader();
		loader1.visible = false;
		loader1.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		loader1.load(new URLRequest("/src/library/audio-volume-high.png"));

		// Volume Muted
		loader2 = new Loader();
		loader2.visible = false;
		loader2.contentLoaderInfo.addEventListener(Event.INIT, initListener);
		loader2.load(new URLRequest("/src/library/audio-volume-muted.png"));

		// Click on the VolumeButton
		this.addEventListener(MouseEvent.CLICK, toggleVolume);
	}

	private function initListener (e:Event):Void {
		addChild(loader1.content);
		addChild(loader2.content);
		updateButton();
	}

	private function updateButton ():Void {
		if (toggle_state == 1) {
			loader1.content.visible = true;
			loader2.content.visible = false;
		} else {
			loader1.content.visible = false;
			loader2.content.visible = true;
		}
	}

	private function toggleVolume(e:Event):Void {
		if (toggle_state == 1) {
			toggle_state = 0;
		} else {
			toggle_state = 1;
		}
		updateButton();
	}

}