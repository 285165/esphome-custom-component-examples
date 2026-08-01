#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <string>
#include <cstdint>

namespace esphome {
namespace openthread_vendor_info {

class OpenThreadVendorInfoComponent : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_vendor_name(const std::string &value) { this->vendor_name_ = value; }
  void set_vendor_model(const std::string &value) { this->vendor_model_ = value; }
  void set_vendor_sw_version(const std::string &value) { this->vendor_sw_version_ = value; }
  void set_vendor_app_url(const std::string &value) { this->vendor_app_url_ = value; }
  void set_tx_power_dbm(int value) {
    this->tx_power_dbm_ = static_cast<int8_t>(value);
    this->has_tx_power_ = true;
  }
  void set_set_on_every_retry(bool value) { this->set_on_every_retry_ = value; }

 protected:
  bool apply_();

  std::string vendor_name_;
  std::string vendor_model_;
  std::string vendor_sw_version_;
  std::string vendor_app_url_;
  int8_t tx_power_dbm_{0};
  bool has_tx_power_{false};
  bool applied_{false};
  bool set_on_every_retry_{false};
};

}  // namespace openthread_vendor_info
}  // namespace esphome