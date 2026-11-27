#include "App_Main.h"
#include "App_Work.h"
#include "App_LowPower.h"
#include "Com_Debug.h"

void App_main(void)
{
    for (;;) {
        App_LowPower_Exit();
        App_Work_Init();
        App_Work_Execute();
        App_LowPower_Enter();
    }
}
