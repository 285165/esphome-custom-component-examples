#include "openthread_diagnostics.h"
#include "esphome/core/log.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#if defined(USE_ESP32) && __has_include("esp_openthread.h")
#include "esp_openthread.h"
#include "esp_openthread_lock.h"
#include "freertos/FreeRTOS.h"
#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/platform/radio.h>
#include <openthread/thread.h>
#define OTD_HAS_OPENTHREAD 1
#else
#define OTD_HAS_OPENTHREAD 0
#endif

namespace esphome {
namespace openthread_diagnostics {

static const char *const TAG = "openthread_diagnostics";

#if OTD_HAS_OPENTHREAD
static std::string role_to_string(otDeviceRole role) {
  switch (role) {
    case OT_DEVICE_ROLE_DISABLED: return "disabled";
    case OT_DEVICE_ROLE_DETACHED: return "detached";
    case OT_DEVICE_ROLE_CHILD: return "child";
    case OT_DEVICE_ROLE_ROUTER: return "router";
    case OT_DEVICE_ROLE_LEADER: return "leader";
    default: return "unknown";
  }
}

static std::string ext_address_to_string(const otExtAddress *addr) {
  char buf[24];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
           addr->m8[0], addr->m8[1], addr->m8[2], addr->m8[3],
           addr->m8[4], addr->m8[5], addr->m8[6], addr->m8[7]);
  return std::string(buf);
}

static std::string ip6_to_string(const otIp6Address *addr) {
  char buf[OT_IP6_ADDRESS_STRING_SIZE];
  otIp6AddressToString(addr, buf, sizeof(buf));
  return std::string(buf);
}

static std::string hex_u8(uint8_t value) {
  char buf[5];
  snprintf(buf, sizeof(buf), "0x%02X", static_cast<unsigned int>(value));
  return std::string(buf);
}

static std::string hex_u16(uint16_t value) {
  char buf[7];
  snprintf(buf, sizeof(buf), "0x%04X", static_cast<unsigned int>(value));
  return std::string(buf);
}

static std::string hex_u32(uint32_t value) {
  char buf[11];
  snprintf(buf, sizeof(buf), "0x%08lX", static_cast<unsigned long>(value));
  return std::string(buf);
}

static float error_rate_to_percent(uint16_t value) {
  return (static_cast<float>(value) * 100.0f) / 65535.0f;
}
#endif

void OpenThreadDiagnostics::dump_config() {
  ESP_LOGCONFIG(TAG, "OpenThread diagnostics component");
  LOG_UPDATE_INTERVAL(this);
#if !OTD_HAS_OPENTHREAD
  ESP_LOGW(TAG, "OpenThread headers were not found at compile time; sensors will publish unavailable data");
#endif
}

void OpenThreadDiagnostics::update() {
#if !OTD_HAS_OPENTHREAD
  ESP_LOGW(TAG, "OpenThread is not available in this ESPHome/ESP-IDF build");
  return;
#else
  otInstance *instance = esp_openthread_get_instance();
  if (instance == nullptr) {
    ESP_LOGW(TAG, "OpenThread instance is null. Is the ESPHome openthread: component enabled and running?");
    return;
  }

  if (!esp_openthread_lock_acquire(pdMS_TO_TICKS(1000))) {
    ESP_LOGW(TAG, "Could not acquire OpenThread API lock");
    return;
  }

  otDeviceRole role = otThreadGetDeviceRole(instance);

  if (role_text_sensor_ != nullptr) role_text_sensor_->publish_state(role_to_string(role));
  if (network_name_text_sensor_ != nullptr) network_name_text_sensor_->publish_state(otThreadGetNetworkName(instance));
  if (thread_version_text_sensor_ != nullptr) thread_version_text_sensor_->publish_state(otGetVersionString());
  if (ext_address_text_sensor_ != nullptr) ext_address_text_sensor_->publish_state(ext_address_to_string(otLinkGetExtendedAddress(instance)));

  if (channel_sensor_ != nullptr) channel_sensor_->publish_state(otLinkGetChannel(instance));

  // Thread identifiers published as HEX text sensors.
  if (pan_id_text_sensor_ != nullptr) pan_id_text_sensor_->publish_state(hex_u16(otLinkGetPanId(instance)));
  if (partition_id_text_sensor_ != nullptr) partition_id_text_sensor_->publish_state(hex_u32(otThreadGetPartitionId(instance)));
  if (leader_router_id_text_sensor_ != nullptr) leader_router_id_text_sensor_->publish_state(hex_u8(otThreadGetLeaderRouterId(instance)));
  if (rloc16_text_sensor_ != nullptr) rloc16_text_sensor_->publish_state(hex_u16(otThreadGetRloc16(instance)));

  // Stable Thread Mesh Local EID from OpenThread API.
  if (mesh_local_eid_text_sensor_ != nullptr) {
    const otIp6Address *mleid = otThreadGetMeshLocalEid(instance);
    if (mleid != nullptr) {
      mesh_local_eid_text_sensor_->publish_state(ip6_to_string(mleid));
    }
  }

  int8_t tx_power = 0;
  if (tx_power_sensor_ != nullptr && otPlatRadioGetTransmitPower(instance, &tx_power) == OT_ERROR_NONE) {
    tx_power_sensor_->publish_state(tx_power);
  }

  uint16_t parent_rloc16 = 0xffff;
  bool has_parent = false;
#if defined(OT_DEVICE_ROLE_CHILD) && defined(OT_DEVICE_ROLE_ROUTER)
  otRouterInfo parent_info;
  if (otThreadGetParentInfo(instance, &parent_info) == OT_ERROR_NONE) {
    parent_rloc16 = parent_info.mRloc16;
    has_parent = true;
  }
#endif

  uint32_t neighbor_count = 0;
  uint32_t child_count = 0;
  bool parent_from_neighbor = false;
  otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;
  otNeighborInfo neighbor;
  while (otThreadGetNextNeighborInfo(instance, &iterator, &neighbor) == OT_ERROR_NONE) {
    neighbor_count++;
    if (neighbor.mIsChild) child_count++;
    if ((has_parent && neighbor.mRloc16 == parent_rloc16) || (!has_parent && !neighbor.mIsChild && !parent_from_neighbor)) {
      parent_from_neighbor = true;
      if (parent_rssi_sensor_ != nullptr) parent_rssi_sensor_->publish_state(neighbor.mLastRssi);
      if (parent_average_rssi_sensor_ != nullptr) parent_average_rssi_sensor_->publish_state(neighbor.mAverageRssi);
      if (parent_link_margin_sensor_ != nullptr) parent_link_margin_sensor_->publish_state(neighbor.mLinkMargin);
      if (parent_link_quality_sensor_ != nullptr) parent_link_quality_sensor_->publish_state(neighbor.mLinkQualityIn);
      if (parent_age_sensor_ != nullptr) parent_age_sensor_->publish_state(neighbor.mAge);
      if (frame_error_rate_sensor_ != nullptr) frame_error_rate_sensor_->publish_state(error_rate_to_percent(neighbor.mFrameErrorRate));
      if (message_error_rate_sensor_ != nullptr) message_error_rate_sensor_->publish_state(error_rate_to_percent(neighbor.mMessageErrorRate));
    }
  }

  if (neighbor_count_sensor_ != nullptr) neighbor_count_sensor_->publish_state(neighbor_count);
  if (child_count_sensor_ != nullptr) child_count_sensor_->publish_state(child_count);

  const otMacCounters *cnt = otLinkGetCounters(instance);
  if (cnt != nullptr) {
    if (mac_tx_total_sensor_ != nullptr) mac_tx_total_sensor_->publish_state(cnt->mTxTotal);
    if (mac_rx_total_sensor_ != nullptr) mac_rx_total_sensor_->publish_state(cnt->mRxTotal);
    if (mac_tx_err_cca_sensor_ != nullptr) mac_tx_err_cca_sensor_->publish_state(cnt->mTxErrCca);
    if (mac_tx_retry_sensor_ != nullptr) mac_tx_retry_sensor_->publish_state(cnt->mTxRetry);
  }

  const otNetifAddress *addr = otIp6GetUnicastAddresses(instance);
  std::string all_addresses;
  std::string mesh_local;
  std::string link_local;
  while (addr != nullptr) {
    std::string one = ip6_to_string(&addr->mAddress);
    if (!all_addresses.empty()) all_addresses += ", ";
    all_addresses += one;

    if (addr->mAddress.mFields.m8[0] == 0xfe && addr->mAddress.mFields.m8[1] == 0x80 && link_local.empty()) {
      link_local = one;
    }
    if (addr->mAddress.mFields.m8[0] == 0xfd && mesh_local.empty()) {
      mesh_local = one;
    }
    addr = addr->mNext;
  }
  if (ip_addresses_text_sensor_ != nullptr) ip_addresses_text_sensor_->publish_state(all_addresses);
  if (mesh_local_address_text_sensor_ != nullptr) mesh_local_address_text_sensor_->publish_state(mesh_local);
  if (link_local_address_text_sensor_ != nullptr) link_local_address_text_sensor_->publish_state(link_local);

  esp_openthread_lock_release();
#endif
}

}  // namespace openthread_diagnostics
}  // namespace esphome
