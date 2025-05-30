#include "slow_peripheral.h"
#include <iostream>
#include <string.h>

int main() {
    SlowPeripheral p("slow.gmelodie.com", 7033);

    std::cout << "Conectando ao central…\n";
    if (!p.connectSlow()) {
        std::cerr << "Falha ao conectar\n";
        return 1;
    }
    std::cout << "Conectado!\n";

    const char *msg = "Hello Central!";
    if (!p.sendDataSlow(reinterpret_cast<const uint8_t *>(msg), strlen(msg))) {
        std::cerr << "Erro ao enviar dados\n";
        return 1;
    } else {
        std::cout << "Dados enviados com sucesso\n";
    }

    std::cout << "Desconectando…\n";
    if (!p.disconnectSlow()) {
        std::cerr << "Erro ao desconectar da central\n";
        return 1;
    } else {
        std::cout << "Desconectado com sucesso\n";
    }

    std::cout << "Finalizado\n";
    return 0;
}
