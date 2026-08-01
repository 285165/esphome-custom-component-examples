#include "openthread_vendor_info.h"
#include "esphome/core/defines.h"

#ifdef USE_OPENTHREAD
#include <esp_openthread.h>
#include <esp_openthread_lock.h>

#include <openthread/instance.h>
#include <openthread/netdiag.h>
#include <openthread/thread.h>
#include <openthread/ip6.h>
#endif

namespace esphome {
namespace openthread_vendor_info {

static const char *const TAG = "openthread_vendor_info";

void OpenThreadVendorInfoComponent::setup() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info external component starting");
}

void OpenThreadVendorInfoComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread Vendor Info:");
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
  return false;
#else

#ifndef OPENTHREAD_CONFIG_NET_DIAG_VENDOR_INFO_SET_API_ENABLE

  ESP_LOGW(TAG,
           "OpenThread vendor info set API is not enabled in this ESP-IDF/OpenThread build");
  return false;

#else

  otInstance *instance = esp_openthread_get_instance();

  if (instance == nullptr) {
    ESP_LOGD(TAG, "OpenThread instance is null");
    return false;
  }

  esp_openthread_lock_acquire(portMAX_DELAY);

  bool ip6_enabled = otIp6IsEnabled(instance);
  bool thread_enabled = otThreadIsEnabled(instance);
  otDeviceRole role = otThreadGetDeviceRole(instance);

  ESP_LOGI(TAG,
           "Thread state: ip6=%d thread=%d role=%d",
           ip6_enabled,
           thread_enabled,
           role);

  if (!ip6_enabled || !thread_enabled) {
    esp_openthread_lock_release();
    ESP_LOGD(TAG, "Thread stack not fully initialized yet");
    return false;
  }

  if (role == OT_DEVICE_ROLE_DISABLED) {
    esp_openthread_lock_release();
    ESP_LOGD(TAG, "Thread role is DISABLED");
    return false;
  }

  otError err;

  if (!vendor_name_.empty()) {
    err = otThreadSetVendorName(instance, vendor_name_.c_str());

    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG,
               "otThreadSetVendorName failed: %d",
               static_cast<int>(err));
      return false;
    }
  }

  if (!vendor_model_.empty()) {
    err = otThreadSetVendorModel(instance, vendor_model_.c_str());

    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG,
               "otThreadSetVendorModel failed: %d",
               static_cast<int>(err));
      return false;
    }
  }

  if (!vendor_sw_version_.empty()) {
    err = otThreadSetVendorSwVersion(instance,
                                     vendor_sw_version_.c_str());

    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG,
               "otThreadSetVendorSwVersion failed: %d",
               static_cast<int>(err));
      return false;
    }
  }

  if (!vendor_app_url_.empty()) {
    err = otThreadSetVendorAppUrl(instance,
                                  vendor_app_url_.c_str());

    if (err != OT_ERROR_NONE) {
      esp_openthread_lock_release();
      ESP_LOGW(TAG,
               "otThreadSetVendorAppUrl failed: %d",
               static_cast<int>(err));
      return false;
    }
  }

  esp_openthread_lock_release();

  ESP_LOGI(TAG, "Vendor information applied successfully");
  return true;

#endif
#endif
}

}  // namespace openthread_vendor_info
}  // namespace esphome