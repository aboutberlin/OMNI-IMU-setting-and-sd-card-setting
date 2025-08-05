#include "sdlogger.h"
#include <stdarg.h>

SdLogger::SdLogger(uint8_t csPin,
                   const __FlashStringHelper* baseName,
                   const __FlashStringHelper* ext_,
                   uint16_t maxIndex)
: cs(csPin), base(baseName), ext(ext_), maxIdx(maxIndex)
{
  fname[0] = '\0';
}

bool SdLogger::begin()
{
  if (!SD.begin(cs)) {
    Serial.println(F("SD init failed"));
    return false;
  }
  if (!openUnique_()) {
    Serial.println(F("create file failed"));
    return false;
  }
  Serial.print(F("Logging to: "));
  Serial.println(fname);
  return true;
}

bool SdLogger::openUnique_()
{
  for (uint16_t i = 1; i <= maxIdx; ++i) {
    snprintf(fname, sizeof(fname), "%s%03u%s",
             reinterpret_cast<const char*>(base), i,
             reinterpret_cast<const char*>(ext));
    if (!SD.exists(fname)) {
      file = SD.open(fname, FILE_WRITE);
      return file;
    }
  }
  return false;
}

void SdLogger::printf(const char* fmt, ...)
{
  if (!file) return;
  char buf[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  file.print(buf);
}
