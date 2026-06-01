#include "tree.h"

int main() {
    OSPFRouteCache router;
    
    router.addStaticRoute("192.168.1.1", "eth0_Local");
    router.addStaticRoute("8.8.8.8", "eth1_Gateway_WAN");
    router.addStaticRoute("10.0.0.5", "eth2_Internal_DMZ");

    std::vector<std::string> live_traffic_stream = {
        "8.8.8.8", "8.8.8.8", "10.0.0.5", "8.8.8.8"
    };

    std::cout << "Processing active OSPF flow channels:\n";
    for (const auto& ip : live_traffic_stream) {
        std::string interface_out = router.route_packet(ip);
        std::cout << "Destination Framework: " << ip << " forwarded to pipe -> " << interface_out << "\n";
    }
    return 0;
}