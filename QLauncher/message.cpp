#include "message.h"

//int GetMessageSize(unsigned char ucCode)
//{
//    switch (ucCode)
//    {
//    // 데이타 사이즈 1
//    case CODE_H2VIS_EXIT:             
//    case CODE_H2VIS_SCENARIO_CTRL:       
//    case CODE_H2VIS_DRVVIEW_CTRL:
//
//    case CODE_VIS2H_BITINFO:              
//    case CODE_VIS2H_SCENARIO_CTRL_ACK:
//
//    case CODE_IOS2H_OPERATE_TRAINING: 
//
//    case CODE_MF2H_BITINFO:
//    case CODE_MF2H_SCENARIO_CTRL_ACK:
//
//    case CODE_H2MF_SCENARIO_CTRL:
//        return HEADER_SIZE + DATASIZE_1;
//    
//    // 데이타 사이즈 2
//    case CODE_H2VIS_WEATHER_CTRL:         
//    case CODE_H2VIS_SHOOTING:             
//    case CODE_H2VIS_ANIMATION_CTRL:       
//    case CODE_H2VIS_HEADLIGHT_CTRL:       
//    case CODE_H2VIS_HUMANMODEL_CTRL:      
//    case CODE_H2VIS_TERRAINSWITCH:
//
//    case CODE_VIS2H_ANIMATION_CTRL_ACK:   
//
//    case CODE_IOS2H_SET_SCENARIO:   
//
//    case CODE_H2IOS_CONNECTION_STATE: 
//        return HEADER_SIZE + DATASIZE_2;
//
//    // 데이타 사이즈 4
//    case CODE_H2VIS_CMDVIEW_K1A1KCPS:     
//        return HEADER_SIZE + DATASIZE_4;
//
//    // 데이타 사이즈 5
//    case CODE_H2VIS_GUNVIEW_K1A1KGPS:     
//        return HEADER_SIZE + DATASIZE_5;
//
//    // 데이타 사이즈 10
//    case CODE_H2VIS_OBJECT_CTRL:
//
//    case CODE_H2VIS_FX:                   
//        return HEADER_SIZE + DATASIZE_10;        
//
//    // 데이타 사이즈 11
//    case CODE_H2VIS_MOVEMENTOBJECT:       
//        return HEADER_SIZE + DATASIZE_11;
//    
//    // 데이타 사이즈 17
//    case CODE_H2VIS_TRAINVEHICLE:         
//        return HEADER_SIZE + DATASIZE_17;
//    
//    default:                        return 0;        
//    }
//}

//NetMessage* CreateMessage(unsigned char ucCode)
//{
//    switch (ucCode)
//    {
//    // HOST 2 VIS
//    case CODE_H2VIS_EXIT:                     return new H2VISExit;
//    case CODE_H2VIS_SCENARIO_CTRL:            return new H2VISScenarioControl;
//    case CODE_H2VIS_WEATHER_CTRL:             return new H2VISWeatherControl;
//    case CODE_H2VIS_OBJECT_CTRL:              return new H2VISObjectControl;
//    case CODE_H2VIS_SHOOTING:                 return new H2VISShooting;
//    case CODE_H2VIS_ANIMATION_CTRL:           return new H2VISAnimationCtrl;
//    case CODE_H2VIS_HEADLIGHT_CTRL:           return new H2VISHeadLightCtrl;
//    case CODE_H2VIS_CMDVIEW_K1A1KCPS:         return new H2VISCommanderViewCtrl_K1A1KCPS;
//    case CODE_H2VIS_GUNVIEW_K1A1KGPS:         return new H2VISGunnerViewCtrl_K1A1KGPS;
//    case CODE_H2VIS_DRVVIEW_CTRL:             return new H2VISDriverViewCtrl;
//    case CODE_H2VIS_HUMANMODEL_CTRL:          return new H2VISHumanModelCtrl;
//    case CODE_H2VIS_FX:                       return new H2VISEffectCtrl;
//    case CODE_H2VIS_TERRAINSWITCH:            return new H2VISTerrainSwitchCtrl;
//    case CODE_H2VIS_TRAINVEHICLE:             return new H2VISTrainVehicleCtrl;
//    case CODE_H2VIS_MOVEMENTOBJECT:           return new H2VISMovementObjectCtrl;
//
//    // VIS 2 HOST
//    case CODE_VIS2H_BITINFO:                    return new VIS2HBITInfo;
//    case CODE_VIS2H_SCENARIO_CTRL_ACK:          return new VIS2HScenarioCtrlAck;
//    case CODE_VIS2H_ANIMATION_CTRL_ACK:         return new VIS2HAnimationCtrlAck;
//
//    // IOS 2 HOST
//    case CODE_IOS2H_SET_SCENARIO:               return new IOS2HSetScenario;
//    case CODE_IOS2H_OPERATE_TRAINING:           return new IOS2HOperateTraining;
//
//    // HOST 2 IOS
//    case CODE_H2IOS_CONNECTION_STATE:           return new H2IOSConnectionState;
//
//    // HOST 2 MF
//    case CODE_H2MF_SCENARIO_CTRL:               return new H2MFScenarioControl;
//
//    // MF 2 HOST
//    case CODE_MF2H_BITINFO:                     return new MF2HBITInfo;
//    case CODE_MF2H_SCENARIO_CTRL_ACK:           return new MF2HScenarioCtrlAck;
//    
//    default:                        return 0;
//    }
//}