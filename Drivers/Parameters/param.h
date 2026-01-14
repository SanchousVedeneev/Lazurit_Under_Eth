
#ifndef __PARAMETERS__H__
#define __PARAMETERS__H__

#include <stdint.h>

typedef enum{
    p_t_unknown = 0,
    // p_t_uint8_t,
    // p_t_uint16_t,
    // p_t_uint32_t,
    // p_t_int8_t,
    // p_t_int16_t,
    p_t_int32_t,
    p_t_float,
    p_t_string,
    p_t_event
}param_t_type;

typedef struct param_s
{
    uint16_t id;
    const char* key;
    param_t_type type;
    void* valuePtr;
    uint16_t valueSize;
    int32_t value;
    void (*event)(struct param_s* param, void* context);
}param_t;


typedef struct param_s_list
{
    uint8_t count;
    param_t* elements;
}param_t_list;



#define PARAM_OK (1)
#define PARAM_ERR_NOT_FOUND (-1)
#define PARAM_ERR_TYPE (-2)
#define PARAM_ERR_VALUE (-3)
#define PARAM_ERR_CGI (-4)
#define PARAM_ERR_SCAN (-5)


// int8_t param_f_setParam(param_t* el, void *value);
// int8_t param_f_setParamFromStr(param_t_list* list, char* key, char* str);
int8_t param_f_setFromCgiStr(param_t_list* list, char* cgiStr);
// param_t* param_f_getParam(param_t_list* list, char* key);
void param_f_checkEvents(param_t_list *list, void* context);
int8_t param_f_isEqual(param_t *param, const void *value);





#endif





