#ifndef __RTC_H__
#define	__RTC_H__

#ifdef LOS_XMC1100
	#include <stdint.h>
	#include <assert.h>

	#include <xmc_scu.h>
	#include <xmc_rtc.h>
#endif

void RTC_Initialize (void);

#endif
