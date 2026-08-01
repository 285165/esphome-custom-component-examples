#include "openthread_vendor_info.h"
#include "esphome/core/defines.h"

#ifdef USE_OPENTHREAD
#include <esp_openthread.h>
#include <esp_openthread_lock.h>
#include <openthread/instance.h>
#include <openthread/netdiag.h>
#endif

namespace esphome {
namespace openthread_vendor_info {

static const char *const TAG = "openthread_vendor_info";

void OpenThreadVendorInfoComponent::setup() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info external component starting");
}

void OpenThreadVendorInfoComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info:");
  ESP_LOGCONFIG(TAG, "  Vendor Name: %s", this->vendor_name_.empty() ? "<not set>" : this->vendor_name_.c_str());
  ESP_LOGCONFIG(TAG, "  Vendor Model: %s", this->vendor_model_.empty() ? "<not set>" : this->vendor_model_.c_str());
  ESP_LOGCONFIG(TAG, "  Vendor SW Version: %s", this->vendor_sw_version_.empty() ? "<not set>" : this->vendor_sw_version_.c_str());
  ESP_LOGCONFIG(TAG, "  Vendor App URL: %s", this->vendor_app_url_.empty() ? "<not set>" : this->vendor_app_url_.c_str());
  ESP_LOGCONFIG(TAG, "  Reapply on every poll: %s", YESNO(this->set_on_every_retry_));
  ESP_LOGCONFIG(TAG, "  Update interval: %u ms", this->get_update_interval());
}

void OpenThreadVendorInfoComponent::update() {
  if (this->applied_ && !this->set_on_every_retry_) {
    return;
  }

  if (this->apply_()) {
    this->applied_ = true;
    ESP_LOGI(TAG, "OpenThread vendor diagnostic info applied");
  }
}

bool OpenThreadVendorInfoComponent::apply_() {
#ifndef USE_OPENTHREAD
  ESP_LOGW(TAG, "USE_OPENTHREAD is not enabled; add the ESPHome openthread: component first");
  return false;
#else
#ifndef OPENTHREAD_CONFIG_NET_DIAG_VENDOR_INFO_SET_API_ENABLE
  ESP_LOGW(TAG, "OpenThread vendor info set API is not enabled in this ESP-IDF/OpenThread build");
  return false;
#else
  otInstance *instance = esp_openthread_get_instance();
  if (instance == nullptr) {
    ESP_LOGD(TAG, "OpenThread instance is not ready yet");
    return false;
  }

  esp_openthread_lock_acquire(portMAX_DELAY);

  otError err = OT_ERROR_NONE;
  if (!this->vendor_name_.empty()) {
    err = otThreadSetVendorName(instance, this->vendor_name_.c_str());
    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG, "otThreadSetVendorName failed: %d", static_cast<int>(err));
      return false;
    }
  }

  if (!this->vendor_model_.empty()) {
    err = otThreadSetVendorModel(instance, this->vendor_model_.c_str());
    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG, "otThreadSetVendorModel failed: %d", static_cast<int>(err));
      return false;
    }
  }

  if (!this->vendor_sw_version_.empty()) {
    err = otThreadSetVendorSwVersion(instance, this->vendor_sw_version_.c_str());
    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG, "otThreadSetVendorSwVersion failed: %d", static_cast<int>(err));
      return false;
    }
  }

  if (!this->vendor_app_url_.empty()) {
    err = otThreadSetVendorAppUrl(instance, this->vendor_app_url_.c_str());
    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG, "otThreadSetVendorAppUrl failed: %d", static_cast<int>(err));
      return false;
    }
  }

  esp_openthread_lock_release();
  return true;
#endif
#endif
}

}  // namespace openthread_vendor_info
}  // namespace esphome
