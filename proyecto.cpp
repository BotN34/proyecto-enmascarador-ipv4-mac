#include <ctime>
#include <sstream>
#include <unistd.h> // Para usar sleep()

// F
std::string getMacAddress(const std::string& interface) {
    std::string command = "cat /sys/class/net/" + interface + "/address";
    char mac[18];
    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "No se pudo obtener la dirección MAC." << std::endl;
        return "";
    }
    fgets(mac, sizeof(mac), fp);
    pclose(fp);
    return std::string(mac);
}

// F
std::string getIPv4Address(const std::string& interface) {
    std::string command = "ip addr show " + interface + " | grep 'inet ' | awk '{print $2}'";
    char ip[20];
    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "No se pudo obtener la dirección IPv4." << std::endl;
        return "";
    }
    fgets(ip, sizeof(ip), fp);
    pclose(fp);
    return std::string(ip);
}

// F
void changeMacAddress(const std::string& interface, const std::string& newMac) {
    std::cout << "[INFO] Cambiando la dirección MAC de " << interface << " a " << newMac << std::endl;
    std::string commandDown = "sudo ifconfig " + interface + " down";
    std::string commandChangeMac = "sudo ifconfig " + interface + " hw ether " + newMac;
    std::string commandUp = "sudo ifconfig " + interface + " up";

    system(commandDown.c_str());
    system(commandChangeMac.c_str());
    system(commandUp.c_str());
}

// F
void changeIPv4Address(const std::string& interface, const std::string& newIP) {
    std::cout << "[INFO] Cambiando la dirección IPv4 de " << interface << " a " << newIP << std::endl;
    std::string command = "sudo ifconfig " + interface + " " + newIP + " netmask 255.255.255.0";
    system(command.c_str());
}

// F
void resetMacAddress(const std::string& interface) {
    std::cout << "[INFO] Restaurando la dirección MAC original de " << interface << std::endl;
    std::string commandDown = "sudo ifconfig " + interface + " down";
    std::string commandUp = "sudo ifconfig " + interface + " up";

    system(commandDown.c_str());
    system(commandUp.c_str());
}

// F
void resetIPv4Address(const std::string& interface, const std::string& originalIP) {
    std::cout << "[INFO] Restaurando la dirección IPv4 original de " << interface << std::endl;
    std::string command = "sudo ifconfig " + interface + " " + originalIP;
    system(command.c_str());
}



// F
std::string generateLogFileName() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "packet_log_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
    return oss.str();
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE]; //       
    pcap_if_t *allDevs, *device; 
    std::string interface = "wlp2s0";  
    std::string newMac = "00:11:22:33:44:55";  // Dirección MAC falsa
    std::string newIP = "192.168.1.100";  // Dirección IP enmascarada falsa

    // O
    std::string originalMac = getMacAddress(interface);
    std::string originalIP = getIPv4Address(interface);
    if (originalMac.empty() || originalIP.empty()) {
        std::cerr << "Error al obtener la MAC o IP original." << std::endl;
        return 1;
    }

    // C
    changeMacAddress(interface, newMac);
    changeIPv4Address(interface, newIP);

    // E
    std::cout << "[INFO] Manteniendo la nueva dirección IPv4 durante 2 minutos.." << std::endl;
    sleep(40); // funcion para detener programa despues de| 2 minutos 

    // R
    resetIPv4Address(interface, originalIP);

    // R
    resetMacAddress(interface);

    //excpecion si no se encuentran dispositivos en la red
    if (pcap_findalldevs(&allDevs, errbuf) == -1) {
        std::cerr << "Error al encontrar dispositivos: " << errbuf << std::endl;
        return 1;
    }

    device = allDevs;
    if (!device) {
        std::cerr << "No se encontraron dispositivos." << std::endl;
        return 1;
    }

    //sino pcap no puede acceder a la direccion IP de los otros dispositivos
    
    pcap_t *handle = pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "No se pudo abrir el dispositivo: " << errbuf << std::endl;
        return 1;
    }

    // C
    std::string logFileName = generateLogFileName();
    std::ofstream logFile(logFileName, std::ios::out);
    if (!logFile.is_open()) {
        std::cerr << "Error al abrir el archivo de registro." << std::endl;
        return 1;
    }

    // E
    logFile << "[INFO] Dirección MAC original: " << originalMac << std::endl;
    logFile << "[INFO] Dirección MAC enmascarada: " << newMac << std::endl;
    logFile << "[INFO] Dirección IPv4 original: " << originalIP << std::endl;
    logFile << "[INFO] Dirección IPv4 enmascarada: " << newIP << std::endl << std::endl;

    // L
    pcap_close(handle);
    pcap_freealldevs(allDevs);
    logFile.close();

    return 0;
}