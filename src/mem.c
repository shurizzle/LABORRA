// https://github.com/acdvorak/mono-meminfo/blob/master/meminfo.c

#include <stdio.h>
#include <unistd.h>
#include <mach/mach.h>

int main(void) {
    mach_port_t host_port;
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmstat;
    vm_size_t pagesize;
    kern_return_t err;

    host_port = mach_host_self();
    err = host_statistics(host_port,
        HOST_VM_INFO,
        (host_info_t) &vmstat,
        &count);

    if (KERN_SUCCESS != err) {
        return err;
    }

    host_page_size(host_port, &pagesize);

    natural_t mem_used = (vmstat.active_count +
                          vmstat.inactive_count +
                          vmstat.wire_count) * pagesize;
    natural_t mem_free = vmstat.free_count * pagesize;
    natural_t mem_total = mem_used + mem_free;

    printf("Active: %u\nWired: %u\nInactive: %u\nFree: %u\nSpeculative: %u\n",
        vmstat.active_count, vmstat.wire_count, vmstat.inactive_count,
        vmstat.free_count, vmstat.speculative_count);
    printf("pagesize: %u\ntotal: %f\n", (unsigned int) pagesize, mem_total / 1024.0 / 1024.0);

    return 0;
}
