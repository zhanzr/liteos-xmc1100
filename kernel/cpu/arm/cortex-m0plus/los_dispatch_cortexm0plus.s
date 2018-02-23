;Tiny OS Hardware driver.
;This file is only for Cortex M0+ core, for others cores, use conditional directive to use other drivers.
;
;This IS a part of the kernel.
;
;Author: zhanzr<zhanzr@foxmail.com>
;Date	:	2/22/2018

;Cortex M0+ is binary compatible with Cortex M0, so just include the task dispatch source code of Cortex M0 

	GET	..\cortex-m0\los_dispatch_cortexm0.s
