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

if($debug == 3) {
	echo 9;
	exit();
}

$sunrise = date_sunrise(time(), SUNFUNCS_RET_TIMESTAMP, 50.967779,-0.114799);
$sunset = date_sunset(time(), SUNFUNCS_RET_TIMESTAMP, 50.967779,-0.114799);

// Time check - So that light is not on overnight
// If it's 11pm or before 7am, then output 10 and exit
// 10 in the Arduino code turns off all LEDs
if(date('H')>22 or date('H')<7) {
	echo 10;
	exit();
}

// Define our current condition file
$cond_file='/var/www/html/weather/api.cond';
$cond_code=file_get_contents($cond_file);
// Switch on $cond_code to set code defined by openweathermap.org
switch ($cond_code) {
	case ($cond_code >= 200 and $cond_code < 300):
		echo 1; // Thunderstorm
		break;
	case ($cond_code >= 300 and $cond_code < 500):
		echo 2; // Drizzle
		break;
	case ($cond_code >= 500 and $cond_code < 600):
		echo 3; // Rain
		break;
	case ($cond_code >= 600 and $cond_code < 700):
		echo 4; // Snow
		break;
	case ($cond_code >= 700 and $cond_code < 800):
		echo 5; // Misty/fog
		break;
	case ($cond_code == 800):
		if(time()>=$sunrise and time()<=$sunset) {
			echo 6; // Clear day
		} else {
			echo 7; // Clear night
		}
		break;
	case ($cond_code > 800):
		if(time()>=$sunrise and time()<=$sunset) {
			echo $cond_code; // Cloudy
		} else {
			echo 7; // Clear night
		}
		break;
	case ($cond_code == 9):
		echo 9; // Demo reel
		break;
}
