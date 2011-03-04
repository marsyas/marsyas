$(document).ready(function(){
		console.log("ready");
		
		// Chrome and Firefox support ogg.  Safari doesn't.
		var audio1 = new Audio("assets/ogg/A12_A12sp__1_.ogg");
		var audio2 = new Audio("assets/ogg/A12_N10.ogg");
		var audio3 = new Audio("assets/ogg/A12_N47.ogg");

		$("#target1").click(function(event){
				console.log("click1");
				audio1.load();
				audio1.play();
				$('#target1 img').animate({opacity : 0},100);
				$('#target1 .play-progress').css( 'opacity', 1);
			});

		$("#target2").click(function(event){
				console.log("click2");
				audio2.load();
				audio2.play();
				$('#target2 img').animate({opacity : 0},100);
				$('#target2 .play-progress').css( 'opacity', 1);
			});

		$("#target3").click(function(event){
				console.log("click3");
				audio3.load();
				audio3.play();
				$('#target3 img').animate({opacity : 0},100);
				$('#target3 .play-progress').css( 'opacity', 1);
			});

		$.fn.wait = function(time, type) {
			console.log("waiting");
			time = time || 1000;
			type = type || "fx";
			return this.queue(type, function() {
					var self = this;
					setTimeout(function() {
							$(self).dequeue();
						}, time);
				});
		};

		audio1.addEventListener('timeupdate', function(evt) {
		    var width = parseInt($('#target1').css('width'));
			var percentPlayed = Math.round(audio1.currentTime / audio1.duration * 100);
			var barWidth = Math.ceil(percentPlayed * (width / 100));
			$('#target1 .play-progress').css( 'width', barWidth);
		});

		audio2.addEventListener('timeupdate', function(evt) {
		    var width = parseInt($('#target2').css('width'));
			var percentPlayed = Math.round(audio2.currentTime / audio2.duration * 100);
			var barWidth = Math.ceil(percentPlayed * (width / 100));
			$('#target2 .play-progress').css( 'width', barWidth);
		});

		audio3.addEventListener('timeupdate', function(evt) {
		    var width = parseInt($('#target3').css('width'));
			var percentPlayed = Math.round(audio3.currentTime / audio3.duration * 100);
			var barWidth = Math.ceil(percentPlayed * (width / 100));
			$('#target3 .play-progress').css( 'width', barWidth);
		});

		audio1.addEventListener('ended', function(evt) {
				$('#target1 .play-progress').wait(500).animate({opacity : 0},500).animate({width : 0},0);
				$('#target1 img').animate({opacity : 1},100);
		});

		audio2.addEventListener('ended', function(evt) {
				$('#target2 .play-progress').animate({opacity : 0},500).animate({width : 0},0);
				$('#target2 img').animate({opacity : 2},100);
		});

		audio3.addEventListener('ended', function(evt) {
				$('#target3 .play-progress').animate({opacity : 0},500).animate({width : 0},0);
				$('#target3 img').animate({opacity : 3},100);
		});

	});