#include "im948_CMD.h"


//***Data sent via bluetooth
char datalength_ble    = 32;      // Bluetooth Data Length (32)
char data_ble[60]      = {0};     // Data array for bluetooth data sending:  Teensy->RS232->Adafruit Feather nRF52840 Express(peripheral)->bluetooth->Adafruit Feather nRF52840 Express(central)->usb->computer
char data_rs232_rx[60] = {0};     // Data array for bluetooth data receive:  computer->USB->Adafruit Feather nRF52840 Express(central)->bluetooth->Adafruit Feather nRF52840 Express(peripheral)->RS232->Teensy

//***For managing the Controller and Bluetooth rate
unsigned long t_0          = 0;
unsigned long current_time = 0;  


// double cyclespersec_ctrl = 28;                                                      // [Hz] teensy controller sample rate (Maximum frequency: 1000 Hz due to Can Bus) Controller must be faster than ble
double cyclespersec_ctrl    = 100;                                                     // [Hz] teensy controller sample rate (Maximum frequency: 1000 Hz due to Can Bus) Controller must be faster than ble
double cyclespersec_gui     = 30;                                                      // [Hz] Bluetooth sending data frequency 
double cyclespersec_sensing = 300; 

unsigned long previous_time_sensing = 0;     
unsigned long previous_time_ctl     = 0;                                                    // used to control the controller sample rate.
unsigned long previous_time_gui     = 0;                                                // used to control the Bluetooth communication frequency
unsigned long Tinterval_ctrl_micros = (unsigned long)(1000000/cyclespersec_ctrl); // used to control the teensy controller frequency
unsigned long Tinterval_gui_micros  = (unsigned long)(1000000/cyclespersec_gui);  // used to control the Bluetooth communication frequency
unsigned long Tinterval_sensing_micros  = (unsigned long)(1000000/cyclespersec_sensing);  // used to control the Bluetooth communication frequency

int L_leg_IMU_angle = 0;    
int R_leg_IMU_angle = 0;   
int L_leg_IMU_vel   = 0;   
int R_leg_IMU_vel   = 0;   

double t;   
int t_teensy   = 0;  
int M_Selected = 0;  
int CtrlMode_Selected = 0;  

void setup()
{
    Serial.begin(1000000);    
    Serial5.begin(115200);     

    SerialImuLeft.begin(460800);     
    SerialImuRight.begin(460800);     

    // SerialImuLeft.begin(115200);     
    // SerialImuRight.begin(115200);     
    
    Dbp("--- IM948 arduino test start V1.03 ---\r\n");  
    delay(3000);  

    // 唤醒传感器 // 
    // Cmd_03();
    Cmd_03_Left();  // left imu 
    delay(200);
    
    Cmd_03_Right(); // right imu  
    delay(200);  

    /**
       * 设置设备参数
     * @param accStill    
     * @param stillToZero 
     * @param moveToZero  
     * @param isCompassOn 
     * @param barometerFilter 
     * @param reportHz 
     * @param gyroFilter    
     * @param accFilter     
     * @param compassFilter 
     * @param Cmd_ReportTag 
     */
    // Cmd_12(5, 255, 0,  0, 3, 2, 2, 4, 9, 0xFFF);// 
    // Cmd_12(5, 255, 0,  0, 3, 200, 2, 4, 9, 0x44);   
    Cmd_12_Left(5, 255, 0,  0, 3, 200, 2, 4, 9, 0x44);   
    delay(300);
    Cmd_12_Right(5, 255, 0,  0, 3, 200, 2, 4, 9, 0x44);   
    delay(300);   

    // 数据主动上报 
    // Cmd_19();  
    Cmd_19_Left();         
    delay(300);   
    Cmd_19_Right();   
    delay(300);  
    t_0 = micros();      
}

void loop()
{
    // 处理imu发过来的数据
    U8 rxByteLeft,rxByteRight;  
    while (SerialImuLeft.available() > 0)  
    { 
        rxByteLeft = SerialImuLeft.read();          // 读取串口数据  
        if (Cmd_GetPkt_Left(rxByteLeft)){break;}  
    }

    while (SerialImuRight.available() > 0)  
    { 
        rxByteRight = SerialImuRight.read();        // 读取串口数据
        if (Cmd_GetPkt_Right(rxByteRight)){break;}     
    }

    current_time = micros() - t_0;                  
    t = current_time/1000000.0;      

    Serial.print(": \n");   
    Serial.print(AngleXLeft);    
    Serial.print(": \n");   
    Serial.print(AngleXRight);    
    Serial.print(": \n");    

    if (current_time - previous_time_gui > Tinterval_gui_micros)
    {
        ///// GUI /////
        Receive_ble_Data();     
        Transmit_ble_Data();     
        
        Serial.print("GUI Time :");     
        Serial.print(String(t, 5));    
        Serial.print("\n");     
        
        previous_time_gui = current_time;     
    }   

//     if (isNewData)
//     {// 已更新数据
//         isNewData = 0; 
//         Serial.print(""); 
//         Serial.print(AngleX);  
//         Serial.print(""); 
//         // display(AngleX); // 显示全局变量的欧拉角X角度值
//     //  display(AngleY); // 显示全局变量的欧拉角Y角度值
//     //  display(AngleZ); // 显示全局变量的欧拉角Z角度值
// //      ........
//     }
}

