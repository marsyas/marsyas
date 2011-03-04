$(document).ready(
	function(){

		$("input[name='group1']").click(
			function(){
				// console.log("change");	
				var median_value = jQuery('#myform1 input:radio:checked').val();
				// console.log("median_value=" + median_value);
				$("#pitchcontour").load("/dtw/pitchcontour/" + median_value);
			});

		// Make things happen when a user clicks on the radio buttons
		$("#buffersize_form").click(
			function(){
				doDTWEmbed();
			});

		$("#hopsize_form").click(
			function(){
				doDTWEmbed();
			});

		$("#tolerance_form").click(
			function(){
				doDTWEmbed();
			});

		$("#median_form").click(
			function(){
				doDTWEmbed();
			});

		$("#pitchcontour-embed").mousemove(
			function(e){
				var x = e.pageX - this.offsetLeft;
				var y = e.pageY - this.offsetTop;
				doMainSVG(x,y);
			});

		
		// console.log("ready1");
	});


function doDTW() {
	// console.log("doDTW()");
	var median_value = jQuery('#myform1 input:radio:checked').val();
	$("#pitchcontour").load("/dtw/pitchcontour/" + median_value);
}

var ajax_request;

function doDTWEmbed() {
	var buffer_size = jQuery("#buffersize_form input:radio:checked").val();
	var hop_size = jQuery("#hopsize_form input:radio:checked").val();
	var tolerance = jQuery("#tolerance_form input:radio:checked").val();
	var median = jQuery("#median_form input:radio:checked").val();

	// console.log("doDTWEmbed() buffer_size=" + buffer_size + 
	// 			" hop_size=" + hop_size + " tolerance=" + tolerance +
	// 			" median=" + median);

	$("#pitchcontour-spinner").show();

	$("#pitchcontour-embed").load("/dtw/pitchcontour_embed" +
								  "?buffer_size=" + buffer_size +
								  "&hop_size=" + hop_size +
								  "&median=" + median +
								  "&tolerance=" + tolerance
								 ,function(rt, ts, xhr){
									 $("#pitchcontour-spinner").fadeOut();
									 
            });
}

function doMainSVG(x,y) {
	// console.log("x=" + x + " y=" + y);
	// var s = "x=" + x;
	// s += " y=" + y;
    var sound_file_len = 2.439;
    var width = 600.;
    var height = 200.;
    var x_offset = 100;
    var y_offset = 287;

	var time = (((x - x_offset) / width) * sound_file_len).toFixed(2);
	var frequency = ((1. - (y - y_offset) / height) * 10000.).toFixed(0);
	$("#pitchcontour-info-time").text(time);
	$("#pitchcontour-info-frequency").text(frequency);
}