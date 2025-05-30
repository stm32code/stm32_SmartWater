#ifndef __GIT__H
#define __GIT__H


//  设备使用外设定义
#define OLED 1			// 是否使用OLED
#define NET_SERVE 0		// 平台选择
#define NETWORK_CHAEK 1 // 是否开启掉线检测
#define KEY_OPEN 1		// 是否开启长按和短按检测
#define USART2_OPEN 0	// 是否使用串口二

// 数据定义
typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

//  C库
#include "cjson.h"
#include <string.h>
#include <stdio.h>
// 单片机头文件
#include "sys.h"
#include "usart.h"	 
// 网络协议层
#include "onenet.h"
// 网络设备
#include "esp8266.h"
// 任务栏
#include "task.h"
#include "timer.h"
// 硬件驱动
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "git.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "JR6001.h"
#include "ds18b20.h"
#include "flash.h"
#include "hx710.h"
#include "level.h"
#include "relay.h"

#if OLED // OLED文件存在
#include "oled.h"
#endif


// 服务器信息
#define SSID "CMCC-KPUD"		// 路由器SSID名称
#define PASS "p675eph9" // 路由器密码
#if NET_SERVE == 0
// 多协议服务器（Onenet旧版支持）
#define ServerIP "183.230.40.39" // 服务器IP地址
#define ServerPort 6002			 // 服务器IP地址端口号
#elif NET_SERVE == 1
// 物联网开发平台（阿里云支持）
#define ServerIP "iot-06z00axdhgfk24n.mqtt.iothub.aliyuncs.com" // 服务器IP地址
#define ServerPort 1883											// 服务器IP地址端口号
#elif NET_SERVE == 2
// EMQX平台（自主）
#define ServerIP "broker.emqx.io" // 服务器IP地址
#define ServerPort 1883			  // 服务器IP地址端口号
#endif
// 设备信息
/*
#define PROID "burn_water_device&h9sjIp8LIU7"															 // 产品ID
#define DEVID "h9sjIp8LIU7.burn_water_device|securemode=2,signmethod=hmacsha256,timestamp=1704470774080|" // 设备ID
#define AUTH_INFO "d8ab484b5849f97cf532111c62179cd5502d890245e6aa8a0dc839b4c97f23d4"						 // 鉴权信息
*/
#define PROID "634589"															 // 产品ID
#define DEVID "1215486857" // 设备ID
#define AUTH_INFO "1"						 // 鉴权信息

// MQTT主题 /broadcast/
#define S_TOPIC_NAME "/broadcast/h9sjIp8LIU7/test1" // 需要订阅的主题
#define P_TOPIC_NAME "/broadcast/h9sjIp8LIU7/test2" // 需要发布的主题

#define P_TOPIC_CMD "/sys/h9sjbaBQFfh/smart_walking_device/thing/event/property/post"

// 自定义布尔类型
typedef enum
{
	MY_TRUE,
	MY_FALSE
} myBool;

// 自定义执行结果类型
typedef enum
{
	MY_SUCCESSFUL = 0x01, // 成功
	MY_FAIL = 0x00		  // 失败

} mySta; // 成功标志位

typedef enum
{
	OPEN = 0x01, // 打开
	CLOSE = 0x00 // 关闭

} On_or_Off_TypeDef; // 成功标志位

typedef enum
{
	DERVICE_SEND = 0x00, // 设备->平台
	PLATFORM_SEND = 0x01 // 平台->设备

} Send_directino; // 发送方向

typedef struct
{
	U8 App;			 // 指令模式
	U8 Device_State; // 模式
	U8 Page;		 // 页面
	U8 Error_Time;
	U8 time_cut_page; // 页面

	F32 temperatuer; // 温度
	F32 Temp_Test;  //温度测试数据
	F32 humiditr;	 // 湿度
	U8 Flage;		 // 模式选择
	U16 people;		 // 人数
	U16 light;		 // 光照
	U16 rain;		 // 雨滴
	U16 wight;  //重量
	

} Data_TypeDef; // 数据参数结构体

typedef struct
{

	U16 somg_value; // 气体阈值
	U16 humi_value; // 湿度阈值
	U16 temp_value; // 温度阈值
	U16 Distance_value; // 距离阈值
	
	
} Threshold_Value_TypeDef; // 数据参数结构体

typedef struct
{

	U8 Key_State;	 // 按键情况
	U8 Fall_State; // 
	U8 Heat_State; // 烧水情况
	
	U8 waning;  //报警
	U8 Voice_goio_1; // 开启和关闭设备
	U8 Voice_goio_2; // 切换模式

} Device_Satte_Typedef; // 状态参数结构体

// 全局引用
extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init; // 设备状态

extern Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data);
// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state);
// 更新OLED显示屏中内容
mySta Update_oled_massage(void);
// 更新设备状态
mySta Update_device_massage(void);
// 解析json数据
mySta massage_parse_json(char *message);
// 验证密码并开锁
void Check_Key_ON_OFF(void);
// 自动关闭舵机
void Automation_Close(void);

#endif
