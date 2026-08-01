# openthread_rssi — ESPHome external component

Exposes OpenThread (Thread / 802.15.4) link RSSI as ESPHome sensors so the
values are published to Home Assistant. Works on ESP32-C6 / ESP32-H2 / ESP32-C5
with the ESP-IDF framework and the built-in `openthread` component.

## Sensors

| Config key           | Meaning                                                        | Valid on            |
|----------------------|----------------------------------------------------------------|---------------------|
| `parent_avg_rssi`    | Average RSSI to the Thread **parent** (`otThreadGetParentAverageRssi`) | Child (MTD/MED)     |
| `parent_last_rssi`   | RSSI of the **last** packet from the parent (`otThreadGetParentLastRssi`) | Child               |
| `neighbor_best_rssi` | Best (closest to 0 dBm) RSSI among all neighbors               | Router / Leader / FTD |
| `neighbor_avg_rssi`  | Mean RSSI across all neighbors in the neighbor table           | Router / Leader / FTD |

All sensors report in **dBm**, `device_class: signal_strength`,
`state_class: measurement`, entity category **diagnostic**. When a value is not
available (e.g. no parent while acting as Leader), the sensor publishes `NAN`
(shows as *unavailable* in Home Assistant).

## Install

Directory layout expected in your repo:

```
components/
  openthread_rssi/
    __init__.py
    sensor.py
    openthread_rssi.h
    openthread_rssi.cpp
```

Then reference it from your ESPHome YAML — see `example.yaml`.

## Notes / gotchas

- The OpenThread stack is **not** thread-safe. The component takes the ESP-IDF
  OpenThread port lock (`esp_openthread_lock_acquire`) around every `ot*` call.
- `127` (INT8_MAX) is OpenThread's "invalid RSSI" sentinel and is filtered out.
- A **Child** device has no neighbors in the router sense → neighbor sensors
  will be `NAN`. A **Leader/Router** has no parent → parent sensors will be `NAN`.
  Add only the sensors relevant to your device role if you want a clean UI.
- Requires the OpenThread stack to already be attached; values appear a few
  `update_interval`s after boot once the device joins the mesh.
