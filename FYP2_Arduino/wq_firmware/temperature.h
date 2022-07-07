#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);


float get_temp_in_celsius()
{
  sensors.requestTemperatures();
  float Celsius = sensors.getTempCByIndex(0);
  return Celsius;
}

float get_temp_in_fahrenheit()
{
  float Celsius = get_temp_in_celsius();
  float Fahrenheit = sensors.toFahrenheit(Celsius);
  return Fahrenheit;
}
