Diretorias do Projeto:
CircuitRouter-SimpleShell corresponde à shell realizada na primeira entrega de projeto. (Não relevante para esta estrega).
CircuitRouter-SeqSolver corresponde à versão sequencial.
CircuitRouter-ParSolver corresponde à versão paralelizada, com granularidade fina.
As pastas lib e inputs não foram mudadas de sitio.
A pasta results tem os ficheiros gerados pelo doTest.sh.

O script doTest.sh faz o que foi pedido no enunciado, e o testAll.sh corre o doTest.sh para todos os ficheiros em inputs/ que acabem em .txt (tendo alterado o sufixo random-x512-y512-z7-n512 de .txt para .notest para evitar testar).

Output de lscpu (mais sucinto que o /proc/cpuinfo e tem a informação necessária):
Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              8
On-line CPU(s) list: 0-7
Thread(s) per core:  2
Core(s) per socket:  4
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               70
Model name:          Intel(R) Core(TM) i7-4750HQ CPU @ 2.00GHz
Stepping:            1
CPU MHz:             798.223
CPU max MHz:         3200.0000
CPU min MHz:         800.0000
BogoMIPS:            3992.09
Virtualization:      VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            6144K
L4 cache:            131072K
NUMA node0 CPU(s):   0-7
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm cpuid_fault epb invpcid_single pti ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid xsaveopt dtherm ida arat pln pts flush_l1d

Output de uname -a:
Linux arch 4.18.16-arch1-1-ARCH #1 SMP PREEMPT Sat Oct 20 22:06:45 UTC 2018 x86_64 GNU/Linux
