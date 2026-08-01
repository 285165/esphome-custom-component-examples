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

  // Link Quality (0-3) / Link Margin (dB)
  bool parent_lq_valid = false;
  uint8_t parent_lq_in = 0;
  uint8_t parent_lq_out = 0;

  // Values taken from the best-RSSI neighbor
  uint8_t neighbor_best_lq_in = 0;
  uint8_t neighbor_best_margin = 0;
  uint16_t neighbor_best_frame_err = 0;
  uint16_t neighbor_best_msg_err = 0;
  uint32_t neighbor_best_age = 0;

  // The OpenThread stack is not thread-safe; take the port lock before calling
  // any ot* API from this (ESPHome loop) task.
  esp_openthread_lock_acquire(portMAX_DELAY);

  otDeviceRole role = otThreadGetDeviceRole(instance);

  // --- Parent RSSI (only meaningful for a Child) ---
  if (this->parent_avg_rssi_sensor_ != nullptr || this->parent_link_margin_sensor_ != nullptr) {
    if (otThreadGetParentAverageRssi(instance, &parent_avg) != OT_ERROR_NONE) {
      parent_avg = OT_RSSI_INVALID;
    }
  }
  if (this->parent_last_rssi_sensor_ != nullptr) {
    if (otThreadGetParentLastRssi(instance, &parent_last) != OT_ERROR_NONE) {
      parent_last = OT_RSSI_INVALID;
    }
  }

  // --- Parent Link Quality In/Out (from otRouterInfo of the parent) ---
  if (this->parent_link_quality_in_sensor_ != nullptr || this->parent_link_quality_out_sensor_ != nullptr) {
    otRouterInfo parent_info;
    if (otThreadGetParentInfo(instance, &parent_info) == OT_ERROR_NONE) {
      parent_lq_in = parent_info.mLinkQualityIn;
      parent_lq_out = parent_info.mLinkQualityOut;
      parent_lq_valid = true;
    }
  }

  // --- Neighbor RSSI / LQI / Link Margin / error rates / age ---
  if (this->neighbor_best_rssi_sensor_ != nullptr || this->neighbor_avg_rssi_sensor_ != nullptr ||
      this->neighbor_best_link_quality_sensor_ != nullptr || this->neighbor_best_link_margin_sensor_ != nullptr ||
      this->neighbor_best_frame_error_rate_sensor_ != nullptr ||
      this->neighbor_best_message_error_rate_sensor_ != nullptr || this->neighbor_count_sensor_ != nullptr ||
      this->neighbor_best_age_sensor_ != nullptr) {
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
        neighbor_best_lq_in = info.mLinkQualityIn;
        neighbor_best_margin = info.mLinkMargin;
        neighbor_best_frame_err = info.mFrameErrorRate;
        neighbor_best_msg_err = info.mMessageErrorRate;
        neighbor_best_age = info.mAge;
      }
    }
  }

  esp_openthread_lock_release();

  ESP_LOGD(TAG, "role=%d parent_avg=%d parent_last=%d parentLQ(in/out)=%d/%d neighbors=%u best=%d lqIn=%d margin=%d",
           (int) role, (int) parent_avg, (int) parent_last, (int) parent_lq_in, (int) parent_lq_out, neighbor_count,
           (int) neighbor_best, (int) neighbor_best_lq_in, (int) neighbor_best_margin);

  // --- Publish RSSI ---
  if (this->parent_avg_rssi_sensor_ != nullptr) {
    this->parent_avg_rssi_sensor_->publish_state(parent_avg != OT_RSSI_INVALID ? (float) parent_avg : NAN);
  }
  if (this->parent_last_rssi_sensor_ != nullptr) {
    this->parent_last_rssi_sensor_->publish_state(parent_last != OT_RSSI_INVALID ? (float) parent_last : NAN);
  }
  if (this->neighbor_best_rssi_sensor_ != nullptr) {
    this->neighbor_best_rssi_sensor_->publish_state(
        (neighbor_count > 0 && neighbor_best != OT_RSSI_INVALID) ? (float) neighbor_best : NAN);
  }
  if (this->neighbor_avg_rssi_sensor_ != nullptr) {
    this->neighbor_avg_rssi_sensor_->publish_state(
        neighbor_count > 0 ? (float) neighbor_sum / (float) neighbor_count : NAN);
  }

  // --- Parent Link Quality In/Out ---
  if (this->parent_link_quality_in_sensor_ != nullptr) {
    this->parent_link_quality_in_sensor_->publish_state(parent_lq_valid ? (float) parent_lq_in : NAN);
  }
  if (this->parent_link_quality_out_sensor_ != nullptr) {
    this->parent_link_quality_out_sensor_->publish_state(parent_lq_valid ? (float) parent_lq_out : NAN);
  }

  // --- Parent Link Margin (derived from parent avg RSSI and noise floor) ---
  if (this->parent_link_margin_sensor_ != nullptr) {
    if (parent_avg != OT_RSSI_INVALID) {
      int16_t margin = (int16_t) parent_avg - (int16_t) this->noise_floor_;
      if (margin < 0) {
        margin = 0;  // Link Margin is defined as a non-negative dB value
      }
      this->parent_link_margin_sensor_->publish_state((float) margin);
    } else {
      this->parent_link_margin_sensor_->publish_state(NAN);
    }
  }

  // --- Neighbor (best link) Link Quality / Link Margin ---
  if (this->neighbor_best_link_quality_sensor_ != nullptr) {
    this->neighbor_best_link_quality_sensor_->publish_state(neighbor_count > 0 ? (float) neighbor_best_lq_in : NAN);
  }
  if (this->neighbor_best_link_margin_sensor_ != nullptr) {
    this->neighbor_best_link_margin_sensor_->publish_state(neighbor_count > 0 ? (float) neighbor_best_margin : NAN);
  }

  // --- Neighbor (best link) Frame / Message error rates (uint16, 0xffff -> 100%) ---
  if (this->neighbor_best_frame_error_rate_sensor_ != nullptr) {
    this->neighbor_best_frame_error_rate_sensor_->publish_state(
        neighbor_count > 0 ? (float) neighbor_best_frame_err * 100.0f / 65535.0f : NAN);
  }
  if (this->neighbor_best_message_error_rate_sensor_ != nullptr) {
    this->neighbor_best_message_error_rate_sensor_->publish_state(
        neighbor_count > 0 ? (float) neighbor_best_msg_err * 100.0f / 65535.0f : NAN);
  }

  // --- Neighbor count (always valid; 0 when no neighbors) ---
  if (this->neighbor_count_sensor_ != nullptr) {
    this->neighbor_count_sensor_->publish_state((float) neighbor_count);
  }

  // --- Age (seconds since last heard) of the best-RSSI neighbor ---
  if (this->neighbor_best_age_sensor_ != nullptr) {
    this->neighbor_best_age_sensor_->publish_state(neighbor_count > 0 ? (float) neighbor_best_age : NAN);
  }
}

void OpenThreadRSSIComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread RSSI:");
  LOG_SENSOR("  ", "Parent Avg RSSI", this->parent_avg_rssi_sensor_);
  LOG_SENSOR("  ", "Parent Last RSSI", this->parent_last_rssi_sensor_);
  LOG_SENSOR("  ", "Neighbor Best RSSI", this->neighbor_best_rssi_sensor_);
  LOG_SENSOR("  ", "Neighbor Avg RSSI", this->neighbor_avg_rssi_sensor_);
  LOG_SENSOR("  ", "Parent Link Quality In", this->parent_link_quality_in_sensor_);
  LOG_SENSOR("  ", "Parent Link Quality Out", this->parent_link_quality_out_sensor_);
  LOG_SENSOR("  ", "Parent Link Margin", this->parent_link_margin_sensor_);
  LOG_SENSOR("  ", "Neighbor Best Link Quality", this->neighbor_best_link_quality_sensor_);
  LOG_SENSOR("  ", "Neighbor Best Link Margin", this->neighbor_best_link_margin_sensor_);
  LOG_SENSOR("  ", "Neighbor Best Frame Error Rate", this->neighbor_best_frame_error_rate_sensor_);
  LOG_SENSOR("  ", "Neighbor Best Message Error Rate", this->neighbor_best_message_error_rate_sensor_);
  LOG_SENSOR("  ", "Neighbor Count", this->neighbor_count_sensor_);
  LOG_SENSOR("  ", "Neighbor Best Age", this->neighbor_best_age_sensor_);
  ESP_LOGCONFIG(TAG, "  Noise Floor: %d dBm", this->noise_floor_);
  LOG_UPDATE_INTERVAL(this);
}

}  // namespace openthread_rssi
}  // namespace esphome

#endif  // USE_ESP32
