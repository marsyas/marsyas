function update() {
		console.log("hi");
		$.get("/annotations", function(data) {
							$("#annotations").html(data);
					});
};

function debug(data) {
		$.get("/recording/annotations/1.txt?user_id=1", function(data) {
							$("#annotations").html(data);
					});
};

function update_pitchcontours(data) {
		console.log("la1");
		$.get("/recordings/pitchcontour/1", function(data) {
							console.log("la2");
							// $("#pitchcontours1").html(data);
							console.log("la3");
					});
}

// do stuff when DOM is ready
$(document).ready(function() {
											update_pitchcontours();
									});

