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
		for(i=0; i<points_array.length; i++) {
			var colour = points_array[i][2] * 256;							  
			context.fillStyle = "rgb(" + colour + "," + colour + "," + colour + ")";			
			var x = points_array[i][0];
			var y = points_array[i][1] / 10;
			context.fillRect(x, y, 1, 1);
		}


	}

}
