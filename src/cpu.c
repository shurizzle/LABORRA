#include <stdio.h>
#include <mach/mach.h>

int main(void) {
    processor_cpu_load_info_t cpuLoad;
    mach_msg_type_number_t processorMsgCount;
    natural_t processorCount;

    kern_return_t err = host_processor_info(mach_host_self(),
        PROCESSOR_CPU_LOAD_INFO,
        &processorCount,
        (processor_info_array_t *)&cpuLoad,
        &processorMsgCount);

    if (err != KERN_SUCCESS) {
        return err;
    }

    putchar('[');
    for (natural_t i = 0; i < processorCount; i++) {
        if (i != 0) {
            putchar(',');
        }

        printf("{\"system\":%d,\"user\":%d,\"idle\":%d}",
            cpuLoad[i].cpu_ticks[CPU_STATE_SYSTEM],
            cpuLoad[i].cpu_ticks[CPU_STATE_USER] + cpuLoad[i].cpu_ticks[CPU_STATE_NICE],
            cpuLoad[i].cpu_ticks[CPU_STATE_IDLE]);
    }
    putchar(']');

    return 0;
}
