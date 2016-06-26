#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "bspconfig.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include "em_rmu.h"

int main(void)
{
	CHIP_Init();

	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
	EMU_DCDCInit(&dcdcInit);
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
	CMU_HFXOInit(&hfxoInit);

	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

	uint32_t resetCause;
	resetCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	if(resetCause & RMU_RSTCAUSE_EM4RST)
	{
		// EM4 wake up
		CMU_ClockEnable(cmuClock_CRYOTIMER, true);
		CRYOTIMER_IntClear(CRYOTIMER_IntGet());
	}
	else
	{
		// Power on or external reset
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

		CMU_ClockEnable(cmuClock_CRYOTIMER, true);

		CRYOTIMER_Init_TypeDef cryoTimer = CRYOTIMER_INIT_DEFAULT;

		cryoTimer.em4Wakeup = true;
		cryoTimer.osc = cryotimerOscLFXO;
		cryoTimer.period = cryotimerPeriod_64k;

		CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
		CRYOTIMER_Init(&cryoTimer);
	}

	EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
	em4Init.retainLfxo = true;
	EMU_EM4Init(&em4Init);

	uint32_t i = 0xFFFFF;
	while(i)
		i--;

	EMU_EnterEM4();

	while(1);

}
