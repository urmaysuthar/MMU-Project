## Design and Implementation of a Virtual Memory Unit (MMU)

This project consists of writing a program that translates logical to physical addresses for a virtual address space of 
size 2<sup>16</sup> = 65,536 bytes. The program will read from a file containing logical addresses and, using a TLB and a page 
table, translates each logical address to its corresponding physical address and output the value of the byte stored 
at the translated physical address. This project involves resolving page faults using demand paging, managing a TLB, and 
implementing a page-replacement algorithm.
