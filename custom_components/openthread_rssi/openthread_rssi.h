#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace openthread_rssi {

class OpenThreadRSSIComponent : public PollingComponent {
 public:
  // RSSI (dBm)
  void set_parent_avg_rssi_sensor(sensor::Sensor *s) { this->parent_avg_rssi_sensor_ = s; }
  void set_parent_last_rssi_sensor(sensor::Sensor *s) { this->parent_last_rssi_sensor_ = s; }
  void set_neighbor_best_rssi_sensor(sensor::Sensor *s) { this->neighbor_best_rssi_sensor_ = s; }
  void set_neighbor_avg_rssi_sensor(sensor::Sensor *s) { this->neighbor_avg_rssi_sensor_ = s; }

  // Link Quality (LQI 0-3) and Link Margin (dB)
  void set_parent_link_quality_in_sensor(sensor::Sensor *s) { this->parent_link_quality_in_sensor_ = s; }
  void set_parent_link_quality_out_sensor(sensor::Sensor *s) { this->parent_link_quality_out_sensor_ = s; }
  void set_parent_link_margin_sensor(sensor::Sensor *s) { this->parent_link_margin_sensor_ = s; }
  void set_neighbor_best_link_quality_sensor(sensor::Sensor *s) { this->neighbor_best_link_quality_sensor_ = s; }
  void set_neighbor_best_link_margin_sensor(sensor::Sensor *s) { this->neighbor_best_link_margin_sensor_ = s; }

  // Error rates (%)
  void set_neighbor_best_frame_error_rate_sensor(sensor::Sensor *s) { this->neighbor_best_frame_error_rate_sensor_ = s; }
  void set_neighbor_best_message_error_rate_sensor(sensor::Sensor *s) {
    this->neighbor_best_message_error_rate_sensor_ = s;
  }

  // Topology / freshness
  void set_neighbor_count_sensor(sensor::Sensor *s) { this->neighbor_count_sensor_ = s; }
  void set_neighbor_best_age_sensor(sensor::Sensor *s) { this->neighbor_best_age_sensor_ = s; }

  void set_noise_floor(int8_t noise_floor) { this->noise_floor_ = noise_floor; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  sensor::Sensor *parent_avg_rssi_sensor_{nullptr};
  sensor::Sensor *parent_last_rssi_sensor_{nullptr};
  sensor::Sensor *neighbor_best_rssi_sensor_{nullptr};
  sensor::Sensor *neighbor_avg_rssi_sensor_{nullptr};

  sensor::Sensor *parent_link_quality_in_sensor_{nullptr};
  sensor::Sensor *parent_link_quality_out_sensor_{nullptr};
  sensor::Sensor *parent_link_margin_sensor_{nullptr};
  sensor::Sensor *neighbor_best_link_quality_sensor_{nullptr};
  sensor::Sensor *neighbor_best_link_margin_sensor_{nullptr};

  sensor::Sensor *neighbor_best_frame_error_rate_sensor_{nullptr};
  sensor::Sensor *neighbor_best_message_error_rate_sensor_{nullptr};
  sensor::Sensor *neighbor_count_sensor_{nullptr};
  sensor::Sensor *neighbor_best_age_sensor_{nullptr};

  int8_t noise_floor_{-100};
};

}  // namespace openthread_rssi
}  // namespace esphome
