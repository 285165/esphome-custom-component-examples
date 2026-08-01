#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace openthread_rssi {

class OpenThreadRSSIComponent : public PollingComponent {
 public:
  // Setters wired up from sensor.py
  void set_parent_avg_rssi_sensor(sensor::Sensor *s) { this->parent_avg_rssi_sensor_ = s; }
  void set_parent_last_rssi_sensor(sensor::Sensor *s) { this->parent_last_rssi_sensor_ = s; }
  void set_neighbor_best_rssi_sensor(sensor::Sensor *s) { this->neighbor_best_rssi_sensor_ = s; }
  void set_neighbor_avg_rssi_sensor(sensor::Sensor *s) { this->neighbor_avg_rssi_sensor_ = s; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  sensor::Sensor *parent_avg_rssi_sensor_{nullptr};
  sensor::Sensor *parent_last_rssi_sensor_{nullptr};
  sensor::Sensor *neighbor_best_rssi_sensor_{nullptr};
  sensor::Sensor *neighbor_avg_rssi_sensor_{nullptr};
};

}  // namespace openthread_rssi
}  // namespace esphome
