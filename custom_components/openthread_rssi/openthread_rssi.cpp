#include "openthread_rssi.h"

#ifdef USE_ESP32
#include "esphome/core/log.h"

// ESP-IDF OpenThread port
#include "esp_openthread.h"
#include "esp_openthread_lock.h"

// OpenThread stack APIs
#include <openthread/instance.h>
#include <openthread/thread.h>

#include <cstdint>

namespace esphome {
namespace openthread_rssi {

static const char *const TAG = "openthread_rssi";

// OpenThread uses 127 (INT8_MAX) as the "invalid RSSI" sentinel.
static constexpr int8_t OT_RSSI_INVALID = 127;

float OpenThreadRSSIComponent::get_setup_priority() const {
  // Run after the OpenThread stack has been brought up.
  return setup_priority::AFTER_WIFI;
}

void OpenThreadRSSIComponent::setup() { ESP_LOGCONFIG(TAG, "Setting up OpenThread RSSI..."); }

void OpenThreadRSSIComponent::update() {
  otInstance *instance = esp_openthread_get_instance();
  if (instance == nullptr) {
    ESP_LOGW(TAG, "OpenThread instance not ready");
    return;
  }

  int8_t parent_avg = OT_RSSI_INVALID;
  int8_t parent_last = OT_RSSI_INVALID;
  int8_t neighbor_best = OT_RSSI_INVALID;
  int32_t neighbor_sum = 0;
  uint32_t neighbor_count = 0;

  // The OpenThread stack is not thread-safe; take the port lock before calling
  // any ot* API from this (ESPHome loop) task.
  esp_openthread_lock_acquire(portMAX_DELAY);

  otDeviceRole role = otThreadGetDeviceRole(instance);

  // --- Parent RSSI (only meaningful for a Child) ---
  if (this->parent_avg_rssi_sensor_ != nullptr) {
    if (otThreadGetParentAverageRssi(instance, &parent_avg) != OT_ERROR_NONE) {
      parent_avg = OT_RSSI_INVALID;
    }
  }
  if (this->parent_last_rssi_sensor_ != nullptr) {
    if (otThreadGetParentLastRssi(instance, &parent_last) != OT_ERROR_NONE) {
      parent_last = OT_RSSI_INVALID;
    }
  }

  // --- Neighbor RSSI (Router/Leader/FTD or any device with neighbors) ---
  if (this->neighbor_best_rssi_sensor_ != nullptr || this->neighbor_avg_rssi_sensor_ != nullptr) {
    otNeighborInfo info;
    otNeighborInfoIterator it = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    while (otThreadGetNextNeighborInfo(instance, &it, &info) == OT_ERROR_NONE) {
      int8_t rssi = info.mAverageRssi;
      if (rssi == OT_RSSI_INVALID) {
        rssi = info.mLastRssi;
      }
      if (rssi == OT_RSSI_INVALID) {
        continue;
      }
      neighbor_sum += rssi;
      neighbor_count++;
      if (neighbor_best == OT_RSSI_INVALID || rssi > neighbor_best) {
        neighbor_best = rssi;  // "best" = closest to 0 dBm
      }
    }
  }

  esp_openthread_lock_release();

  ESP_LOGD(TAG, "role=%d parent_avg=%d parent_last=%d neighbors=%u best=%d", (int) role, (int) parent_avg,
           (int) parent_last, neighbor_count, (int) neighbor_best);

  // --- Publish ---
  if (this->parent_avg_rssi_sensor_ != nullptr) {
    if (parent_avg != OT_RSSI_INVALID) {
      this->parent_avg_rssi_sensor_->publish_state(parent_avg);
    } else {
      this->parent_avg_rssi_sensor_->publish_state(NAN);
    }
  }
  if (this->parent_last_rssi_sensor_ != nullptr) {
    if (parent_last != OT_RSSI_INVALID) {
      this->parent_last_rssi_sensor_->publish_state(parent_last);
    } else {
      this->parent_last_rssi_sensor_->publish_state(NAN);
    }
  }
  if (this->neighbor_best_rssi_sensor_ != nullptr) {
    if (neighbor_count > 0 && neighbor_best != OT_RSSI_INVALID) {
      this->neighbor_best_rssi_sensor_->publish_state(neighbor_best);
    } else {
      this->neighbor_best_rssi_sensor_->publish_state(NAN);
    }
  }
  if (this->neighbor_avg_rssi_sensor_ != nullptr) {
    if (neighbor_count > 0) {
      this->neighbor_avg_rssi_sensor_->publish_state((float) neighbor_sum / (float) neighbor_count);
    } else {
      this->neighbor_avg_rssi_sensor_->publish_state(NAN);
    }
  }
}

void OpenThreadRSSIComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread RSSI:");
  LOG_SENSOR("  ", "Parent Avg RSSI", this->parent_avg_rssi_sensor_);
  LOG_SENSOR("  ", "Parent Last RSSI", this->parent_last_rssi_sensor_);
  LOG_SENSOR("  ", "Neighbor Best RSSI", this->neighbor_best_rssi_sensor_);
  LOG_SENSOR("  ", "Neighbor Avg RSSI", this->neighbor_avg_rssi_sensor_);
  LOG_UPDATE_INTERVAL(this);
}

}  // namespace openthread_rssi
}  // namespace esphome

#endif  // USE_ESP32
