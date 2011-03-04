$(document).ready(function(){
					  console.log("ready");
					  var el= document.getElementById("flag");

					  if (el && el.getContext) { 
						  drawSpectrogram(el);
					  }
				  });


function drawSpectrogram(el) {
	var context = el.getContext('2d');
	if(context){
		// Create an ImageData object.
		var imgd = context.createImageData(539,1486);
		var pix = imgd.data;

		for(i=0; i < points.points_array.length; i++) {
			var colour = points.points_array[i][2] * 256;							  
			var index = (points.points_array[i][0] + (points.points_array[i][1] * points.width))*4;
			pix[index+0] = colour; // red
			pix[index+1] = colour; // green
			pix[index+2] = colour; // blue
			pix[index+3] = 127;    // alpha
		}							  
		// Draw the ImageData object at the given (x,y) coordinates.
		context.putImageData(imgd, 0, 0);
	}
}
