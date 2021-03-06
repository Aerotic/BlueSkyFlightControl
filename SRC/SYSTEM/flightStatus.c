/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     flightStatus.c
 * @说明     飞行状态分类与检测
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.05 
**********************************************************************************************************/
#include "flightStatus.h"
#include "rc.h"
#include "navigation.h"

typedef struct
{
    uint8_t init;           //初始化状态
    uint8_t failsafe;       //失控保护状态
	uint8_t armed;          //电机锁定状态
	uint8_t flight;         //飞行状态
	uint8_t placement;      //放置状态
	uint8_t altControl;     //高度控制状态
	uint8_t posControl;     //位置控制状态
	uint8_t mode;
}FLIGHT_STATUS_t;

FLIGHT_STATUS_t flyStatus;

/**********************************************************************************************************
*函 数 名: ArmedCheck
*功能说明: 解锁检查
*形    参: 无
*返 回 值: 检查结果
**********************************************************************************************************/
bool ArmedCheck(void)
{
    if(GetInitStatus() != INIT_FINISH)
        return false;
    
    return true;
}

/**********************************************************************************************************
*函 数 名: SetArmedStatus
*功能说明: 设置飞控锁定状态
*形    参: 状态
*返 回 值: 无
**********************************************************************************************************/
void SetArmedStatus(uint8_t status)
{
	if(status == DISARMED)	//上锁
	{
        //导航数据复位
        NavigationReset();
        
		flyStatus.armed = DISARMED;
	}
	else if(status == ARMED)	//解锁
	{	
        //解锁检查
        if(!ArmedCheck())
            return;
        
        //导航数据复位
        NavigationReset();
        
		flyStatus.armed = ARMED;
	}
}

/**********************************************************************************************************
*函 数 名: GetArmedStatus
*功能说明: 获取飞控锁定状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
uint8_t GetArmedStatus(void)
{
    return flyStatus.armed;
}

/**********************************************************************************************************
*函 数 名: SetFlightStatus
*功能说明: 获取飞行状态
*形    参: 状态
*返 回 值: 无
**********************************************************************************************************/
void SetFlightStatus(uint8_t status)
{
    flyStatus.flight = status;
}

/**********************************************************************************************************
*函 数 名: GetFlightStatus
*功能说明: 获取飞行状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
uint8_t GetFlightStatus(void)
{
    return flyStatus.flight;
}

/**********************************************************************************************************
*函 数 名: PlaceStausCheck
*功能说明: 飞行器放置状态检测：静止或运动
*形    参: 角速度
*返 回 值: 无
**********************************************************************************************************/
void PlaceStausCheck(Vector3f_t gyro)
{
    Vector3f_t gyroDiff;
    static Vector3f_t lastGyro;
    static float threshold = 1.0f;
    static uint16_t checkNum = 0;
    static int16_t count = 0;
    
    gyroDiff.x = gyro.x - lastGyro.x;
    gyroDiff.y = gyro.y - lastGyro.y;    
    gyroDiff.z = gyro.z - lastGyro.z;
    lastGyro = gyro;
    
    if(count < 100)
    {
        count++;
        //陀螺仪数值变化大于阈值
        if(abs(gyroDiff.x) > threshold || abs(gyroDiff.y) > threshold || abs(gyroDiff.z) > threshold)
        {
            checkNum++;
        }
    }
    else
    {
        //陀螺仪数据抖动次数大于一定值时认为飞机不处于静止状态
        if(checkNum > 30)
            flyStatus.placement = MOTIONAL;
        else
            flyStatus.placement = STATIC;
        
        checkNum = 0;
        count = 0;
    }  
}

/**********************************************************************************************************
*函 数 名: GetPlaceStatus
*功能说明: 获取飞行器放置状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
uint8_t GetPlaceStatus(void)
{
    return flyStatus.placement;    
}

/**********************************************************************************************************
*函 数 名: SetAltControlStatus
*功能说明: 设置当前的高度控制状态
*形    参: 状态
*返 回 值: 无
**********************************************************************************************************/
void SetAltControlStatus(uint8_t status)
{
    flyStatus.altControl = status;    
}

/**********************************************************************************************************
*函 数 名: GetAltControlStatus
*功能说明: 设置当前的高度控制状态
*形    参: 状态
*返 回 值: 无
**********************************************************************************************************/
uint8_t GetAltControlStatus(void)
{
    return flyStatus.altControl;    
}

/**********************************************************************************************************
*函 数 名: SetPosControlStatus
*功能说明: 设置当前的位置控制状态
*形    参: 状态
*返 回 值: 无
**********************************************************************************************************/
void SetPosControlStatus(uint8_t status)
{
    flyStatus.posControl = status;    
}

/**********************************************************************************************************
*函 数 名: GetPosControlStatus
*功能说明: 获取当前的位置控制状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
uint8_t GetPosControlStatus(void)
{
    return flyStatus.posControl;    
}

/**********************************************************************************************************
*函 数 名: SetInitStatus
*功能说明: 设置飞控初始化状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
void SetInitStatus(uint8_t status)
{
    flyStatus.init = status;    
}

/**********************************************************************************************************
*函 数 名: GetInitStatus
*功能说明: 获取飞控初始化状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
uint8_t GetInitStatus(void)
{
    return flyStatus.init;    
}

/**********************************************************************************************************
*函 数 名: SetFlightMode
*功能说明: 设置飞行模式
*形    参: 模式
*返 回 值: 无
**********************************************************************************************************/
void SetFlightMode(uint8_t mode)
{
    switch(mode)
    {
        case    MANUAL:
            flyStatus.mode = MANUAL;
            break;
        case    SEMIAUTO:
            flyStatus.mode = SEMIAUTO;
            break;
        case    AUTO:
            flyStatus.mode = AUTO;
            break;
        case    AUTOTAKEOFF:
            //flyStatus.mode = AUTOTAKEOFF;
            break;
        case    AUTOLAND:
            //flyStatus.mode = AUTOLAND;
            break;
        case    RETURNTOHOME:
            //flyStatus.mode = RETURNTOHOME;
            break;
        case    AUTOCIRCLE:
            //flyStatus.mode = AUTOCIRCLE;
            break;
        case    AUTOPILOT:
            //flyStatus.mode = AUTOPILOT;
            break;
        case    FOLLOWME:
            //flyStatus.mode = FOLLOWME;
            break;
		case    0xFF:
            break;
        default:
            flyStatus.mode = AUTO;
            break;
    }
}

/**********************************************************************************************************
*函 数 名: GetFlightMode
*功能说明: 获取当前的飞行模式
*形    参: 无
*返 回 值: 模式
**********************************************************************************************************/
uint8_t GetFlightMode(void)
{
    return flyStatus.mode;    
}

/**********************************************************************************************************
*函 数 名: SetFailSafeStatus
*功能说明: 设置失控状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
void SetFailSafeStatus(bool status)
{
    flyStatus.failsafe = status;
}

/**********************************************************************************************************
*函 数 名: GetFailSafeStatus
*功能说明: 获取失控状态
*形    参: 无
*返 回 值: 状态
**********************************************************************************************************/
bool GetFailSafeStatus(void)
{
    return flyStatus.failsafe;
}



