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

  ESP_LOGW(TAG, "USE_OPENTHREAD not enabled");
  return false;

#else

#ifndef OPENTHREAD_CONFIG_NET_DIAG_VENDOR_INFO_SET_API_ENABLE

  ESP_LOGW(TAG,
           "OpenThread vendor info set API is not enabled in this ESP-IDF/OpenThread build");
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

  ESP_LOGI(TAG,
           "Thread state: ip6=%d role=%d",
           ip6_enabled,
           static_cast<int>(role));

  if (!ip6_enabled) {
    esp_openthread_lock_release();
    return false;
  }

  ESP_LOGI(TAG,
           "Configured Vendor Name='%s' len=%u",
           vendor_name_.c_str(),
           (unsigned) vendor_name_.length());

  ESP_LOGI(TAG,
           "Configured Vendor Model='%s' len=%u",
           vendor_model_.c_str(),
           (unsigned) vendor_model_.length());

  ESP_LOGI(TAG,
           "Configured Vendor SW='%s' len=%u",
           vendor_sw_version_.c_str(),
           (unsigned) vendor_sw_version_.length());

  const char *current_name = otThreadGetVendorName(instance);
  const char *current_model = otThreadGetVendorModel(instance);
  const char *current_sw = otThreadGetVendorSwVersion(instance);

  ESP_LOGI(TAG,
           "Current Vendor Name: %s",
           current_name ? current_name : "<null>");

  ESP_LOGI(TAG,
           "Current Vendor Model: %s",
           current_model ? current_model : "<null>");

  ESP_LOGI(TAG,
           "Current Vendor SW: %s",
           current_sw ? current_sw : "<null>");

  otError err;

  if (!vendor_name_.empty()) {

    err = otThreadSetVendorName(
        instance,
        vendor_name_.c_str());

    if (err != OT_ERROR_NONE) {

      ESP_LOGW(TAG,
               "otThreadSetVendorName failed: %d (%s)",
               (int) err,
               ot_error_to_string(err));

      esp_openthread_lock_release();
      return false;
    }
  }

  if (!vendor_model_.empty()) {

    err = otThreadSetVendorModel(
        instance,
        vendor_model_.c_str());

    if (err != OT_ERROR_NONE) {

      ESP_LOGW(TAG,
               "otThreadSetVendorModel failed: %d (%s)",
               (int) err,
               ot_error_to_string(err));

      esp_openthread_lock_release();
      return false;
    }
  }

  if (!vendor_sw_version_.empty()) {

    err = otThreadSetVendorSwVersion(
        instance,
        vendor_sw_version_.c_str());

    if (err != OT_ERROR_NONE) {

      ESP_LOGW(TAG,
               "otThreadSetVendorSwVersion failed: %d (%s)",
               (int) err,
               ot_error_to_string(err));

      esp_openthread_lock_release();
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