
var keyBits = 512; //production min 1024 - 2048 but takes really long
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
 /*
  * getKEYS (pin)
  * aes_d(ENCKEY,md5(password)) = rsaPrivate
  */
}

function register() {
 /* 
  * generate rsa key
  * aes_e(rsaPrivate,md5(password)) = ENCKEY
  * send( ENCKE,PupKey,pin)
  */
	var keys = genrsa(keyBits);
}


function genrsa(bits) {
  var before = new Date();
  var rsa = new RSAKey();
  var res = {};
  do_status("Generating RSA Key...");
  rsa.generate(bits,dr.e.value);
  res.n = linebrk(rsa.n.toString(16),64);
  res.d = linebrk(rsa.d.toString(16),64);
  res.p = linebrk(rsa.p.toString(16),64);
  res.q = linebrk(rsa.q.toString(16),64);
  dr.dmp1.value = linebrk(rsa.dmp1.toString(16),64);
  dr.dmq1.value = linebrk(rsa.dmq1.toString(16),64);
  dr.coeff.value = linebrk(rsa.coeff.toString(16),64);
  var after = new Date();
  do_status("Key Generation Time: " + (after - before) + "ms");
}