#include <stdio.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <mach/mach.h>

int main(void) {
    int64_t mem_phys = 0;
    {
        int mib[] = { CTL_HW, HW_MEMSIZE };
        size_t length = sizeof(mem_phys);

        if (sysctl(mib, 2, &mem_phys, &length, NULL, 0) == -1) {
            return 1;
        }
    }

    vm_statistics_data_t vmstat;
    {
        mach_port_t host_port;
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        kern_return_t err;

        host_port = mach_host_self();
        err = host_statistics(host_port,
            HOST_VM_INFO,
            (host_info_t) &vmstat,
            &count);

        if (KERN_SUCCESS != err) {
            return err;
        }
    }

    double mem_used = vmstat.active_count + vmstat.wire_count;
    double mem_total = mem_used + vmstat.inactive_count + vmstat.free_count +
                        vmstat.speculative_count;

    printf("{\"total\":%lld,\"used\":%lld}",
        mem_phys,
        (int64_t) (mem_used / mem_total * mem_phys));

    return 0;
}
