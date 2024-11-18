#include <iostream>
#include <sstream>
#include <cstdio>   // Para usar popen y pclose
#include <cstdlib>  // Para usar system
#include <string>
#include <ctime>    // Para obtener el tiempo actual
#include <thread>   // Para usar std::this_thread::sleep_for
#include <chrono>   // Para usar std::chrono::seconds

// F
std::string getMacAddress(const std::string& interface) {
    std::ostringstream command;
    command << "cat /sys/class/net/" << interface << "/address";
    char mac[18];
    FILE* fp = popen(command.str().c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "No se pudo obtener la dirección MAC." << std::endl;
        return "";
    }
    if (fgets(mac, sizeof(mac), fp) == nullptr) {
        std::cerr << "No se pudo leer la dirección MAC." << std::endl;
        pclose(fp);
        return "";
    }
    pclose(fp);

    // Eliminar el carácter de nueva línea al final de la dirección MAC
    std::string macAddress(mac);
    macAddress.erase(macAddress.find_last_not_of("\n") + 1);
    return macAddress;
}

// F
std::string getIPv4Address(const std::string& interface) {
    std::ostringstream command;
    command << "ip addr show " << interface << " | grep 'inet ' | awk '{print $2}'";
    char ip[20];
    FILE* fp = popen(command.str().c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "No se pudo obtener la dirección IPv4." << std::endl;
        return "";
    }
    if (fgets(ip, sizeof(ip), fp) == nullptr) {
        std::cerr << "No se pudo leer la dirección IPv4." << std::endl;
        pclose(fp);
        return "";
    }
    pclose(fp);

    // Eliminar el carácter de nueva línea al final de la dirección IP
    std::string ipAddress(ip);
    ipAddress.erase(ipAddress.find_last_not_of("\n") + 1);
    return ipAddress;
}

// F
void changeMacAddress(const std::string& interface, const std::string& newMac) {
    std::cout << "[INFO] Cambiando la dirección MAC de " << interface << " a " << newMac << std::endl;

    // Usar ostringstream para construir los comandos
    std::ostringstream commandDown, commandChangeMac, commandUp;
    commandDown << "ifconfig " << interface << " down";
    commandChangeMac << "ifconfig " << interface << " hw ether " << newMac;
    commandUp << "ifconfig " << interface << " up";

    // Ejecutar cada comando y verificar el estado de retorno
    if (system(commandDown.str().c_str()) != 0) {
        std::cerr << "[ERROR] No se pudo desactivar la interfaz " << interface << std::endl;
    }
    if (system(commandChangeMac.str().c_str()) != 0) {
        std::cerr << "[ERROR] No se pudo cambiar la dirección MAC." << std::endl;
    }
    if (system(commandUp.str().c_str()) != 0) {
        std::cerr << "[ERROR] No se pudo activar la interfaz " << interface << std::endl;
    }
}

// F
void changeIPv4Address(const std::string& interface, const std::string& newIP) {
    std::cout << "[INFO] Cambiando la dirección IPv4 de " << interface << " a " << newIP << std::endl;

    std::ostringstream command;
    command << "ifconfig " << interface << " " << newIP << " netmask 255.255.255.0";

    // Ejecutar el comando y verificar el estado de retorno
    if (system(command.str().c_str()) != 0) {
        std::cerr << "[ERROR] No se pudo cambiar la dirección IPv4." << std::endl;
    }
}

int main() {
    std::string interface = "wlp2s0"; // Cambia a la interfaz deseada
    std::string newMac = "00:11:22:33:44:55"; // Cambia a la MAC deseada
    std::string newIP = "192.168.1.100"; // Cambia a la IP deseada

    // Obtener direcciones actuales
    std::string originalMac = getMacAddress(interface);
    std::string originalIP = getIPv4Address(interface);

    // Mostrar direcciones actuales
    std::cout << "Dirección MAC actual: " << originalMac << std::endl;
    std::cout << "Dirección IPv4 actual: " << originalIP << std::endl;

    // Cambiar las direcciones
    changeMacAddress(interface, newMac);
    changeIPv4Address(interface, newIP);

    // Mostrar las nuevas direcciones
    std::cout << "Nueva dirección MAC: " << getMacAddress(interface) << std::endl;
    std::cout << "Nueva dirección IPv4: " << getIPv4Address(interface) << std::endl;

    // Contador para 2 minutos
    time_t startTime = time(nullptr); // Obtener el tiempo actual
    time_t elapsedTime = 0;

    while (elapsedTime < 120) { // 120 segundos = 2 minutos
        elapsedTime = time(nullptr) - startTime;
        std::cout << "[INFO] Tiempo transcurrido: " << elapsedTime << " segundos." << std::endl;
        
        // Esperar 1 segundo para no sobrecargar la salida
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Restaurar las direcciones originales después de 2 minutos
    std::cout << "[INFO] Restaurando las direcciones originales..." << std::endl;
    changeMacAddress(interface, originalMac);
    changeIPv4Address(interface, originalIP);

    // Mostrar las direcciones restauradas
    std::cout << "Dirección MAC restaurada: " << getMacAddress(interface) << std::endl;
    std::cout << "Dirección IPv4 restaurada: " << getIPv4Address(interface) << std::endl;

    return 0;
}