#include "git.h"

// �����ʱ���趨
static Timer task1_id;
static Timer task2_id;
static Timer task3_id;
extern U8 bTimeout500ms; /** < 500���붨ʱ����ʱ��־ */
extern U8 bTimeout250ms; /** < 50���붨ʱ����ʱ��־ */
extern U8 bTimeout50ms;  /** < 50���붨ʱ����ʱ��־ */
// ��ȡȫ�ֱ���
const char *topics[] = {S_TOPIC_NAME};

// Ӳ����ʼ��
void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    HZ = GB16_NUM();                                // ����
    delay_init();                                   // ��ʱ������ʼ��
    GENERAL_TIM_Init(TIM_4, 0, 1);
    Usart1_Init(115200); // ����1��ʼ��Ϊ115200
    Usart2_Init(9600);   // ����ģ��
    Usart3_Init(115200); // ����2������ESP8266��
    System_PB34_setIO();
    LED_Init();
    Key_GPIO_Config(); // key

 
#endif
    while (Reset_Threshole_Value(&threshold_value_init, &device_state_init) != MY_SUCCESSFUL)
        delay_ms(5); // ��ʼ����ֵ

#if OLED // OLED�ļ�����
    OLED_Clear();
#endif
}
// �����ʼ��
void Net_Init()
{

#if OLED // OLED�ļ�����
    char str[50];
    OLED_Clear();
    // дOLED����
    sprintf(str, "-���WIFI�ȵ�");
    OLED_ShowCH(0, 0, (unsigned char *)str);
    sprintf(str, "-����:%s         ", SSID);
    OLED_ShowCH(0, 2, (unsigned char *)str);
   
    while (OneNet_DevLink()) // ����OneNET
        delay_ms(300);
    while (OneNet_Subscribe(topics, 1)) // ��������
        delay_ms(300);

    Connect_Net = 60; // �����ɹ�
#if OLED              // OLED�ļ�����
    OLED_Clear();
#endif
}

// ����1
void task1(void)
{
}
// ����2
void task2(void)
{
    char str[50];
// �豸����
#if NETWORK_CHAEK
    if (Connect_Net == 180) {

#if OLED // OLED�ļ�����
        OLED_Clear();
        // дOLED����
        sprintf(str, "-���WIFI�ȵ�");
        OLED_ShowCH(0, 0, (unsigned char *)str);
        sprintf(str, "-����:%s         ", SSID);
        OLED_ShowCH(0, 2, (unsigned char *)str);
     
            delay_ms(300);
        Connect_Net = 60; // �����ɹ�
#if OLED                  // OLED�ļ�����
        OLED_Clear();
#endif
    }
#endif

    Read_Data(&Data_init);   // ���´���������
  
                             // BEEP= ~BEEP;
    State = ~State;
}
// ����3
void task3(void)
{

    // 2.5��һ��
    if (Connect_Net && Data_init.App == 0) {
        Data_init.App = 1;
    }
}
// �����ʼ��
void SoftWare_Init(void)
{
    // ��ʱ����ʼ��
    timer_init(&task1_id, task1, 50000, 1); // ÿ1�����ϴ�һ���豸����
    timer_init(&task2_id, task2, 100, 1);   // �������ݰ�
    timer_init(&task3_id, task3, 2500, 1);  // ÿ10�뷢��һ�����ݵ��ͻ���

    // timer_start(&task1_id);
    timer_start(&task2_id);
    timer_start(&task3_id);
}
// ������
int main(void)
{

    unsigned char *dataPtr = NULL;
    SoftWare_Init(); // �����ʼ��
    Hardware_Init(); // Ӳ����ʼ��
    Net_Init();		   // �����ʼ
    TIM_Cmd(TIM4, ENABLE); // ʹ�ܼ�����

    Get_Maopi(); // ����
    delay_ms(500);
    Get_Maopi(); // ����

    //relay1out = 0;
    while (1) {
        if (bTimeout50ms == MY_TRUE) {

            bTimeout50ms = MY_FALSE;
        }
        if (bTimeout250ms == MY_TRUE) {
						Get_Weight();
            bTimeout250ms = MY_FALSE;
        }
        if (bTimeout500ms == MY_TRUE) {

            bTimeout500ms = MY_FALSE;
        }

        // �߳�
        timer_loop(); // ��ʱ��ִ��
        // ���ڽ����ж�
        dataPtr = ESP8266_GetIPD(0);
        if (dataPtr != NULL) {
            OneNet_RevPro(dataPtr); // ��������
        }
    }
}
