
#ifndef __ETH__H__
#define __ETH__H__

#include "main.h"

#include "EthConf.h"

typedef enum{
  state_idle = 0,
  state_req_inProc,
  state_req_done,
  state_res_inProc,
  state_res_done,
  state_close
}Eth_t_SocExchangeState;

typedef struct Eth_s_SocTcpLoopBack
{
  uint8_t echoMode;
  uint8_t hwSocNum;
  uint16_t port;
  uint8_t* bufRx;
  uint16_t bufRxLen;
  uint8_t* bufTx;
  uint16_t bufTxLen;
  Eth_t_SocExchangeState  exchangeState;
  void (* cbfunc_newConnection)(struct Eth_s_SocTcpLoopBack* socketObj);
  int32_t (* cbfunc_pendingData)(struct Eth_s_SocTcpLoopBack* socketObj);
  void (* cbfunc_clientDisconnected)();
}Eth_t_SocTcpLoopBack;

typedef struct Eth_s_SocTcp
{
  uint8_t hwSocNum;
  uint16_t port;
  uint8_t* bufRx;
  uint16_t bufRxLen;
  uint8_t* bufTx;
  uint16_t bufTxLen;
  uint8_t echoMode;
  Eth_t_SocExchangeState  exchangeState;
  uint16_t txFrameSize;
  uint32_t txTotalSize;
  uint8_t  txContentIdx;
  void (* cbfunc_newConnection)(struct Eth_s_SocTcp* socketObj);
  int32_t (* cbfunc_pendingData)(struct Eth_s_SocTcp* socketObj);
  int32_t (* cbfunc_sendData)(struct Eth_s_SocTcp* socketObj);
  void (* cbfunc_clientDisconnected)();
  void *parent;
}Eth_t_SocTcp;



uint8_t Eth_f_GetMacFromUID(uint8_t* mac);
uint8_t Eth_f_Init();
int32_t Eth_f_runTcpSrv(Eth_t_SocTcp* socketObj);









#endif