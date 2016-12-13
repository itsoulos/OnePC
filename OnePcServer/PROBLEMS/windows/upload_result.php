<?php
	include_once("mysql.php");
	$link=mysql_connect($dbhost,$dbuser,$dbpass);
	if(!$link) die("0");
	mysql_select_db($database);
	$name=$_POST["name"];
	$type=$_POST["type"];
	$weights=$_POST["weights"];
	$result=$_POST["result"];
	$sql="insert into result(name,type,weights,result) values('$name','$type',$weights,$result)";
	$res=mysql_query($sql);
	if(!$res) 
	{
		mysql_close($link);
		die("0");
	}
	mysql_close($link);
	echo "1";
?>
