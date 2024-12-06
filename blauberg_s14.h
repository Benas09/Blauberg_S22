#include "esphome.h"

class BlaubergS14 : public Component, public UARTDevice, public CustomAPIDevice {
    static const int S14_SPEED_0 = 0x00;
    static const int S14_SPEED_1 = 0x01;
    static const int S14_SPEED_2 = 0x02;
    static const int S14_SPEED_3 = 0x03;

    static const int S14_DAMPER_OFF = 0x00;
    static const int S14_DAMPER_ON = 0x08;

    static const int S14_RESET_FILTER_OFF = 0x00;
    static const int S14_RESET_FILTER_ON = 0x04;

    static const int S14_RESPONSE_OK = 0x00;
    static const int S14_RESPONSE_FILTER_REPLACEMENT_REQUIRED = 0x01;
    static const int S14_RESPONSE_DEFROSTING_REQUIRED = 0x02;

    static const int DEFROSTING_TIME = 120000;

    public:
        Sensor *sensor_response;
        Sensor *sensor_speed;
        BinarySensor *sensor_damper;
        BinarySensor *sensor_isDefrosting;
        BinarySensor *sensor_filterReplacementRequired;

        BlaubergS14(UARTComponent *parent) : UARTDevice(parent) {
            this->sensor_response = new Sensor();
            this->sensor_speed = new Sensor();
            this->sensor_damper = new BinarySensor();
            this->sensor_isDefrosting = new BinarySensor();
            this->sensor_filterReplacementRequired = new BinarySensor();

            this->sensor_response->publish_state(this->lastResponse);
            this->sensor_speed->publish_state(this->currentSpeed);
            this->sensor_damper->publish_state(this->get_damper());
            this->sensor_isDefrosting->publish_state(this->isDefrosting);
            this->sensor_filterReplacementRequired->publish_state(this->filterReplacementRequired);
        }

        static BlaubergS14* instance(UARTComponent *parent)
        {
            static BlaubergS14* INSTANCE = new BlaubergS14(parent);
            return INSTANCE;
        }

        void setup() override {
        }

        void loop() override {
            if (
                0 == this->lastMillis
                || this->lastMillis + 90 < millis()
            ) {
                if (
                    0 == this->defrostingFromMillis ||
                    millis() - this->defrostingFromMillis > DEFROSTING_TIME
                ) {
                    write(this->currentSpeed | this->currentDamper | this->currentResetFilter);

                    if (this->isDefrosting) {
                        this->isDefrosting = false;
                        
                        this->sensor_isDefrosting->publish_state(this->isDefrosting);
                    }
                } else {
                    write(S14_SPEED_0 | this->currentDamper | this->currentResetFilter);

                    if (!this->isDefrosting) {
                        this->isDefrosting = true;
                        
                        this->sensor_isDefrosting->publish_state(this->isDefrosting);
                    }
                }
                
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
                    if (S14_RESPONSE_DEFROSTING_REQUIRED == response) {
                        this->defrostingFromMillis = millis();
                    }

                    if ((S14_RESPONSE_FILTER_REPLACEMENT_REQUIRED == response) != this->filterReplacementRequired) {
                        this->filterReplacementRequired = S14_RESPONSE_FILTER_REPLACEMENT_REQUIRED == response;

                        if (!this->filterReplacementRequired) {
                            this->set_resetFilter(false);
                        }

                        this->sensor_filterReplacementRequired->publish_state(this->filterReplacementRequired);
                    }

                    this->sensor_response->publish_state(response);
                }
                
                this->lastResponse = response;
                this->lastMillis = millis();
            }
        }

        bool is_speed(int speed) {
            switch (speed) {
                case 3:
                    return this->currentSpeed == S14_SPEED_3;
                case 2:
                    return this->currentSpeed == S14_SPEED_2;
                case 1:
                    return this->currentSpeed == S14_SPEED_1;
                case 0:
                    return this->currentSpeed == S14_SPEED_0;
                default:
                    return false;
            }
        }

        void set_speed(int speed) {
            if (3 == speed) {
                this->currentSpeed = S14_SPEED_3;
            } else if (2 == speed) {
                this->currentSpeed = S14_SPEED_2;
            } else if (1 == speed) {
                this->currentSpeed = S14_SPEED_1;
            } else {
                this->currentSpeed = S14_SPEED_0;
            }

            this->sensor_speed->publish_state(this->currentSpeed);
        }

        bool get_damper() {
            return this->currentDamper == S14_DAMPER_ON;
        }

        void set_damper(bool damper) {
            this->currentDamper = damper ? S14_DAMPER_ON : S14_DAMPER_OFF;
            this->sensor_damper->publish_state(this->get_damper());
        }

        bool get_resetFilter() {
            return this->currentResetFilter == S14_RESET_FILTER_ON;
        }

        void set_resetFilter(bool resetFilter) {
            this->currentResetFilter = resetFilter ? S14_RESET_FILTER_ON : S14_RESET_FILTER_OFF;
        }

    private:
        long lastMillis = 0;
        long defrostingFromMillis = 0;
        bool isDefrosting = false;
        bool filterReplacementRequired = false;
        int currentSpeed = S14_SPEED_0;
        int currentDamper = S14_DAMPER_OFF;
        int currentResetFilter = S14_RESET_FILTER_OFF;
        int lastResponse = S14_RESPONSE_OK;
};