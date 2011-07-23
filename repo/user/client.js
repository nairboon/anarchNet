
function check()
{
	$('#location').html(location.hostname);
	if(location.protocol === "https:")
		$('#ssl-yes').show();
	else 
		$('#ssl-no').show();

}


$(document).ready(function(){
	check();
});

function login() {
	
}

function register() {

}