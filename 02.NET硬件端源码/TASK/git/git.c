#include "git.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{
	// 温度测量
	Data_init.Temp_Test = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // 获取温度
	if (Data_init.Temp_Test > 1)
	{
		Data_init.temperatuer = Data_init.Temp_Test;
	}

	return MY_SUCCESSFUL;
}
// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

//	Value->temp_value = 100;
//	//写
//	W_Test();
//	// 读
	R_Test();
	// 状态重置
	device_state_init.Voice_goio_1 = 0;
	device_state_init.Voice_goio_2 = 0;
	device_state_init.waning = 0;
	threshold_value_init.temp_value = 31;
	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
	char str[50];
	if (Data_init.Flage)
	{
		sprintf(str, "模式 : 恒温模式  ");
	}
	else
	{
		sprintf(str, "模式 : 正常模式  ");
	}

	OLED_ShowCH(0, 0, (unsigned char *)str);

	sprintf(str, "水温阈值 : %d  度", threshold_value_init.temp_value);
	OLED_ShowCH(0, 2, (unsigned char *)str);

	sprintf(str, "水  温 : %.2f  度", Data_init.temperatuer);

	OLED_ShowCH(0, 4, (unsigned char *)str);
	if (Data_init.wight < 50)
	{
		sprintf(str, "空烧警告!!  ");
	}
	else
	{
		if (relay1in == 1)
		{
			sprintf(str, "烧水  ~~    ");
		}
		else
			sprintf(str, "无异常~~    ");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// 更新设备状态
mySta Update_device_massage()
{

	// 语音控制切换模式
	if (LEVEL2 != device_state_init.Voice_goio_2)
	{
		device_state_init.Voice_goio_2 = LEVEL2;
		if (device_state_init.Voice_goio_2 == 1)
		{
			// 恒温模式
			Data_init.Flage = 1;
		}
		else
		{
			// 正常模式
			Data_init.Flage = 0;
		}
	}
	if(Data_init.Flage==1){
		if (Data_init.temperatuer < threshold_value_init.temp_value && Data_init.wight > 50)
		{
			// 开始烧水
			relay1out = 1;
			
		}
		else
		{
			// 停止烧水
			relay1out = 0;
		}
	}else{
	
		if ( Data_init.temperatuer > threshold_value_init.temp_value || Data_init.wight < 50)
		{
			// 烧水完成
			if(Data_init.temperatuer > threshold_value_init.temp_value && device_state_init.waning == 16 ){
				device_state_init.waning = 15;
				device_state_init.Voice_goio_1 = 0;
			}
			// 停止烧水
			relay1out = 0;
		}else	if (LEVEL1 == 1 )
		{
				// 开始烧水
				relay1out = 1;
				// device_state_init.waning = 16;
		}else if (LEVEL1 == 0 && device_state_init.Voice_goio_1 == 0){
				// 停止烧水
				relay1out = 0;
		}
		
	}



	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			OneNet_SendMqtt(1); // 发送数据到APP
			break;
		}
		Data_init.App = 0;
	}

	return MY_SUCCESSFUL;
}

// 定时器
void Automation_Close(void)
{
	
	if(device_state_init.waning > 0 && device_state_init.waning <= 15 ){
		device_state_init.waning--;
		if(device_state_init.waning == 1){
			// 已关闭
			JR6001_SongControl(2,0);
		}else if(device_state_init.waning > 1){
			// 播放音乐
			JR6001_SongControl(4,1);
		}
	}
}
// 检测按键是否按下
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// 与上一次的键值比较 如果不相等，表明有键值的变化，开始计时
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // 时间记录器
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // 短按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
			// 模式切换
			if (Data_init.Flage)
			{
				Data_init.Flage = 0;
			}
			else
			{
				Data_init.Flage = 1;
			}
			// 已切换
			JR6001_SongControl(3,0);
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
			if (relay1in == 0 && Data_init.wight > 50)
			{
				// 开始烧水
				relay1out = 1;
				// 已开启
				JR6001_SongControl(1,0);
				device_state_init.waning = 16;
				device_state_init.Voice_goio_1 = 1; 
			}
			else
			{
				// 停止烧水
				relay1out = 0;
				// 已关闭
				JR6001_SongControl(2,0);
				device_state_init.Voice_goio_1 = 0;
			}

			break;
		case KEY3_PRES:
			printf("Key3_Short\n");
			if(threshold_value_init.temp_value>30){
					threshold_value_init.temp_value --;
			}
			W_Test();
			break;
		case KEY4_PRES:
			printf("Key4_Short\n");
			if(threshold_value_init.temp_value<100){
					threshold_value_init.temp_value ++;
			}
			W_Test();
			break;
		default:
			break;
		}
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // 长按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");

			break;
		case KEY2_PRES:
			printf("Key2_Long\n");

			break;
		case KEY3_PRES:
			printf("Key3_Long\n");

			break;
		default:
			break;
		}
		num_on = 0;
	}
}
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	cJSON *cjson_data = NULL; // 数据
	const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */
	cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // 消息包
		Data_init.Flage = cJSON_GetObjectItem(cjson_data, "flage")->valueint;
		break;
	case 0x02: // 消息包
		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_data, "temp_v")->valueint;
		W_Test();

		break;
	case 0x03: // 数据包
		if (Data_init.Flage == 0)
		{
			relay1out = cJSON_GetObjectItem(cjson_data, "hot")->valueint;
			if(relay1in == 0){
				device_state_init.Voice_goio_1 = 1;
			}
		}

		break;
	case 0x04: // 数据包
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
