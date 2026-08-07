#pragma once

#include <string>
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace openthread_diagnostics {

class OpenThreadDiagnostics : public PollingComponent {
 public:
  void update() override;
  void dump_config() override;

  void set_channel_sensor(sensor::Sensor *s) { channel_sensor_ = s; }
  void set_pan_id_sensor(sensor::Sensor *s) { pan_id_sensor_ = s; }
  void set_rloc16_sensor(sensor::Sensor *s) { rloc16_sensor_ = s; }
  void set_partition_id_sensor(sensor::Sensor *s) { partition_id_sensor_ = s; }
  void set_leader_router_id_sensor(sensor::Sensor *s) { leader_router_id_sensor_ = s; }
  void set_tx_power_sensor(sensor::Sensor *s) { tx_power_sensor_ = s; }
  void set_neighbor_count_sensor(sensor::Sensor *s) { neighbor_count_sensor_ = s; }
  void set_child_count_sensor(sensor::Sensor *s) { child_count_sensor_ = s; }
  void set_parent_rssi_sensor(sensor::Sensor *s) { parent_rssi_sensor_ = s; }
  void set_parent_average_rssi_sensor(sensor::Sensor *s) { parent_average_rssi_sensor_ = s; }
  void set_parent_link_margin_sensor(sensor::Sensor *s) { parent_link_margin_sensor_ = s; }
  void set_parent_link_quality_sensor(sensor::Sensor *s) { parent_link_quality_sensor_ = s; }
  void set_parent_age_sensor(sensor::Sensor *s) { parent_age_sensor_ = s; }
  void set_frame_error_rate_sensor(sensor::Sensor *s) { frame_error_rate_sensor_ = s; }
  void set_message_error_rate_sensor(sensor::Sensor *s) { message_error_rate_sensor_ = s; }
  void set_mac_tx_total_sensor(sensor::Sensor *s) { mac_tx_total_sensor_ = s; }
  void set_mac_rx_total_sensor(sensor::Sensor *s) { mac_rx_total_sensor_ = s; }
  void set_mac_tx_err_cca_sensor(sensor::Sensor *s) { mac_tx_err_cca_sensor_ = s; }
  void set_mac_tx_retry_sensor(sensor::Sensor *s) { mac_tx_retry_sensor_ = s; }

  void set_role_text_sensor(text_sensor::TextSensor *s) { role_text_sensor_ = s; }
  void set_network_name_text_sensor(text_sensor::TextSensor *s) { network_name_text_sensor_ = s; }
  void set_ext_address_text_sensor(text_sensor::TextSensor *s) { ext_address_text_sensor_ = s; }
  void set_mesh_local_address_text_sensor(text_sensor::TextSensor *s) { mesh_local_address_text_sensor_ = s; }
  void set_link_local_address_text_sensor(text_sensor::TextSensor *s) { link_local_address_text_sensor_ = s; }
  void set_ip_addresses_text_sensor(text_sensor::TextSensor *s) { ip_addresses_text_sensor_ = s; }
  void set_thread_version_text_sensor(text_sensor::TextSensor *s) { thread_version_text_sensor_ = s; }

 protected:
  sensor::Sensor *channel_sensor_{nullptr};
  sensor::Sensor *pan_id_sensor_{nullptr};
  sensor::Sensor *rloc16_sensor_{nullptr};
  sensor::Sensor *partition_id_sensor_{nullptr};
  sensor::Sensor *leader_router_id_sensor_{nullptr};
  sensor::Sensor *tx_power_sensor_{nullptr};
  sensor::Sensor *neighbor_count_sensor_{nullptr};
  sensor::Sensor *child_count_sensor_{nullptr};
  sensor::Sensor *parent_rssi_sensor_{nullptr};
  sensor::Sensor *parent_average_rssi_sensor_{nullptr};
  sensor::Sensor *parent_link_margin_sensor_{nullptr};
  sensor::Sensor *parent_link_quality_sensor_{nullptr};
  sensor::Sensor *parent_age_sensor_{nullptr};
  sensor::Sensor *frame_error_rate_sensor_{nullptr};
  sensor::Sensor *message_error_rate_sensor_{nullptr};
  sensor::Sensor *mac_tx_total_sensor_{nullptr};
  sensor::Sensor *mac_rx_total_sensor_{nullptr};
  sensor::Sensor *mac_tx_err_cca_sensor_{nullptr};
  sensor::Sensor *mac_tx_retry_sensor_{nullptr};

  text_sensor::TextSensor *role_text_sensor_{nullptr};
  text_sensor::TextSensor *network_name_text_sensor_{nullptr};
  text_sensor::TextSensor *ext_address_text_sensor_{nullptr};
  text_sensor::TextSensor *mesh_local_address_text_sensor_{nullptr};
  text_sensor::TextSensor *link_local_address_text_sensor_{nullptr};
  text_sensor::TextSensor *ip_addresses_text_sensor_{nullptr};
  text_sensor::TextSensor *thread_version_text_sensor_{nullptr};
};

}  // namespace openthread_diagnostics
}  // namespace esphome
