<?php
// Basic connection settings
$databaseHost = 'plexnas.wumfi.com';
$databaseUsername = 'wumfi';
$databasePassword = 'BaileyDog';
$databaseName = 'smart_control';

// Connect to the database
$mysqli = mysqli_connect($databaseHost, $databaseUsername, $databasePassword, $databaseName); 
?>
