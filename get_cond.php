<?php
// 17/1/2019
//
// Server side code to accompany arduino weather cloud
// Inspired by https://www.reddit.com/r/esp8266/comments/bzhzcg/my_ikea_drömsyn_weathertelling_light
//
// Will return a simplifed code as defined in https://openweathermap.org/weather-conditions
//
// Defined as:
// 	1 = Thunder
// 	2 = Drizzle
// 	3 = Rain
// 	4 = Snow
// 	5 = Misty/fog
// 	6 = Clear day
// 	7 = Clear night
// 	801-804 = Cloudy levels
// 	9 = Demo
//
// Arduino calls this to get above return code, then acts accordingly with pixels

// Debug mode:
// 	1 = Will operate normally
// 	2 = Will use predefined json to save API calls
// 	3 = Demo reel
$debug=1;

// Time check - So that light is not on overnight
// If it's 11pm or before 7am, then output 10 and exit
// 10 in the Arduino code turns off all LEDs
if(date('H')>22 or date('H')<7) {
	echo 10;
	exit();
}

// Define our current condition file
$cond_file='/var/www/html/weather/current.cond';

// If condition file does not exist, create it with dummy value
if(!is_file($cond_file)){
	$contents = 0;
	file_put_contents($cond_file, $contents);
}

// Load my API key and define curl URL
$apiKey=str_replace(array("\r\n", "\n", "\r"), '', file_get_contents('/var/www/html/weather/api_key'));
$url='http://api.openweathermap.org/data/2.5/weather?id=2654308&appid='.$apiKey.'&units=metric';

if($debug==1) {
	// Initiate cURL
	$ch=curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

	// Grab response
	$json=curl_exec($ch);
} else {
	$json='{"coord":{"lon":-0.13,"lat":50.96},"weather":[{"id":201,"main":"Clouds","description":"few clouds","icon":"02d"}],"base":"stations","main":{"temp":17.16,"pressure":1019,"humidity":77,"temp_min":15,"temp_max":18.89},"visibility":10000,"wind":{"speed":5.1,"deg":210},"clouds":{"all":20},"dt":1560761506,"sys":{"type":1,"id":1395,"message":0.0078,"country":"GB","sunrise":1560743143,"sunset":1560802623},"timezone":3600,"id":2654308,"name":"Burgess Hill","cod":200}';
}

// Convert response to an array
$ary = json_decode($json, true);

// Grab the current weather ID, or set demo reel
if($debug==3) {
	$cond_code=9;
} else {
	$cond_code=$ary['weather'][0]['id'];
}

// Switch on $cond_code to set code defined by openweathermap.org
switch ($cond_code) {
	case ($cond_code >= 200 and $cond_code < 300):
		$condition = 1; // Thunderstorm
		break;
	case ($cond_code >= 300 and $cond_code < 500):
		$condition = 2; // Drizzle
		break;
	case ($cond_code >= 500 and $cond_code < 600):
		$condition = 3; // Rain
		break;
	case ($cond_code >= 600 and $cond_code < 700):
		$condition = 4; // Snow
		break;
	case ($cond_code >= 700 and $cond_code < 800):
		$condition = 5; // Misty/fog
		break;
	case ($cond_code == 800):
		$sunrise = date_sunrise(time(), SUNFUNCS_RET_TIMESTAMP, 38.4, -9, 90, 1);
		$sunset = date_sunset(time(), SUNFUNCS_RET_TIMESTAMP, 38.4, -9, 90, 1);
		if(time()>=$sunrise and time()<=$sunset) {
			$condition = 6; // Clear day
		} else {
			$condition = 7; // Clear night
		}
		break;
	case ($cond_code > 800):
		$sunrise = date_sunrise(time(), SUNFUNCS_RET_TIMESTAMP, 38.4, -9, 90, 1);
		$sunset = date_sunset(time(), SUNFUNCS_RET_TIMESTAMP, 38.4, -9, 90, 1);
		if(time()>=$sunrise and time()<=$sunset) {
			$condition = $cond_code; // Cloudy
		} else {
			$condition = 7; // Clear night
		}
		break;
		break;
	case ($cond_code == 9):
		$condition = 9; // Demo reel
		break;
}

// Get the last value from our conditions file
$last=file_get_contents($cond_file);
// Only output condition if it's changed from last read
if ($last != $condition) {

	// Put condition code to stdout
	echo $condition;

	// Save the condition code to our conditions file
	file_put_contents($cond_file,$condition);
} else {

	// Nothing has changed, therefore output previous result
	echo $last;
}
?>
