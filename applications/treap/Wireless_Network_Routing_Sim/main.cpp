#include "NetworkTopology.h"

int main() {
    NetworkTopology mesh_network;

    // 1. Register basic dynamic node points
    mesh_network.registerRouter({"192.168.1.10", "Server Room Base"}, -55.0);
    mesh_network.registerRouter({"192.168.1.50", "Main Hallway Drone"}, -72.0);
    mesh_network.registerRouter({"192.168.1.25", "Lobby Access Point"}, -45.0);

    int choice = 0;
    while (true) {
        std::cout << "\n================= WIRELESS AD-HOC MESH SIMULATOR =================\n";
        mesh_network.printPrimaryGateway();
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "1. View Complete Local Routing Directory (Sorted by IP)\n";
        std::cout << "2. Update Router Signal / Simulate Node Movement\n";
        std::cout << "3. Route Packet to Target IP Address\n";
        std::cout << "4. Terminate Simulation\n";
        std::cout << "Select Operation: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) {
            std::cout << "\n--- Active Routing Directory ---\n";
            mesh_network.printRoutingTable();
        } 
        else if (choice == 2) {
            std::string ip, loc;
            double signal;
            std::cout << "Enter Router IP address to update/add: ";
            std::cin >> ip;
            std::cout << "Enter Location Label (e.g., Warehouse_B): ";
            std::cin >> loc;
            std::cout << "Enter current RSSI Signal Strength (-90.0 to -30.0 dBm): ";
            std::cin >> signal;

            mesh_network.registerRouter({ip, loc}, signal);
            std::cout << "Topology adjusted successfully!\n";
        } 
        else if (choice == 3) {
            std::string target_ip;
            std::cout << "Enter destination IP address: ";
            std::cin >> target_ip;

            Node* target = mesh_network.locateRouter(target_ip);
            if (target) {
                std::cout << "Success! Path resolved in O(log N) depth.\n";
                std::cout << "Packet safely transmitted to " << target->key.location 
                          << " via signal lane " << target->priority << " dBm.\n";
            } else {
                std::cout << "Error: Routing failure. IP address unreachable on this mesh.\n";
            }
        } 
        else if (choice == 4) {
            std::cout << "Shutting down simulator.\n";
            break;
        }
    }
    return 0;
}