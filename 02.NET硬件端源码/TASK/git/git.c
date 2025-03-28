#include "git.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	// �¶Ȳ���
	Data_init.Temp_Test = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // ��ȡ�¶�
	if (Data_init.Temp_Test > 1)
	{
		Data_init.temperatuer = Data_init.Temp_Test;
	}

	return MY_SUCCESSFUL;
}
// ��ʼ��
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

//	Value->temp_value = 100;
//	//д
//	W_Test();
//	// ��
	R_Test();
	// ״̬����
	device_state_init.Voice_goio_1 = 0;
	device_state_init.Voice_goio_2 = 0;
	device_state_init.waning = 0;
	threshold_value_init.temp_value = 31;
	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];
	if (Data_init.Flage)
	{
		sprintf(str, "ģʽ : ����ģʽ  ");
	}
	else
	{
		sprintf(str, "ģʽ : ����ģʽ  ");
	}

	OLED_ShowCH(0, 0, (unsigned char *)str);

	sprintf(str, "ˮ����ֵ : %d  ��", threshold_value_init.temp_value);
	OLED_ShowCH(0, 2, (unsigned char *)str);

	sprintf(str, "ˮ  �� : %.2f  ��", Data_init.temperatuer);

	OLED_ShowCH(0, 4, (unsigned char *)str);
	if (Data_init.wight < 50)
	{
		sprintf(str, "���վ���!!  ");
	}
	else
	{
		if (relay1in == 1)
		{
			sprintf(str, "��ˮ  ~~    ");
		}
		else
			sprintf(str, "���쳣~~    ");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// �����豸״̬
mySta Update_device_massage()
{

	// ���������л�ģʽ
	if (LEVEL2 != device_state_init.Voice_goio_2)
	{
		device_state_init.Voice_goio_2 = LEVEL2;
		if (device_state_init.Voice_goio_2 == 1)
		{
			// ����ģʽ
			Data_init.Flage = 1;
		}
		else
		{
			// ����ģʽ
			Data_init.Flage = 0;
		}
	}
	if(Data_init.Flage==1){
		if (Data_init.temperatuer < threshold_value_init.temp_value && Data_init.wight > 50)
		{
			// ��ʼ��ˮ
			relay1out = 1;
			
		}
		else
		{
			// ֹͣ��ˮ
			relay1out = 0;
		}
	}else{
	
		if ( Data_init.temperatuer > threshold_value_init.temp_value || Data_init.wight < 50)
		{
			// ��ˮ���
			if(Data_init.temperatuer > threshold_value_init.temp_value && device_state_init.waning == 16 ){
				device_state_init.waning = 15;
				device_state_init.Voice_goio_1 = 0;
			}
			// ֹͣ��ˮ
			relay1out = 0;
		}else	if (LEVEL1 == 1 )
		{
				// ��ʼ��ˮ
				relay1out = 1;
				// device_state_init.waning = 16;
		}else if (LEVEL1 == 0 && device_state_init.Voice_goio_1 == 0){
				// ֹͣ��ˮ
				relay1out = 0;
		}
		
	}



	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			OneNet_SendMqtt(1); // �������ݵ�APP
			break;
		}
		Data_init.App = 0;
	}

	return MY_SUCCESSFUL;
}

// ��ʱ��
void Automation_Close(void)
{
	
	if(device_state_init.waning > 0 && device_state_init.waning <= 15 ){
		device_state_init.waning--;
		if(device_state_init.waning == 1){
			// �ѹر�
			JR6001_SongControl(2,0);
		}else if(device_state_init.waning > 1){
			// ��������
			JR6001_SongControl(4,1);
		}
	}
}
// ��ⰴ���Ƿ���
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// ����һ�εļ�ֵ�Ƚ� �������ȣ������м�ֵ�ı仯����ʼ��ʱ
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // ʱ���¼��
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // �̰�
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
			// ģʽ�л�
			if (Data_init.Flage)
			{
				Data_init.Flage = 0;
			}
			else
			{
				Data_init.Flage = 1;
			}
			// ���л�
			JR6001_SongControl(3,0);
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
			if (relay1in == 0 && Data_init.wight > 50)
			{
				// ��ʼ��ˮ
				relay1out = 1;
				// �ѿ���
				JR6001_SongControl(1,0);
				device_state_init.waning = 16;
				device_state_init.Voice_goio_1 = 1; 
			}
			else
			{
				// ֹͣ��ˮ
				relay1out = 0;
				// �ѹر�
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
	else if (key == 0 && num_on >= Key_Scan_Time) // ����
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
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��
		Data_init.Flage = cJSON_GetObjectItem(cjson_data, "flage")->valueint;
		break;
	case 0x02: // ��Ϣ��
		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_data, "temp_v")->valueint;
		W_Test();

		break;
	case 0x03: // ���ݰ�
		if (Data_init.Flage == 0)
		{
			relay1out = cJSON_GetObjectItem(cjson_data, "hot")->valueint;
			if(relay1in == 0){
				device_state_init.Voice_goio_1 = 1;
			}
		}

		break;
	case 0x04: // ���ݰ�
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
