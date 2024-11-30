# ABI
Calls can be made to the sm by setting a7 to the correct call number and setting all the regs as indicated by the table below.

| NB (a7) | Call name | Who? | Registers |
|---------|-----------|------|------|
|     0    |    sm_init       |   boot only once   |   /   |
|     1    |    sp_init       |   untrusted   |   a0: data_start a1:data_size  => a0:SP_key |
|     2    |      enclave_init     |   untrusted   |   a0:text_start a1: text_size a2:key a3:MAC => a0:enclave_id   |
|     3    |      reserved     |      |      |
|     4    |       run_enclave    |  untrusted    |   a0:id a1-a5: args for the enclave => a0-a6: res from the enclave   |
|     20    |      unprotect     |    enclave & untrusted  |   a0: id or nothing if called from enclave => -1 failure, 1 succes, -2 enclave unprotected   |
