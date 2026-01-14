
#include "param.h"
//#include <stddef.h>

#include "main.h"
#include <string.h>
#include <stdio.h>


int16_t param_f_findParamIdx(param_t_list *list, char *key, uint16_t keyLen)
{
   int16_t idx = -1;
    int16_t len = 0;
    for(int i = 0; i < list->count ; i++){
        len = strlen(list->elements[i].key);
        if(
        (len == keyLen) &&
        (strncmp(list->elements[i].key, key, len)==0)
        ){
        // printf("Param idx = %d" ,i);
            idx = i;
            break;
        }
    }
    return idx;
}

int8_t param_f_setParam(param_t *el, void *value)
{
    if (el == NULL)
        return PARAM_ERR_NOT_FOUND;

    uint8_t status = PARAM_OK;
    switch (el->type)
    {
    case p_t_int32_t:
        int32_t newVal = (int32_t)value;
        if (newVal != el->value)
        {
            el->value = newVal;
            if (el->event != NULL)
            {
                el->event(el,NULL);
            }
        }
        break;
    case p_t_float:
        if (value == NULL) return PARAM_ERR_VALUE;
        float *fl = (float *)el->value;
        float flVal = *(float *)el->value;
        if (*fl != flVal)
        {
            *fl = flVal;
            if (el->event != NULL)
            {
                el->event(el,NULL);
            }
        }
        break;
    case p_t_string:
        if (value == NULL) return PARAM_ERR_VALUE;
        char *chBuf = (char *)el->valuePtr;
        char *inStr = (char *)value;
        // if (strlen(inStr) >= el->valueSize)
        // {
            strncpy(chBuf, inStr, el->valueSize);
        // }
        // else
        // {
        //     strcpy(chBuf, inStr);
        // }
        if (el->event != NULL)
        {
            el->event(el,NULL);
        }

        break;
    case p_t_event:
        el->value += 1;
        break;
    default:
        status = PARAM_ERR_TYPE;
        break;
    }
    return status;
}

// int8_t param_f_setParamFromStr(param_t_list *list, char *key, char *str)
// {
//     param_t *el = NULL;
//     __IO int16_t paramIdx = 0;
//     if ((paramIdx = param_f_findParamIdx(list, key)) < 0)
//     {
//         return PARAM_ERR_NOT_FOUND;
//     }
//     el = &list->elements[paramIdx];
//     asm("NOP");

//     void *value = NULL;
//     switch (el->type)
//     {
//     case p_t_int32_t:
//         int32_t s32 = 0;
//         sscanf(str, "%ld", &s32);
//         value = (void *)s32;
//         break;
//     case p_t_string:
//         value = (void*)str;
//     break;
//     default:
//         break;
//     }

//     return param_f_setParam(el, value);
// }

int8_t param_f_setFromCgiStr(param_t_list *list, char *cgiStr)
{
    char* equals = strchr(cgiStr, '=');
    char* nextParam = strchr(cgiStr,'&');
    if(equals == NULL) return PARAM_ERR_CGI;
    uint16_t keyLen = equals - cgiStr;
    int16_t idx = param_f_findParamIdx(list,cgiStr,keyLen);
    if(idx == -1){
        return idx;
    }
    param_t *p = &list->elements[idx];
    equals += 1;
    uint16_t dataLen = 0;
    switch (p->type)
    {
    case p_t_int32_t:
        int32_t s32 = 0;
        if (sscanf(equals, "%ld", &s32))
        {
            p->value = s32;
        }
        else
            return PARAM_ERR_SCAN;
        break;
    case p_t_string:
    {
        memset(p->valuePtr, 0, p->valueSize);
        if (nextParam == NULL) dataLen = strlen(equals);
        else dataLen = nextParam - equals;

        if (dataLen <= p->valueSize)
            strncpy(p->valuePtr, equals, dataLen);
        else
            return PARAM_ERR_VALUE;
    }
        break;
    case p_t_float:{
        float f32 = 0;
        if (sscanf(equals, "%f", &f32))
        {
            float* pf32 = (float*)&p->value;
            *pf32 = f32;
        }
        else
            return PARAM_ERR_SCAN;
    }
        break;
    case p_t_event:{
        p->value ++;
    }
    break;
    default:
        return PARAM_ERR_TYPE;
        break;
    }

    return PARAM_OK;
}

// param_t *param_f_getParam(param_t_list *list, char *key)
// {
//     for (uint8_t i = 0; i < list->count; i++)
//     {
//         param_t *el = &list->elements[i];
//         size_t keyLen = strlen(el->key);
//         if (strncmp(key, el->key, keyLen) == 0)
//         {
//             return el;
//         }
//     }
//     return NULL;
// }

void param_f_checkEvents(param_t_list *list, void* context)
{

    for (size_t i = 0; i < list->count; i++)
    {
        param_t *el = &list->elements[i];
        if (el->type == p_t_event)
        {
            while (el->value)
            {
                if (el->event != NULL)
                {
                    el->value--;
                    el->event(el,context);
                }
                else
                {
                    break;
                }
            }
        }
    }
}

int8_t param_f_isEqual(param_t *param, const void *value)
{
    int8_t ret = RESET;
    if(param==NULL) return PARAM_ERR_NOT_FOUND;
    switch (param->type)
    {
    case p_t_string:
        if(strcmp(param->valuePtr,value)==0){
            ret = SET;
        }
        break;
    
    default:
        break;
    }
    return ret;
}
