

# Omni IMU Test with Teensy

This repository contains **example code** and **setup instructions** for testing **Omni IMU sensors** with a **Teensy** microcontroller.
It includes three main steps:

1. **Pairing IMU with Teensy** (`pair_imu_with_teensy`)
2. **Testing IMU data accuracy** (`tesy_im948_ok_with_teensy`)
3. **Checking SD card logging** (`test_sd_card_logged/test`)

4. ![usb to ttl](/image.png)
---

## 📦 Files

| Folder                      | Purpose                                         |
| --------------------------- | ----------------------------------------------- |
| `pair_imu_with_teensy`      | Pair IMU and TR100 Bluetooth module with Teensy |
| `tesy_im948_ok_with_teensy` | Verify IMU data output after pairing            |
| `test_sd_card_logged/test`  | Check data logging to SD card                   |

---

## 📖 IMU Resources

* **IMU Wiki:** [http://biomechatronics.wikidot.com/wiki\:imu-sensor](http://biomechatronics.wikidot.com/wiki:imu-sensor)
* **PC Software:** [Download](https://www.dropbox.com/scl/fi/q0h8ouc72m3rgowg6f0s0/IMU_2.33_Release_English-v2.exe?rlkey=f4t5tojolnzs5rr06gbkz9c7t&e=1&dl=0)
* **Specifications (PPT):** [Download](https://www.dropbox.com/scl/fi/7xy8oyd4o0yxv5cz81y83/2025-2-14-Omni-imu-specifications.pptx?dl=0&rlkey=al0zkvw6fepnodw61cta6f2so)
* **Reference Code:** [Omni\_IMU GitHub](https://github.com/biomechatronics001/SimToReal_Deployment_HipExo/tree/RAM/Lower_level_controllers/Omni_IMU)

---

## 🔹 Step 1 — Pairing IMU & TR100 with Teensy

### Hardware

* **Bluetooth TR100** modules (1-to-1 with each IMU)
* **Max sampling rate:** 250 Hz
* **Default baud rate:** `460800`

### Pairing Indicators

* **IMU**:

  * Short press power → Turn on
  * 1 flash every 4s = Not connected
  * 2 flashes every 4s = Connected
* **TR100**:

  * Double flash every 2s = Connected

---

### Pairing Procedure
0. IM948 menu: https://www.yuque.com/docs/share/de4f810f-4790-4866-9b17-657be095f2bb?# 
1. Download TR100 PC tool:
   [Baidu Link](https://pan.baidu.com/s/1QrPmc_ImhZfWZHYgccVJXw?pwd=vxec) (Code: `vxec`)
2. Open the tool → Click **Connect Bluetooth**
3. Power on IMU → Read **ID number** → Write it on the back
4. Refer to: [TR100 User Manual](https://www.yuque.com/cxqwork/lkw3sg/lcyzdvgeaeyk98sh?singleDoc#)
5. Upload pairing code to Teensy:

   * `SerialImuLeft  = Serial7` (U8) — use **U8\_pair.ino**
   * `SerialImuRight = Serial6` (U7) — use **U7\_pair.ino**
6. **Known Bug:**

   * After first pairing, there is no data output
   * Solution: Re-pair with PC tool, then power-cycle Teensy

---

## 🔹 Step 2 — Testing IMU Data

* Use `tesy_im948_ok_with_teensy` to check:

  * Data streaming rate (up to 250 Hz)
  * Accuracy of IMU readings
* If data looks correct, proceed to SD card logging

---

## 🔹 Step 3 — Logging to SD Card

* Use `test_sd_card_logged/test` to verify:

  * SD card writing speed
  * Data integrity

---

## ✅ Workflow Summary

1. **Pair IMU & TR100** (`pair_imu_with_teensy`)
2. **Test Data Accuracy** (`tesy_im948_ok_with_teensy`)
3. **Verify SD Card Logging** (`test_sd_card_logged/test`)
4. Once stable, integrate into your **main.ino** project

# IMU Initialization Issue and Solution

During testing, we confirmed that the IMU initialization problem is **not caused by the IMU itself**, but by the **Serial communication between the TR100 and the Teensy (Serial7/Serial6)**.  

The key function is `imu.INIT()`, which starts `Serial7.begin()` and `Serial6.begin()`. In theory, the data frames from TR100 should arrive immediately. In practice, however, we sometimes only receive valid frames after about 10 seconds.  

Since the IMU offset calibration (`imu.mean`) relies on the first 5 seconds of incoming data, when no new frames are received it defaults to `0` as the offset. Once real data finally arrives, the system interprets the angle as `frame - 0`, which produces incorrect readings (e.g., jumping close to **100°** instead of the expected ~3° offset).  

The root cause appears to be **Serial blocking at 460 kHz**. This high baud rate, combined with PCB layout and potential shielding issues between the two nearby UART ports, results in delayed valid data reception. Addressing this at the hardware/PCB level is difficult.  

### Practical Engineering Solution
Instead of redesigning the hardware, the issue can be resolved in software by:
1. Allowing **longer initialization time** before offset calibration.  
2. Adding a **check to ensure the offset is non-zero** before accepting it.  
3. Verifying that **IMU initialization is successful** before proceeding.  

This approach avoids incorrect offset calibration and ensures stable IMU behavior without hardware redesign.

