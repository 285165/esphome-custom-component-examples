#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace openthread_diagnostics {

class OpenThreadDiagnostics : public PollingComponent {
 public:
  void dump_config() override;
  void update() override;

  void set_channel_sensor(sensor::Sensor *sensor) { channel_sensor_ = sensor; }
  void set_tx_power_sensor(sensor::Sensor *sensor) { tx_power_sensor_ = sensor; }
  void set_parent_rssi_sensor(sensor::Sensor *sensor) { parent_rssi_sensor_ = sensor; }
  void set_parent_average_rssi_sensor(sensor::Sensor *sensor) { parent_average_rssi_sensor_ = sensor; }
  void set_parent_link_margin_sensor(sensor::Sensor *sensor) { parent_link_margin_sensor_ = sensor; }
  void set_parent_link_quality_sensor(sensor::Sensor *sensor) { parent_link_quality_sensor_ = sensor; }
  void set_parent_age_sensor(sensor::Sensor *sensor) { parent_age_sensor_ = sensor; }
  void set_frame_error_rate_sensor(sensor::Sensor *sensor) { frame_error_rate_sensor_ = sensor; }
  void set_message_error_rate_sensor(sensor::Sensor *sensor) { message_error_rate_sensor_ = sensor; }
  void set_neighbor_count_sensor(sensor::Sensor *sensor) { neighbor_count_sensor_ = sensor; }
  void set_child_count_sensor(sensor::Sensor *sensor) { child_count_sensor_ = sensor; }
  void set_mac_tx_total_sensor(sensor::Sensor *sensor) { mac_tx_total_sensor_ = sensor; }
  void set_mac_rx_total_sensor(sensor::Sensor *sensor) { mac_rx_total_sensor_ = sensor; }
  void set_mac_tx_err_cca_sensor(sensor::Sensor *sensor) { mac_tx_err_cca_sensor_ = sensor; }
  void set_mac_tx_retry_sensor(sensor::Sensor *sensor) { mac_tx_retry_sensor_ = sensor; }

  void set_role_text_sensor(text_sensor::TextSensor *sensor) { role_text_sensor_ = sensor; }
  void set_network_name_text_sensor(text_sensor::TextSensor *sensor) { network_name_text_sensor_ = sensor; }
  void set_thread_version_text_sensor(text_sensor::TextSensor *sensor) { thread_version_text_sensor_ = sensor; }
  void set_ext_address_text_sensor(text_sensor::TextSensor *sensor) { ext_address_text_sensor_ = sensor; }
  void set_pan_id_text_sensor(text_sensor::TextSensor *sensor) { pan_id_text_sensor_ = sensor; }
  void set_partition_id_text_sensor(text_sensor::TextSensor *sensor) { partition_id_text_sensor_ = sensor; }
  void set_leader_router_id_text_sensor(text_sensor::TextSensor *sensor) { leader_router_id_text_sensor_ = sensor; }
  void set_rloc16_text_sensor(text_sensor::TextSensor *sensor) { rloc16_text_sensor_ = sensor; }
  void set_mesh_local_eid_text_sensor(text_sensor::TextSensor *sensor) { mesh_local_eid_text_sensor_ = sensor; }
  void set_ip_addresses_text_sensor(text_sensor::TextSensor *sensor) { ip_addresses_text_sensor_ = sensor; }
  void set_mesh_local_address_text_sensor(text_sensor::TextSensor *sensor) { mesh_local_address_text_sensor_ = sensor; }
  void set_link_local_address_text_sensor(text_sensor::TextSensor *sensor) { link_local_address_text_sensor_ = sensor; }

 protected:
  sensor::Sensor *channel_sensor_{nullptr};
  sensor::Sensor *tx_power_sensor_{nullptr};
  sensor::Sensor *parent_rssi_sensor_{nullptr};
  sensor::Sensor *parent_average_rssi_sensor_{nullptr};
  sensor::Sensor *parent_link_margin_sensor_{nullptr};
  sensor::Sensor *parent_link_quality_sensor_{nullptr};
  sensor::Sensor *parent_age_sensor_{nullptr};
  sensor::Sensor *frame_error_rate_sensor_{nullptr};
  sensor::Sensor *message_error_rate_sensor_{nullptr};
  sensor::Sensor *neighbor_count_sensor_{nullptr};
  sensor::Sensor *child_count_sensor_{nullptr};
  sensor::Sensor *mac_tx_total_sensor_{nullptr};
  sensor::Sensor *mac_rx_total_sensor_{nullptr};
  sensor::Sensor *mac_tx_err_cca_sensor_{nullptr};
  sensor::Sensor *mac_tx_retry_sensor_{nullptr};

  text_sensor::TextSensor *role_text_sensor_{nullptr};
  text_sensor::TextSensor *network_name_text_sensor_{nullptr};
  text_sensor::TextSensor *thread_version_text_sensor_{nullptr};
  text_sensor::TextSensor *ext_address_text_sensor_{nullptr};
  text_sensor::TextSensor *pan_id_text_sensor_{nullptr};
  text_sensor::TextSensor *partition_id_text_sensor_{nullptr};
  text_sensor::TextSensor *leader_router_id_text_sensor_{nullptr};
  text_sensor::TextSensor *rloc16_text_sensor_{nullptr};
  text_sensor::TextSensor *mesh_local_eid_text_sensor_{nullptr};
  text_sensor::TextSensor *ip_addresses_text_sensor_{nullptr};
  text_sensor::TextSensor *mesh_local_address_text_sensor_{nullptr};
  text_sensor::TextSensor *link_local_address_text_sensor_{nullptr};
};

}  // namespace openthread_diagnostics
}  // namespace esphome
