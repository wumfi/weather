api_key=$(cat api_key)
output=$(curl -s "https://api.openweathermap.org/data/2.5/weather?lat=50.967779&lon=-.114799&appid=${api_key}" | jq .weather[].id)
mysql -BN -hplexnas.wumfi.com -Dsmart_control -uwumfi -pBaileyDog -e "update weather set code=${output};"
