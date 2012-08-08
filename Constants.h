#pragma once

const int D2Display = 0x6FB1E120;
#define BIGLOG false
#define LOGFILE "iagolog.txt"


#define PROCESSCHAT_ADDR (void*) (0x6FAD2CC8)
#define PROCESSCHAT_SIZE (DWORD) (0x6FAD2CD4 - 0x6FAD2CC8)

#define LOSEHEALTH_ADDR  (void*) (0x6FAFF361)
#define LOSEHEALTH_SIZE  (DWORD) (6)

#define SEND_ADDR		 (void*) (0x6FC017BE)
#define SEND_SIZE        (DWORD) (0x6FC017C4 - 0x6FC017BE)

