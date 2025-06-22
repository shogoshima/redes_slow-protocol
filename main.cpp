#include "slow_peripheral.h"
#include <iostream>
#include <string.h>

using namespace std;

int main() {
    SlowPeripheral p("slow.gmelodie.com", 7033);

    cout << "INICIANDO 3-WAY CONNECT…\n";
    if (!p.connectSlow()) {
        cerr << "FALHA AO CONECTAR\n";
        return 1;
    }

    const char *data = "";
    if (!p.sendDataSlow(reinterpret_cast<const uint8_t *>(data), strlen(data))) {
        cerr << "ERRO AO ENVIAR OS DADOS\n";
        return 1;
    }
    cout << "3-WAY CONNECT BEM SUCEDIDO!\n";

    cout << endl;
    cout << "ESCREVA A MENSAGEM A SER ENVIADA (Aperte ENTER para enviar):\n";
    cout << "(Caso queira terminar, digite 0)\n";
    string msg;
    while (getline(cin, msg)) {
        if (strcmp(msg.c_str(), "0") == 0) break;

        cout << "REALIZAR UMA 0-WAY CONNECTION? (1 para sim, 0 para não)\n";
        bool zwc = false;
        cin >> zwc;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // limpa o '\n' que sobra do cin >> zwc

        cout << "ENVIANDO DADOS…\n";
        if (!p.sendDataSlow(reinterpret_cast<const uint8_t *>(msg.c_str()), msg.length(), zwc)) {
            cerr << "ERRO AO ENVIAR OS DADOS\n";
        } else {
            cout << "DADOS ENVIADOS COM SUCESSO\n\n";

            cout << "ESCREVA A PRÓXIMA MENSAGEM A SER ENVIADA (APERTE ENTER PARA ENVIAR)\n";
            cout << "(Caso queira terminar, digite 0)\n";
        }
    }

    cout << endl;
    cout << "DESCONECTANDO…\n";
    if (!p.disconnectSlow()) {
        cerr << "ERRO AO DESCONECTAR DA CENTRAL\n";
        return 1;
    } else {
        cout << "DESCONECTADO COM SUCESSO\n";
    }

    cout << "FINALIZADO\n";
    return 0;
}
