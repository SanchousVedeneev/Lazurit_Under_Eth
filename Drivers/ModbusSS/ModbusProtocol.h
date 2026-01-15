#ifndef MODBUS_PROTOCOL_H
#define MODBUS_PROTOCOL_H

#include "ModbusConf.h"

typedef enum{
    MBP_reg_0 = 256,
    MBP_reg_1,
    MBP_reg_2,
    MBP_reg_3,
    MBP_reg_4,
}ModbusProtocol_enum_t;

#endif