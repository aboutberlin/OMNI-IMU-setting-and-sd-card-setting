#pragma once
#include <Arduino.h>
#include <SD.h>

class SdLogger {
public:
  SdLogger(uint8_t csPin,
           const __FlashStringHelper* baseName,
           const __FlashStringHelper* ext,
           uint16_t maxIndex = 999);

  bool   begin();
  void   print  (const char* s)              { if (file) file.print(s); }
  void   println(const char* s)              { if (file) file.println(s); }
  void   print  (const __FlashStringHelper* s) { if (file) file.print(s); }
  void   println(const __FlashStringHelper* s) { if (file) file.println(s); }
  void   printf (const char* fmt, ...);
  void   flush()                             { if (file) file.flush(); }
  bool isOpen() { return file; }  // 使用 File 的隐式 operator bool()
  const  char* filename() const              { return fname; }

private:
  uint8_t cs;
  const __FlashStringHelper* base;
  const __FlashStringHelper* ext;
  uint16_t maxIdx;
  File file;
  char fname[32];

  bool   openUnique_();
};




// #include <Arduino.h>
// #include "SdLogger.h"

// // === 创建一个 SdLogger 实例 ===
// // 使用 Teensy 的 BUILTIN_SDCARD 引脚（可改为其他）
// #define FSTR(x) (reinterpret_cast<const __FlashStringHelper *>(x))

// #include "sdlogger.h"
// SdLogger logger(BUILTIN_SDCARD, FSTR("samsung_log_"), FSTR(".csv"));

// SdLogger logger(BUILTIN_SDCARD, F("samsung_log_"), F(".csv"));

// void setup() {
//   Serial.begin(115200);
//   while (!Serial); // 等待串口准备就绪（用于串口调试）

//   Serial.println(F("== 测试 SD 卡日志系统 =="));

//   if (logger.begin()) {
//     Serial.print(F("文件名: "));
//     Serial.println(logger.filename());

//     logger.println(F("Time_ms,Angle_L,Angle_R,Torque_L,Torque_R")); // CSV 表头
//     logger.flush();
//   } else {
//     Serial.println(F("SD 初始化或文件创建失败！"));
//   }
// }

// void loop() {
//   if (!logger.isOpen()) return;

//   static uint32_t t0 = millis();
//   uint32_t now = millis();
//   float t = (now - t0) / 1000.0f;

//   // 模拟数据
//   float thetaL = 30.0f * sin(t);
//   float thetaR = -30.0f * cos(t);
//   float tauL   = 5.0f * sin(2*t);
//   float tauR   = 5.0f * cos(2*t);

//   // 写入一行
//   logger.printf("%lu,%.2f,%.2f,%.2f,%.2f\n",
//                 now, thetaL, thetaR, tauL, tauR);

//   Serial.print(F("写入一行：t=")); Serial.println(t, 2);

//   logger.flush();  // 若频繁写，可每 N 次 flushlogger.flush();  // ✅ 加这句！


//   delay(100);  // 每 100ms 写一行，方便测试
// }
