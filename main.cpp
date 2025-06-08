#include "slow_peripheral.h"
#include <iostream>
#include <string.h>

int main() {
    SlowPeripheral p("slow.gmelodie.com", 7033);

    std::cout << "INICIANDO 3-WAY CONNECT…\n";
    if (!p.connectSlow()) {
        std::cerr << "FALHA AO CONECTAR\n";
        return 1;
    }

    const char *data = "";
    if (!p.sendDataSlow(reinterpret_cast<const uint8_t *>(data), strlen(data))) {
        std::cerr << "ERRO AO ENVIAR OS DADOS\n";
        return 1;
    } 
    std::cout << "3-WAY CONNECT BEM SUCEDIDO!\n";

    std::cout << "ENVIANDO DADOS…\n";
    const char *msg = "dados";
    if (!p.sendDataSlow(reinterpret_cast<const uint8_t *>(msg), strlen(msg), true)) {
        std::cerr << "ERRO AO ENVIAR OS DADOS\n";
        return 1;
    } else {
        std::cout << "DADOS ENVIADOS COM SUCESSO\n";
    }

    std::cout << "DESCONECTANDO…\n";
    if (!p.disconnectSlow()) {
        std::cerr << "ERRO AO DESCONECTAR DA CENTRAL\n";
        return 1;
    } else {
        std::cout << "DESCONECTADO COM SUCESSO\n";
    }

    std::cout << "FINALIZADO\n";
    return 0;
}
