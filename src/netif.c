#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

typedef enum InterfaceType {
    _6_TO_4,
    BLUETOOTH,
    ETHERNET,
    FIREWIRE,
    IEEE80211,
    IP_SEC,
    IR_DA,
    L2TP,
    MODEM,
    PPP,
    SERIAL,
    VLAN,
    WWAN,
    IPv4
} InterfaceType;

char * CFString_to_C_string(CFStringRef cfstring) {
    if (cfstring == NULL) {
        return NULL;
    }

    CFIndex string_len;
    char *string;

    string_len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfstring),
	    kCFStringEncodingUTF8);
    string = (char *)malloc(string_len + 1);
    if (!CFStringGetCString(cfstring, string, string_len + 1,
        kCFStringEncodingUTF8)) {
        free(string);
        return NULL;
    }
    return string;
}

InterfaceType remap_interface_type(CFStringRef interface_type) {
    #define IS_TYPE(x) CFStringCompare(interface_type, x, 0) == kCFCompareEqualTo
    #define IF_IS_TYPE(x) if (IS_TYPE(x))
    #define MAP_TYPE(x,y) IF_IS_TYPE(x){return y;}

    MAP_TYPE(kSCNetworkInterfaceType6to4, _6_TO_4);
    MAP_TYPE(kSCNetworkInterfaceTypeBluetooth, BLUETOOTH);
    MAP_TYPE(kSCNetworkInterfaceTypeBond, ETHERNET);
    MAP_TYPE(kSCNetworkInterfaceTypeEthernet, ETHERNET);
    MAP_TYPE(kSCNetworkInterfaceTypeFireWire, FIREWIRE);
    MAP_TYPE(kSCNetworkInterfaceTypeIEEE80211, IEEE80211);
    MAP_TYPE(kSCNetworkInterfaceTypeIPSec, IP_SEC);
    MAP_TYPE(kSCNetworkInterfaceTypeIrDA, IR_DA);
    MAP_TYPE(kSCNetworkInterfaceTypeL2TP, L2TP);
    MAP_TYPE(kSCNetworkInterfaceTypeModem, MODEM);
    MAP_TYPE(kSCNetworkInterfaceTypePPP, PPP);
    MAP_TYPE(kSCNetworkInterfaceTypeSerial, SERIAL);
    MAP_TYPE(kSCNetworkInterfaceTypeVLAN, VLAN);
    MAP_TYPE(kSCNetworkInterfaceTypeWWAN, WWAN);

    return IPv4;

    #undef MAP_TYPE
    #undef IF_IS_TYPE
    #undef IS_TYPE
}

int main(void) {
    CFArrayRef network_interfaces = SCNetworkInterfaceCopyAll();
    CFIndex num_interfaces = CFArrayGetCount(network_interfaces);
    for (CFIndex i = 0; i < num_interfaces; i++) {
        SCNetworkInterfaceRef interface = (SCNetworkInterfaceRef) CFArrayGetValueAtIndex(network_interfaces, i);
        CFStringRef bsdname_CFString = SCNetworkInterfaceGetBSDName(interface);
        if (bsdname_CFString == NULL) {
            continue;
        }

        CFStringRef friendly_name_CFString = SCNetworkInterfaceGetLocalizedDisplayName(interface);
        CFStringRef interface_type_CFString = SCNetworkInterfaceGetInterfaceType(interface);
        InterfaceType type = remap_interface_type(interface_type_CFString);
        if (interface_type_CFString) {
            CFRelease(interface_type_CFString);
        }
        char *bsdname = CFString_to_C_string(bsdname_CFString);
        char *friendly_name = CFString_to_C_string(friendly_name_CFString);
        if (bsdname_CFString) {
            CFRelease(bsdname_CFString);
        }
        if (friendly_name_CFString) {
            CFRelease(friendly_name_CFString);
        }

        printf("bsdname: %s\nfriendly_name: %s\ntype: %d\n\n", bsdname, friendly_name, type);

        if (bsdname) {
            free(bsdname);
        }
        if (friendly_name) {
            free(friendly_name);
        }
    }

    // if (network_interfaces) {
    //     CFRelease(network_interfaces);
    // }
    return 0;
}
