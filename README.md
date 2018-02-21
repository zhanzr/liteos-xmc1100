XMC1100 Bootkit port Huawei LiteOS.

I would like to only keep the code relevant to my port. The original repository contains too much close-coupled code, which makes the kernel hard to understand and reuse.

Plan to rewrite the kernel. 
Purpose:
	1.Make it simpler and more light than the original LiteOS.
	2.First only support Cortex M0/M0+/M1 core.
	3.Once the Cortex M0 port mature enoough, then to support Cortex M3/M4/M7
	4.Support other cores.
	5.Provide Windows/Linux simulator.
	6.Complete the Middleware interface: FS/Network/USB/Graphic/HMI and so on.
	