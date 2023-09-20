#include "esphome.h"

class BlaubergS22 : public Component, public UARTDevice, public CustomAPIDevice {
    static const int S22_SPEED_0 = 0x00;
    static const int S22_SPEED_1 = 0x01;
    static const int S22_SPEED_2 = 0x02;
    static const int S22_SPEED_3 = 0x03;

    static const int S22_DAMPER_OFF = 0x00;
    static const int S22_DAMPER_ON = 0x08;

    public:
        Sensor *sensor_response;
        Sensor *sensor_speed;
        BinarySensor *sensor_damper;

        BlaubergS22(UARTComponent *parent) : UARTDevice(parent) {
            this->sensor_response = new Sensor();
            this->sensor_speed = new Sensor();
            this->sensor_damper = new BinarySensor();

            this->sensor_response->publish_state(this->lastResponse);
            this->sensor_speed->publish_state(this->currentSpeed);
            this->sensor_damper->publish_state(this->get_damper());
        }

        static BlaubergS22* instance(UARTComponent *parent)
        {
            static BlaubergS22* INSTANCE = new BlaubergS22(parent);
            return INSTANCE;
        }

        void setup() override {
        }

        void loop() override {
            if (
                0 == this->lastMillis
                || this->lastMillis + 90 < millis()
            ) {
                write(this->currentSpeed | this->currentDamper);
                this->lastMillis = millis();
            }

            bool responseReceived = false;
            int response = 0x00;

            while (available()) {
                response = read();
                responseReceived = true;
            }

            if (responseReceived) {
                if (this->lastResponse != response) {
                    this->sensor_response->publish_state(response);
                }
                
                this->lastResponse = response;
                this->lastMillis = millis();
            }
        }

        bool is_speed(int speed) {
            switch (speed) {
                case 3:
                    return this->currentSpeed == S22_SPEED_3;
                case 2:
                    return this->currentSpeed == S22_SPEED_2;
                case 1:
                    return this->currentSpeed == S22_SPEED_1;
                case 0:
                    return this->currentSpeed == S22_SPEED_0;
                default:
                    return false;
            }
        }

        void set_speed(int speed) {
            if (3 == speed) {
                this->currentSpeed = S22_SPEED_3;
            } else if (2 == speed) {
                this->currentSpeed = S22_SPEED_2;
            } else if (1 == speed) {
                this->currentSpeed = S22_SPEED_1;
            } else {
                this->currentSpeed = S22_SPEED_0;
            }

            this->sensor_speed->publish_state(this->currentSpeed);
        }

        bool get_damper() {
            return this->currentDamper == S22_DAMPER_ON;
        }

        void set_damper(bool damper) {
            this->currentDamper = damper ? S22_DAMPER_ON : S22_DAMPER_OFF;
            this->sensor_damper->publish_state(this->get_damper());
        }

    private:
        long lastMillis = 0;
        int currentSpeed = S22_SPEED_0;
        int currentDamper = S22_DAMPER_OFF;
        int lastResponse = 0x00;
};
