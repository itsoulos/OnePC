<?php
	include_once("mysql.php");
	$link=mysql_connect($dbhost,$dbuser,$dbpass);
	if(!$link) die("0");
	mysql_select_db($database);
	$sql="select count(*) as c,avg(result) as a,name,type,weights from result group by name,weights";
	$res=mysql_query($sql);
	if(!$res)
	{
		mysql_close($link);
		die("0");
	}
	echo "<table border=1 align=center>";
	echo "<tr>";
	echo "<td><b>Name</b></td><td><b>Type</b></td><td><b>Weights</b></td><td><b>Count</b></td><td><b>Result</b></td>";
	echo "</tr>";
	while($row=mysql_fetch_array($res,MYSQL_BOTH))
	{
		echo "<tr>";
		echo "<td>".$row["name"]."</td>";
		echo "<td>".$row["type"]."</td>";
		echo "<td align=right>".$row["weights"]."</td>";
		echo "<td align=right>".$row["c"]."</td>";
		if($row["type"]=="classification")
		{
			printf("<td>%.2lf%%</td>",$row["a"]);
		}
		else
		{
			printf("<td>%15.8lg</td>",$row["a"]);
		}
		echo "</tr>";
	}
	echo "</table>";
	mysql_close($link);
?>
