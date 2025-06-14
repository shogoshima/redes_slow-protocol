#pragma once
#include "slow_packet.h"
#include <chrono>
#include <cstdint>
#include <functional>
#include <netinet/in.h> // sockaddr_in
#include <string>

enum class SlowState {
    INACTIVE,
    CONNECTING,
    CONNECTED,
    WAIT_ACK,
    DISCONNECTING
};

class SlowPeripheral {
public:
    SlowPeripheral(const char *central_ip, uint16_t central_port = 7033);
    ~SlowPeripheral();

    // Função para conectar na central
    bool connectSlow();

    // Função para enviar dados para a central
    bool sendDataSlow(const uint8_t *buf, size_t len, bool revive = false);

    // Função para desconectar da central
    bool disconnectSlow();

private:
    int sockfd_;
    sockaddr_in central_addr_;
    socklen_t central_addr_len_;

    SlowState state_ = SlowState::INACTIVE; // A conexão se inicia inativo

    uint8_t sid_[16];     // Session ID atual
    uint32_t sttl_ = 0;   // Vou precisar deslocar 5 bits na hora de utilizar
    uint32_t seqnum_ = 0; // Seqnum da sessão
    uint16_t window_ = 0; // Window restante da central

    // Timeouts, reenvio
    std::chrono::milliseconds timeout_{500};
    bool waitForPacket(SlowPacket &out);

    // Helpers
    bool sendPacket(const SlowPacket &pkt);
    bool receivePacket(SlowPacket &pkt);
    void makeConnectPacket(SlowPacket &pkt);
    void makeDataPacket(SlowPacket &p,
                        const uint8_t *buf, size_t len,
                        bool revive = false,
                        uint8_t fid = 0,
                        uint8_t fo = 0,
                        bool moreBits = false);
    void makeDisconnectPacket(SlowPacket &pkt);
};
