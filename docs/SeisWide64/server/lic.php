<?php
//	require_once "Db.php";

	$hardid = trim($_REQUEST[hardid]);
	$org = trim($_REQUEST[org]);
	
	// these not used in license file, but put to database for records
	$name = trim($_REQUEST[name]);
	$email = trim($_REQUEST[email]);
	$phone = trim($_REQUEST[phone]);
	$comments = trim($_REQUEST[comments]);
	
	// now determine if this request is a valid one.
	// if not completely valid, we will return a [Settings] ini format
	// if completely valid, we will issue a license file
	$isRequestComnplete = true;
	if (strlen($hardid)!=14 || $hardid{4}!='-' || $hardid{9}!='-')
		return;
	// no hard ware ID, so just return empty
		
	if (!filter_var($email, FILTER_VALIDATE_EMAIL))
		$isRequestComnplete = false;
	else if (strlen($name) < 2)
		$isRequestComnplete = false;
	else if (strlen($org) < 2)
		$isRequestComnplete = false;	
		
	$strToShow = "[Setting]\r\nhardid=";  
	$strToShow .= $hardid;
	$strToShow .= "\nname=";
	$strToShow .= $name;
	$strToShow .= "\norg=";
	$strToShow .= $org;
	$strToShow .= "\nemail=";
	$strToShow .= $email;
	$strToShow .= "\nphone=";
	$strToShow .= $phone;
	$strToShow .= "\ncomments=";
	$strToShow .= $comments;
			
	if ($isRequestComnplete) {
		$finalCode =  getFinalCode($hardid, "FULL", 30, $org);
		$finalCode = XOR_encrypt($finalCode, "Miche66c1963");
		echo base64_encode($finalCode);
		
		// now mail
		ini_set("SMTP", "msa.hinet.net");	
		$to = "seiswide@gmail.com"; 
		$subject = "License issued to: " . $name . ', ' . $org . ', ' . $hardid; 
		// $body = "Hi,\n\nHow are you?"; 
		$headers = 'From: deping.chian@gmail.com' . "\r\n" .
	    'Reply-To: deping.chian@gmail.com' . "\r\n" .
	    'X-Mailer: PHP/' . phpversion();
	    mail($to, $subject, $strToShow, $headers);
	}
	else {
		// now only return the fields information
		echo $strToShow;
		/*
		echo "[Setting]";
		echo "\nhardid=";  echo $hardid;
		echo "\nname=";  echo $name;
		echo "\norg=";  echo $org;
		echo "\nemail="; echo $email;
		echo "\nphone="; echo $phone;
		echo "\ncomments="; echo $comments;
		*/
	}
	
	//	  $magicNumber = getMagicNumberFromString("0050-56C0-0008####FULL");  -->  1156
	//    $finalCode =  0050-56C0-0008####FULL1156####30####11/16/2014####01/15/2015####Deping at Home
	//    XOR_Encryption(base64_encode($finalCode))

	function getMagicNumberFromString($str)
	{
		$res = 0;
		for ($i=0; $i<strlen($str); $i++) {
			$res += ord($str{$i});
		}
		return $res;
	}
	
	function getFinalCode($hardid, $atype, $numdays, $org) {
		$startdate = date("m/d/Y");  // today
		$interval = new DateInterval('P' . $numdays . 'D');
		$endDate = new DateTime($startdate);
		$endDate->add($interval);
		
		$start = $hardid . "####" . $atype;
		return  $start . getMagicNumberFromString($start) . "####" .  
			$numdays . "####" .
			$startdate . "####" .  
			$endDate->format('m/d/Y') . "####" .  $org;
	}
	// http://globalrescue.hopto.org/seiswide/lic.php?hardid=0050-56C0-0008&org=Deping%20at%20Home
	
function XOR_encrypt($message, $key){
  $ml = strlen($message);
  $kl = strlen($key);
  $newmsg = "";

  for ($i = 0; $i < $ml; $i++){
    $newmsg .= $message{$i} ^ chr(ord($key{$i % $kl}) & 10 );
  }
     
  return $newmsg;
}


	
?>	