void Receive_ble_Data(){
  if (Serial5.available() >= 20) 
  {

    // Read the incoming byte:
    Serial5.readBytes(&data_rs232_rx[0], 20);     

    // float value_scale = 10.0;     
    if (data_rs232_rx[0] == 165) { // Check the first byte
      if (data_rs232_rx[1] == 90) { // Check the second byte
        if (data_rs232_rx[2] == 20) { // Check the number of elemnts in the package

          M_Selected        = data_rs232_rx[4];    
          CtrlMode_Selected = data_rs232_rx[6];    

        //   GUI_stiffness_cmd = data_rs232_rx[7];    
        //   GUI_damping_cmd   = data_rs232_rx[8];    
        //   GUI_force_cmd     = data_rs232_rx[9];    
        //   GUI_assistive_ratio_cmd = data_rs232_rx[10];     

        //   GUI_K_p   = GUI_stiffness_cmd/value_scale;       
        //   GUI_K_d   = GUI_damping_cmd/value_scale;     
        //   GUI_force = GUI_force_cmd/value_scale;    
        //   assistive_ratio = GUI_assistive_ratio_cmd/value_scale/10.0;    

        //   /// reference  
        //   GUI_pos_ampl_cmd    = data_rs232_rx[11];    
        //   GUI_pos_fre_cmd     = data_rs232_rx[12];    
        //   GUI_force_ampl_cmd  = data_rs232_rx[13];      
        //   GUI_force_fre_cmd   = data_rs232_rx[14];      

        //   pos_ampl = GUI_pos_ampl_cmd/value_scale;  
        //   pos_fre  = GUI_pos_fre_cmd/value_scale;  
        //   cmd_ampl = GUI_force_ampl_cmd/value_scale;    
        //   cmd_fre  = GUI_force_fre_cmd/value_scale;   

          // Serial.print("| Motor ");
          // Serial.print(M_Selected, DEC); // This contains the motor number
          // Serial.print(" selected | Control Mode ");
          // Serial.print(CtrlMode_Selected, DEC); // This contains the Control mode
          // Serial.print(" selected | Command ");
          // //Serial.print(GUI_force_cmd, DEC); // This contains the desired command
          // // Serial.print(GUI_K);  
          // Serial.print(GUI_K_p);  
          // Serial.print(GUI_K_d);  
          // Serial.println(" sent |");  
        }
      }
    }
  }
}


void Transmit_ble_Data(){ 
  float value_scale_ratio = 100;    

  int t_teensy = t * value_scale_ratio;      

  // L_leg_IMU_angle = imu.LTx * value_scale_ratio;       
  // R_leg_IMU_angle = imu.RTx * value_scale_ratio;      

  // L_leg_IMU_angle = sig_m1.pos*180/M_PI * value_scale_ratio;       
  // R_leg_IMU_angle = sig_m2.pos*180/M_PI * value_scale_ratio;      

  L_leg_IMU_angle = AngleXLeft * value_scale_ratio;    
  R_leg_IMU_angle = AngleXRight * value_scale_ratio;    

  L_leg_IMU_vel   = VelXLeft * value_scale_ratio;   
  R_leg_IMU_vel   = VelXRight * value_scale_ratio;         

  data_ble[0]  = 165;  
  data_ble[1]  = 90;  
  data_ble[2]  = datalength_ble;  
  data_ble[3]  = L_leg_IMU_angle;  
  data_ble[4]  = L_leg_IMU_angle >> 8;   
  data_ble[5]  = R_leg_IMU_angle;  
  data_ble[6]  = R_leg_IMU_angle >> 8;  
  data_ble[7]  = 0; 
  data_ble[8]  = 0 >> 8; 
  data_ble[9]  = 0;    
  data_ble[10] = 0 >> 8;    
  data_ble[11] = 0;  
  data_ble[12] = 0 >> 8;   
  data_ble[13] = 0;   
  data_ble[14] = 0 >> 8;    
  data_ble[15] = t_teensy; 
  data_ble[16] = t_teensy >> 8;  
  data_ble[17] = L_leg_IMU_vel;  
  data_ble[18] = L_leg_IMU_vel >> 8;  
  data_ble[19] = R_leg_IMU_vel;  
  data_ble[20] = R_leg_IMU_vel >> 8;  
  data_ble[21] = 0;  
  data_ble[22] = 0 >> 8;  
  data_ble[23] = 0;  
  data_ble[24] = 0 >> 8;  
  data_ble[25] = 0;
  data_ble[26] = 0 >> 8;  
  data_ble[27] = 0;
  data_ble[28] = 0 >> 8;  

  Serial5.write(data_ble, datalength_ble);  
  //Serial7.write(data_ble, datalength_ble);  
  //Serial.print("Transmit Data Function Executed");  
}

// #include "im948_CMD.h"

// void setup()
// {
//     Serial.begin(1000000); 
//     Serial7.begin(115200); 
//     Serial.println("--- IM948 arduino test start V1.03 ---");
//     delay(5000); 

//     
//     Cmd_03(); 
//     Cmd_12(5, 255, 0, 0, 3, 2, 2, 4, 9, 0xFFF); // 设置设备参数
//     Cmd_19(); // 数据主动上报
// }

// void loop()
// {
//     // 处理发来数据
//     U8 rxByte;
//     while (Serial7.available() > 0) 
//     { 
//         rxByte = Serial7.read(); // 读取数据
//         Serial.print("Received: 0x"); // 在 Arduino 串口监视器打印
//         Serial.println(rxByte, HEX); 

//       
//         if (Cmd_GetPkt(rxByte)) {
//             Serial.println("Valid data packet received!");
//             break;
//         }
//     }

//     // 处理解析出的数据
//     if (isNewData)
//     {   
//         isNewData = 0;
//         Serial.print("AngleX: "); Serial.println(AngleX);
//         Serial.print("AngleY: "); Serial.println(AngleY);
//         Serial.print("AngleZ: "); Serial.println(AngleZ);
//     }
// }
