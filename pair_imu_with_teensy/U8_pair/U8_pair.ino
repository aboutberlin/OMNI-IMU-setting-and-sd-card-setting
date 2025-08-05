// Teensy 4.1 使用 Serial7: TX2 (pin 47), RX2 (pin 48)

void setup() {
  // 用于串口监视器调试
  Serial.begin(115200);
  while (!Serial); // 等待串口连接（可选）

  // 初始化 TR100 通信串口
  Serial7.begin(460800); // TR100 默认波特率为 460800

  delay(1000); // 等待模块上电稳定（1秒内发送指令）

  // === 设置为主机绑定模式 ===
  Serial7.write("AT+BleSetToClientModeFind....."); // 补齐30字节
  Serial.println("已发送：主机绑定模式指令");

  delay(100); // 等待模块处理

  // === 设置蓝牙名称为 TR100（可选） ===
  Serial7.write("AT+BleSetName:TR100==========="); // 蓝牙名称最多15字节，用 = 补足
  Serial.println("已发送：设置蓝牙名称指令");

  Serial.println("全部配置指令已发送，等待绑定...");
}

void loop() {
  // 打印 TR100 回复内容（如有）
  while (Serial7.available()) {
    char c = Serial7.read();
    Serial.write(c); // 打印到串口监视器
  }

  // 可选：添加一些延时，避免串口监视器刷太快
  delay(10);
}
