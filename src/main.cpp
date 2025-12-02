// Signal K application file.
//

#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>

#include <memory>

#include "Arduino.h"
#include "sensesp.h"
#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp/transforms/frequency.h"
#include "sensesp/transforms/linear.h"
#include "sensesp/transforms/moving_average.h"
#include "sensesp_app_builder.h"
#include "sensesp_onewire/onewire_temperature.h"

using namespace sensesp;
using namespace sensesp::onewire;

//#define BMP_SENSOR_ACTIVE

Adafruit_BMP280 bmp280; // I2C
Adafruit_BME280 bme280; // I2C

float read_bmp_temp_callback() { return (bmp280.readTemperature());}
float read_bmp_pressure_callback() { return (bmp280.readPressure());}

float read_bme_temp_callback() { return (bme280.readTemperature());}
float read_bme_pressure_callback() { return (bme280.readPressure());}


// The setup function performs one-time application initialization.
void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("vp-motor-sensor")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi_client("Trevangsvej_197", "xx")
                    //->set_wifi_access_point("vp-motor-sensor", "xx")
                    //->set_sk_server("192.168.1.180", 80)
                    //->set_sk_server("openplotter.local", 80)
                    ->enable_uptime_sensor()
                    ->get_app();

  Wire.begin();

#ifdef BMP_SENSOR_ACTIVE
  /// Engine Room BMP Temp Sensor ////  
  bmp280.begin();


  // Create a RepeatSensor with float output that reads the temperature
  // using the function defined above.
  auto* engine_bmp_room_temp =
      new RepeatSensor<float>(5000, read_bmp_temp_callback);

  auto* engine_bmp_room_pressure = 
      new RepeatSensor<float>(60000, read_bmp_pressure_callback);

  // Send the temperature to the Signal K server as a Float
// Set the Signal K Path for the output
  const char* sk_path_temp = "propulsion.engineRoom.temperature";
  // Send the temperature to the Signal K server as a Float
  engine_bmp_room_temp->connect_to(new SKOutputFloat(sk_path_temp));

  const char* sk_path_pressure = "propulsion.engineRoom.pressure";
  // Send the temperature to the Signal K server as a Float
  engine_bmp_room_pressure->connect_to(new SKOutputFloat(sk_path_pressure));
#else

  // Engine Room BME Temp Sensor 
  bme280.begin(); 


  // Create a RepeatSensor with float output that reads the temperature
  // using the function defined above.
  auto* engine_bme_room_temp =
      new RepeatSensor<float>(5000, read_bme_temp_callback);

  auto* engine_bme_room_pressure = 
      new RepeatSensor<float>(60000, read_bme_pressure_callback);

  // Send the temperature to the Signal K server as a Float
// Set the Signal K Path for the output
  const char* sk_path_temp = "propulsion.engineRoom.temperature";
  // Send the temperature to the Signal K server as a Float
  engine_bme_room_temp->connect_to(new SKOutputFloat(sk_path_temp));

  const char* sk_path_pressure = "propulsion.engineRoom.pressure";
  // Send the temperature to the Signal K server as a Float
  engine_bme_room_pressure->connect_to(new SKOutputFloat(sk_path_pressure));
#endif

  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { event_loop()->tick(); }
