/** prevent recursive inclusion --------------------------------------------- */

#ifndef WAKEUP_H
#define WAKEUP_H


/** includes ---------------------------------------------------------------- */

#include "../stm8s_conf.h"
#include "wakeup.c"


/** interrupt handlers ------------------------------------------------------ */

//extern INTERRUPT_HANDLER(AWU_IRQHandler, 1);


/** functions --------------------------------------------------------------- */

void wakeup_setup(AWU_Timebase_TypeDef timebase);

uint32_t LSIMeasurment(void);


#endif /* WAKEUP_H */