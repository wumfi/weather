# Get our API key
api_key=$(cat api_key)

# Get current weather
curl "http://api.openweathermap.org/data/2.5/weather?id=2654308&appid=${api_key}&units=metric">current.sample

# Get forecast
curl "http://api.openweathermap.org/data/2.5/forecast?id=2654308&appid=${api_key}&units=metric">forecast.sample
