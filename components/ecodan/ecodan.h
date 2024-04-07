#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <string>

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "proto.h"
#include "status.h"

namespace esphome {
namespace ecodan 
{    
    class EcodanHeatpump : public PollingComponent, public esphome::api::CustomAPIDevice {
    public:        
        EcodanHeatpump() : PollingComponent(10000) {}
        void set_rx(int rx);
        void set_tx(int tx);
        void setup() override;
        void update() override;
        void dump_config() override;    
    
        void register_sensor(sensor::Sensor *obj, const std::string& key) {
            sensors[key] = obj;
        }

        void register_textSensor(text_sensor::TextSensor *obj, const std::string& key) {
            textSensors[key] = obj;
        }

        // exposed as external component commands
        void set_z1_target_temperature(float value);
        void set_z1_flow_target_temperature(float value);
        void set_dhw_target_temperature(float value);
        void set_dhw_mode(std::string mode);
        void set_dhw_force(bool on);
        void set_power_mode(bool on);
        void set_hp_mode(int mode);

    protected:
        std::map<std::string, sensor::Sensor*> sensors;
        std::map<std::string, text_sensor::TextSensor*> textSensors;

        // publish func
        void publish_state(const std::string& sensorKey, float sensorValue);
        void publish_state(const std::string& sensorKey, const std::string& sensorValue);

        std::string get_device_model();

        float get_temperature_step();
        float get_min_thermostat_temperature();
        float get_max_thermostat_temperature();
        float get_min_dhw_temperature();
        float get_max_dhw_temperature();
        float get_min_flow_target_temperature(std::string mode);
        float get_max_flow_target_temperature(std::string mode);

        bool begin_connect();
        bool begin_update_status();

        bool initialize();
        void handle_loop();
        bool is_connected();        
    
    private:
        HardwareSerial& port = Serial1;
        uint8_t serialRxPort{2};
        uint8_t serialTxPort{1};        
        
        std::thread serialRxThread;
        std::queue<Message> cmdQueue;
        std::mutex cmdQueueMutex;

        Status status;
        float temperatureStep = 0.5f;
        bool connected = false;
        bool heatpumpInitialized = false;
       
        void resync_rx();
        bool serial_rx(Message& msg);
        bool serial_tx(Message& msg);

        bool dispatch_next_cmd();
        void clear_command_queue();
        bool begin_get_status();
        
        void handle_response();
        void handle_get_response(Message& res);
        void handle_set_response(Message& res);
        void handle_connect_response(Message& res);
        void handle_ext_connect_response(Message& res);

        void serial_rx_thread();
    };

} // namespace ecodan
} // namespace esphome