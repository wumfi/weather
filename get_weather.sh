output=$(curl -s "https://api.openweathermap.org/data/2.5/weather?lat=50.967779&lon=-.114799&appid=f8d0bdd6dfb78b81f9bc0baf247c0002" | jq .weather[].id)
mysql -BN -hplexnas.wumfi.com -Dsmart_control -uwumfi -pBaileyDog -e "update weather set code=${output};"
