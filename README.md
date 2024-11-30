Spock : a software-base TEE
==============
Spock is an open source software-based Trusted Execution Environment (TEE) designed for RISC-V embedded devices. It relies on Physical Memory Protection (PMP) and requires only machine and user modes as specified in the RISC-V privileged instruction set. By leveraging PMP, Spock creates secure memory regions, or enclaves, to protect sensitive data and code from unauthorized access, including from the operating system.

Spock guarantees confidentiality and integrity for the enclaves. Spock’s architecture revolves around the Security Manager (SM), which manages enclave data and buffer permissions. This design enables efficient virtualization of memory management and dynamic allocation of PMP entries, allowing the creation of virtual enclaves beyond hardware-imposed limitations.

With its minimalist API, Spock provides essential security functions, including secure execution and attestation, while maintaining a small trusted computing base (TCB). Its flexible design makes it suitable for very low resource embedded devices, offering a solution for both relocatable and fixed enclaves. Spock delivers robust security tailored for modern embedded computing in a connected world.
