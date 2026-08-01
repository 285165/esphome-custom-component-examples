#include "openthread_vendor_info.h"
#include "esphome/core/defines.h"

#ifdef USE_OPENTHREAD
#include <esp_openthread.h>
#include <esp_openthread_lock.h>

#include <openthread/instance.h>
#include <openthread/netdiag.h>
#include <openthread/thread.h>
#include <openthread/ip6.h>
#include <openthread/platform/radio.h>
#endif

namespace esphome {
namespace openthread_vendor_info {

static const char *const TAG = "openthread_vendor_info";

static const char *ot_error_to_string(otError err) {
  switch (err) {
    case OT_ERROR_NONE:
      return "OT_ERROR_NONE";
    case OT_ERROR_FAILED:
      return "OT_ERROR_FAILED";
    case OT_ERROR_INVALID_ARGS:
      return "OT_ERROR_INVALID_ARGS";
    case OT_ERROR_INVALID_STATE:
      return "OT_ERROR_INVALID_STATE";
    case OT_ERROR_NO_BUFS:
      return "OT_ERROR_NO_BUFS";
    case OT_ERROR_NOT_IMPLEMENTED:
      return "OT_ERROR_NOT_IMPLEMENTED";
    default:
      return "UNKNOWN";
  }
}

void OpenThreadVendorInfoComponent::setup() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info external component starting");
}

void OpenThreadVendorInfoComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info:");
  ESP_LOGCONFIG(TAG, "  Vendor Name: %s",
                this->vendor_name_.empty() ? "<not set>" : this->vendor_name_.c_str());
  ESP_LOGCONFIG(TAG, "  Vendor Model: %s",
                this->vendor_model_.empty() ? "<not set>" : this->vendor_model_.c_str());
  ESP_LOGCONFIG(TAG, "  Vendor SW Version: %s",
                this->vendor_sw_version_.empty() ? "<not set>" : this->vendor_sw_version_.c_str());
  if (this->has_tx_power_) {
    ESP_LOGCONFIG(TAG, "  TX Power: %d dBm", this->tx_power_dbm_);
  } else {
    ESP_LOGCONFIG(TAG, "  TX Power: <not set>");
  }
}

void OpenThreadVendorInfoComponent::update() {
  if (this->applied_ && !this->set_on_every_retry_) {
    return;
  }
  if (this->apply_()) {
    this->applied_ = true;
    ESP_LOGI(TAG, "OpenThread settings applied");
  }
}

bool OpenThreadVendorInfoComponent::apply_() {
#ifndef USE_OPENTHREAD
  ESP_LOGW(TAG, "USE_OPENTHREAD not enabled");
  return false;
#else

  otInstance *instance = esp_openthread_get_instance();
  if (instance == nullptr) {
    ESP_LOGD(TAG, "OpenThread instance not ready");
    return false;
  }

  esp_openthread_lock_acquire(portMAX_DELAY);

  bool ip6_enabled = otIp6IsEnabled(instance);
  otDeviceRole role = otThreadGetDeviceRole(instance);

  ESP_LOGI(TAG, "Thread state: ip6=%d role=%d", ip6_enabled, static_cast<int>(role));

  if (!ip6_enabled) {
    esp_openthread_lock_release();
    ESP_LOGD(TAG, "IPv6 not enabled yet");
    return false;
  }

  bool ok = true;

  // ---- Transmit power (platform radio API, dBm) ----
  if (this->has_tx_power_) {
    otError err = otPlatRadioSetTransmitPower(instance, this->tx_power_dbm_);
    if (err != OT_ERROR_NONE) {
      ESP_LOGW(TAG, "otPlatRadioSetTransmitPower(%d) failed: %d (%s)",
               this->tx_power_dbm_, static_cast<int>(err), ot_error_to_string(err));
      ok = false;
    } else {
      int8_t readback = 0;
      if (otPlatRadioGetTransmitPower(instance, &readback) == OT_ERROR_NONE) {
        ESP_LOGI(TAG, "TX power set to %d dBm (readback=%d dBm)",
                 this->tx_power_dbm_, readback);
      } else {
        ESP_LOGI(TAG, "TX power set to %d dBm", this->tx_power_dbm_);
      }
    }
  }

  // ---- Vendor info (requires OPENTHREAD_CONFIG_NET_DIAG_VENDOR_INFO_SET_API_ENABLE) ----
#ifdef OPENTHREAD_CONFIG_NET_DIAG_VENDOR_INFO_SET_API_ENABLE
  otError err;

  if (!this->vendor_name_.empty()) {
    err = otThreadSetVendorName(instance, this->vendor_name_.c_str());
    if (err != OT_ERROR_NONE) {
      ESP_LOGW(TAG, "otThreadSetVendorName('%s') failed: %d (%s)",
               this->vendor_name_.c_str(), static_cast<int>(err), ot_error_to_string(err));
      ok = false;
    }
  }

  if (!this->vendor_model_.empty()) {
    err = otThreadSetVendorModel(instance, this->vendor_model_.c_str());
    if (err != OT_ERROR_NONE) {
      ESP_LOGW(TAG, "otThreadSetVendorModel('%s') failed: %d (%s)",
               this->vendor_model_.c_str(), static_cast<int>(err), ot_error_to_string(err));
      ok = false;
    }
  }

  if (!this->vendor_sw_version_.empty()) {
    err = otThreadSetVendorSwVersion(instance, this->vendor_sw_version_.c_str());
    if (err != OT_ERROR_NONE) {
      ESP_LOGW(TAG, "otThreadSetVendorSwVersion('%s') failed: %d (%s)",
               this->vendor_sw_version_.c_str(), static_cast<int>(err), ot_error_to_string(err));
      ok = false;
    }
  }

  if (!this->vendor_app_url_.empty()) {
    err = otThreadSetVendorAppUrl(instance, this->vendor_app_url_.c_str());
    if (err != OT_ERROR_NONE) {
      ESP_LOGW(TAG, "otThreadSetVendorAppUrl('%s') failed: %d (%s)",
               this->vendor_app_url_.c_str(), static_cast<int>(err), ot_error_to_string(err));
      ok = false;
    }
  }
#else
  ESP_LOGW(TAG, "Vendor info set API not enabled in this build; skipping vendor fields");
#endif

  esp_openthread_lock_release();
  return ok;

#endif
}

}  // namespace openthread_vendor_info
}  // namespace esphome