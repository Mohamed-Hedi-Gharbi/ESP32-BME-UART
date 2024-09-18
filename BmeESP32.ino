#include "Arduino.h"
#include "bme68xLibrary.h"
#include <esp_sleep.h>
#include <driver/uart.h>


#define SDA_PIN 2//13 
#define SCL_PIN 15

Bme68x bme;
static float P, G, H, T; 
static uint32_t  Press_int = 0, Gas_int = 0 ;
static uint16_t Temp_int = 0, Hum_int = 0 ;

// tableau de 12 octet --- 1 octet = 8 bits ---
static uint8_t sendBuffer[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // Initialisation du buffer avec des zéros
 

 

void setup() {
    // Initialize I2C with custom pins for BME sensor
    Wire.begin(SDA_PIN, SCL_PIN);
    int wake_thresh = 3;

    uart_set_wakeup_threshold(UART_NUM_0, wake_thresh);
    esp_sleep_enable_uart_wakeup(UART_NUM_0);


    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }

    //pinMode(4, OUTPUT);

    // Initialize BME sensor
    bme.begin(0x76, Wire);
    if (bme.checkStatus()) {
        if (bme.checkStatus() == BME68X_ERROR) {
            return;
        } else if (bme.checkStatus() == BME68X_WARNING) {
        }
    }

    // Set the default configuration for temperature, pressure, and humidity
    bme.setTPH();

    // Set the heater configuration to 300 deg C for 100ms for Forced mode
    bme.setHeaterProf(300, 100);
}

void loop() {
          esp_light_sleep_start();


          Serial.write(sendBuffer, 12);


          bme68xData data;
          bme.setOpMode(BME68X_FORCED_MODE);
          delayMicroseconds(bme.getMeasDur());

          if (bme.fetchData()) {
            bme.getData(data);

           T = data.temperature *100;
           H = data.humidity * 100;
           P = data.pressure * 100;
           G = data.gas_resistance * 100;


        
           // Convert float To int
           Press_int = (uint32_t) P;
           Gas_int = (uint32_t) G;
           Hum_int = (uint16_t) H ;
           Temp_int = (uint16_t) T;


           // Update buffer with new data
           sendBuffer[0] = (uint8_t) ((Press_int >> 24) & 0xFF) ;
           sendBuffer[1] = (uint8_t) ((Press_int >> 16) & 0xFF);
           sendBuffer[2] = (uint8_t) ((Press_int >> 8) & 0xFF );
           sendBuffer[3] = (uint8_t) (Press_int & 0xFF);
           sendBuffer[4] = (uint8_t) ((Gas_int >> 24) & 0xFF );
           sendBuffer[5] = (uint8_t) ((Gas_int >> 16) & 0xFF);
           sendBuffer[6] = (uint8_t) ((Gas_int >> 8) & 0xFF);
           sendBuffer[7] = (uint8_t) (Gas_int & 0xFF);
           sendBuffer[8] = (uint8_t) ((Hum_int >> 8) & 0xFF);
           sendBuffer[9] = (uint8_t) (Hum_int & 0xFF);
           sendBuffer[10] = (uint8_t) ((Temp_int >> 8) & 0XFF);
           sendBuffer[11] = (uint8_t) (Temp_int & 0xFF);
         }
 
} 



