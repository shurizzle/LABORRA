#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netdb.h>

#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/CaptiveNetwork.h>

typedef enum InterfaceType {
    _6_TO_4,
    BLUETOOTH,
    BOND,
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
    #define MAP_TYPE(x,y) if(IS_TYPE(x)){return y;}

    MAP_TYPE(kSCNetworkInterfaceType6to4, _6_TO_4);
    MAP_TYPE(kSCNetworkInterfaceTypeBluetooth, BLUETOOTH);
    MAP_TYPE(kSCNetworkInterfaceTypeBond, BOND);
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
    #undef IS_TYPE
}

void free_string_array(char **array) {
    char **iter = array;
    for (; *iter; iter++) {
        free(*iter);
    }
    free(array);
}

bool is_in_array(char *what, char **iter) {
    for (; *iter; iter++) {
        if (!strcmp(what, *iter)) {
            return true;
        }
    }

    return false;
}

id CWWiFiClient;

char ** get_wifi_supported_interfaces() {
    CFArrayRef interfaces = (CFArrayRef)objc_msgSend(CWWiFiClient, sel_registerName("interfaceNames"));
    if (!interfaces) {
        return calloc(1, sizeof(char*));
    }
    CFIndex num_interfaces = CFArrayGetCount(interfaces);
    char **res = malloc((num_interfaces + 1) * sizeof(char*));
    res[num_interfaces] = NULL;

    for (CFIndex i = 0; i < num_interfaces; i++) {
        CFStringRef name = (CFStringRef) CFArrayGetValueAtIndex(interfaces, i);
        res[i] = CFString_to_C_string(name);
        CFRelease(name);
    }

    return res;
}

void get_wifi_infos(char *interface, char **bssid, char **ssid, bool *is_active) {
    CFStringRef name = CFStringCreateWithCString(kCFAllocatorDefault, interface, kCFStringEncodingUTF8);
    id client = (id)objc_msgSend(CWWiFiClient, sel_registerName("sharedWiFiClient"));
    id iface = (id)objc_msgSend(client, sel_registerName("interface"), name);

    if (!iface) {
        return;
    }

    *is_active = !!objc_msgSend(iface, sel_registerName("powerOn"));

    if (*is_active) {
        CFStringRef CFbssid = (CFStringRef)objc_msgSend(iface, sel_registerName("bssid"));
        if (CFbssid) {
            *bssid = CFString_to_C_string(CFbssid);
            CFRelease(CFbssid);
        }

        CFStringRef CFssid = (CFStringRef)objc_msgSend(iface, sel_registerName("ssid"));
        if (CFssid) {
            *ssid = CFString_to_C_string(CFssid);
            CFRelease(CFssid);
        }
    }

    CFRelease(iface);
}

int main(void) {
    CWWiFiClient = (id)objc_getClass("CWWiFiClient");
    id pool = (id)objc_getClass("NSAutoreleasePool");
    if (!pool)
    {
        fprintf(stderr, "Unable to get NSAutoreleasePool!\nAborting\n");
        return -1;
    }
    pool = objc_msgSend(pool, sel_registerName("alloc"));
    if (!pool)
    {
        fprintf(stderr, "Unable to create NSAutoreleasePool...\nAborting...\n");
        return -1;
    }
    pool = objc_msgSend(pool, sel_registerName("init"));

    char **wifi_ifaces = get_wifi_supported_interfaces();
    CFArrayRef network_interfaces = SCNetworkInterfaceCopyAll();
    CFIndex num_interfaces = CFArrayGetCount(network_interfaces);
    for (CFIndex i = 0; i < num_interfaces; i++) {
        SCNetworkInterfaceRef interface = (SCNetworkInterfaceRef) CFArrayGetValueAtIndex(network_interfaces, i);
        CFStringRef bsdname_CFString = SCNetworkInterfaceGetBSDName(interface);
        if (bsdname_CFString == NULL) {
            continue;
        }
        char *bsdname = CFString_to_C_string(bsdname_CFString);
        CFRelease(bsdname_CFString);

        CFStringRef friendly_name_CFString = SCNetworkInterfaceGetLocalizedDisplayName(interface);
        CFStringRef interface_type_CFString = SCNetworkInterfaceGetInterfaceType(interface);
        InterfaceType type = remap_interface_type(interface_type_CFString);
        if (interface_type_CFString) {
            CFRelease(interface_type_CFString);
        }
        char *friendly_name = CFString_to_C_string(friendly_name_CFString);
        if (friendly_name_CFString) {
            CFRelease(friendly_name_CFString);
        }
        bool is_up;
        {
            struct ifreq ifr;
            int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
            memset(&ifr, 0, sizeof(ifr));
            strcpy(ifr.ifr_name, bsdname);
            ioctl(sock, SIOCGIFFLAGS, &ifr);
            close(sock);
            is_up = !!(ifr.ifr_flags & IFF_UP);
        }
        char *bssid = NULL;
        char *ssid = NULL;
        bool is_active = false;

        if (type == IEEE80211 && is_in_array(bsdname, wifi_ifaces)) {
            get_wifi_infos(bsdname, &bssid, &ssid, &is_active);
        }

        // if (type == ETHERNET) {
        //     CFDictionaryRef conf = SCNetworkInterfaceGetConfiguration(interface);
        //     if (conf) {
        //         CFNumberRef if_power_on_ref = (CFNumberRef)CFDictionaryGetValue(conf, kSCPropNetAirPortPowerEnabled);
        //         CFNumberTypeRef t = CFNumberGetType(if_power_on_ref);
        //         int tmp = 0;
        //         CFNumberGetValue(if_power_on_ref, t, &tmp);
        //         CFRelease(if_power_on_ref);
        //         CFRelease(t);
        //         CFRelease(conf);
        //         is_active = !!tmp;
        //     }
        // }

        printf("bsdname: %s\nfriendly_name: %s\ntype: %d\nup: %s\n", bsdname, friendly_name, type, is_up ? "true" : "false");

        printf("bssid: %s\nssid: %s\n", bssid, ssid);

        printf("active: %s\n", is_active ? "true" : "false");

        printf("\n");

        if (bsdname) {
            free(bsdname);
        }
        if (friendly_name) {
            free(friendly_name);
        }
        if (bssid) {
            free(bssid);
        }
        if (ssid) {
            free(ssid);
        }
    }
    free_string_array(wifi_ifaces);
    return 0;
}
