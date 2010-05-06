<?php
/*
 bootstrapping server script
params: id, ip, port - to register a node
			 none		  - just to get some nodes
returns a list of 10 nodes

todo:	check for ttl
			remove old entries
			check for duplicates
*/

if(isset($_GET['id']))
	$node_id = $_GET['id'];
if(isset($_GET['ip']))
	$node_ip = $_GET['ip'];
if(isset($_GET['port']))
	$node_port = $_GET['port'];
	
$ttl = 60*60*24; // 1 day
$timestamp = time();
// we store the new node	
if( isset($node_id) && isset($node_ip) && isset($node_port)) 
 // syntax cheking
	if( strlen($node_id) == 128 && is_numeric($node_port) && preg_match('/^(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:[.](?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}$/',$node_ip)) {
			$fh = fopen("an.txt","a") or die("Cant open file");
			fwrite($fh, "\"$node_id\",\"$node_ip\",$node_port,$ttl,$timestamp\n");
			fclose($fh);
}
else {
		if(strlen($node_id)!= 128)
			echo "id length is not 128";
		else if(!is_numeric($node_port))
			echo "port is not a number: $node_port";
		else
			echo "$node_ip not an ip";
}
//read 10 entries
$lines = array();
$fh = fopen("an.txt","r") or die("Cant open file");
while( ($line =fgetcsv($fh,1024)) !== false) {
	array_push($lines,$line);
}
fclose($fh);

foreach(array_rand($lines,10) as $i )
	echo $lines[$i][0] . ","  . $lines[$i][1] . ","  . $lines[$i][2] . "\n" ;

?>