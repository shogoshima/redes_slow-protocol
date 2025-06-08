#include "slow_peripheral.h"
#include "slow_packet.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

SlowPeripheral::SlowPeripheral(const char *host, uint16_t port) {
    // Criando o socket UDP
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        perror("socket");
        return;
    }

    // Pegar o host pelo nome do host ("slow.gmelodie.com")
    hostent *server = gethostbyname(host);
    if (server == nullptr) {
        std::cerr << "Erro ao resolver o host: " << host << std::endl;
        close(sockfd_);
        return;
    }

    // Configurar o endereço do servidor
    memset(&central_addr_, 0, sizeof(central_addr_));
    central_addr_.sin_family = AF_INET;
    central_addr_.sin_port = htons(port);
    memcpy(&central_addr_.sin_addr, server->h_addr, server->h_length);

    central_addr_len_ = sizeof(central_addr_);

    state_ = SlowState::INACTIVE;
}

SlowPeripheral::~SlowPeripheral() {
    if (sockfd_ >= 0) close(sockfd_);
}

bool SlowPeripheral::sendPacket(const SlowPacket &pkt) {
    auto buf = serialize(pkt);
    if (window_ < pkt.data_len) {
        std::cout << "Sem janela suficiente" << std::endl;
        return false;
    }
    window_ -= pkt.data_len;

    std::cout << "Enviando pacote (" << buf.size() << " bytes) via UDP..." << std::endl;
    ssize_t sent = sendto(sockfd_, buf.data(), buf.size(), 0,
                          reinterpret_cast<sockaddr *>(&central_addr_), central_addr_len_);
    std::cout << "Bytes enviados: " << sent << std::endl;

    return (sent == static_cast<ssize_t>(buf.size()));
}

bool SlowPeripheral::hasSid() const {
    for (uint8_t b : sid_) {
        if (b != 0) return true; // encontrou um byte não-zero → sid válido
    }
    return false; // todos os bytes são zero → sid ainda nil
}

bool SlowPeripheral::receivePacket(SlowPacket &pkt) {
    uint8_t buffer[1500];
    ssize_t n = recvfrom(sockfd_, buffer, sizeof(buffer), 0,
                         nullptr, nullptr);
    if (n <= 0) return false;
    std::cout << "Bytes recebidos: " << n << std::endl;

    // verificar se sid é um uuid-v8 válido?

    // agora desserializa e sai do loop
    pkt = deserialize(buffer, n);
    return true;
}

bool SlowPeripheral::waitForPacket(SlowPacket &out) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd_, &readfds);

    struct timeval tv;
    auto ms = timeout_.count();
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;

    int ret = select(sockfd_ + 1, &readfds, nullptr, nullptr, &tv);
    if (ret < 0) {
        perror("select");
        return false;
    }
    if (ret == 0) {
        std::cerr << "[Timeout] Nenhum pacote recebido no timeout." << std::endl;
        return false;
    }

    return receivePacket(out);
}

// Função para conectar no Central
bool SlowPeripheral::connectSlow() {
    state_ = SlowState::CONNECTING;

    // Criando o pacote de Connect
    SlowPacket pkt;
    makeConnectPacket(pkt);

    // Mandando o pacote, e esperando o Setup
    if (!sendPacket(pkt)) return false;
    while (!waitForPacket(pkt)) {
        std::cout << "Tentando novamente..." << window_ << std::endl;
        if (!sendPacket(pkt)) {
            return false;
        }
    }

    uint8_t flags = pkt.sttl_and_flags & 0x1F; // Extrai os 5 bits de flags
    if (flags & SLOW_FLAG_AR) {
        std::cout << "ACCEPT recebido!" << std::endl;
        memcpy(sid_, pkt.sid, 16);
        sttl_ = pkt.sttl_and_flags >> 5;
        seqnum_ = pkt.seqnum;
        window_ = pkt.window;
        state_ = SlowState::CONNECTED;
        return true;
    }

    state_ = SlowState::INACTIVE;
    return false;
}

bool SlowPeripheral::sendDataSlow(const uint8_t *buf, size_t len, bool revive) {
    // Criando o pacote de dados
    SlowPacket pkt;
    makeDataPacket(pkt, buf, len, revive);

    // Mandando o pacote, e esperando o Ack
    if (!sendPacket(pkt)) return false;
    while (!waitForPacket(pkt)) {
        std::cout << "Tentando novamente..." << window_ << std::endl;
        if (!sendPacket(pkt)) {
            return false;
        }
    }

    uint8_t flags = pkt.sttl_and_flags & 0x1F; // Extrai os 5 bits de flags
    if (flags & SLOW_FLAG_ACK) {
        std::cout << "ACK recebido!" << std::endl;
        sttl_ = pkt.sttl_and_flags >> 5;
        seqnum_ = pkt.seqnum;
        window_ = pkt.window;
        return true;
    }

    return false;
}

bool SlowPeripheral::disconnectSlow() {
    SlowPacket pkt;
    makeDisconnectPacket(pkt);

    // Mandando o pacote, e esperando o Ack
    if (!sendPacket(pkt)) return false;
    while (!waitForPacket(pkt)) {
        std::cout << "Tentando novamente..." << window_ << std::endl;
        if (!sendPacket(pkt)) {
            return false;
        }
    }

    uint8_t flags = pkt.sttl_and_flags & 0x1F; // Extrai os 5 bits de flags
    if (flags & SLOW_FLAG_ACK) {
        sttl_ = pkt.sttl_and_flags >> 5;
        seqnum_ = pkt.seqnum;
        window_ = pkt.window;
        state_ = SlowState::INACTIVE;
        return true;
    }

    return false;
}

void SlowPeripheral::makeConnectPacket(SlowPacket &p) {
    memset(&p, 0, sizeof(p));

    // sid: Nil UUID = todos os 16 bytes em zero
    std::fill(std::begin(p.sid), std::end(p.sid), 0x00);

    // sttl: 0, flags: CONNECT (bit 4 = 1 << 4)
    uint32_t sttl = 0;
    uint8_t flags = SLOW_FLAG_CONNECT; // = 1 << 4
    p.sttl_and_flags = (sttl << 5) | (flags & 0x1F);

    // seqnum, acknum: 0
    p.seqnum = 0;
    p.acknum = 0;

    // window: por exemplo, buffer livre de 10.000 bytes
    p.window = 10000;

    // fid e fo: 0
    p.fid = 0;
    p.fo = 0;

    // data: inexistente
    p.data_len = 0;
}

void SlowPeripheral::makeDataPacket(SlowPacket &p,
                                    const uint8_t *buf, size_t len,
                                    bool revive,
                                    uint8_t fid,
                                    uint8_t fo,
                                    bool moreBits) {
    memset(&p, 0, sizeof(p));
    memcpy(p.sid, sid_, 16);

    // flags + sttl
    uint8_t flags = 0;
    if (moreBits) flags |= SLOW_FLAG_MB;
    if (revive) flags |= SLOW_FLAG_REVIVE;
    p.sttl_and_flags = (sttl_ << 5) | (flags & 0x1F);

    p.seqnum = seqnum_ + len; // número atual
    p.acknum = seqnum_;       // último ack recebido

    p.window = window_; // janela corrente
    p.fid = fid;        // Fragment ID
    p.fo = fo;          // Fragment Offset

    p.data_len = len;
    memcpy(p.data, buf, len);
}

void SlowPeripheral::makeDisconnectPacket(SlowPacket &p) {
    memset(&p, 0, sizeof(p));

    // sid: Pegar o salvo
    memcpy(p.sid, sid_, 16);

    // sttl: 0, flags: ACK, CONNECT, REVIVE
    uint32_t sttl = sttl_;
    uint8_t flags = 0;
    flags |= SLOW_FLAG_ACK;
    flags |= SLOW_FLAG_CONNECT;
    flags |= SLOW_FLAG_REVIVE;
    p.sttl_and_flags = (sttl << 5) | (flags & 0x1F);

    // seqnum, acknum: próximos números
    p.seqnum = seqnum_;
    p.acknum = seqnum_;

    // window: por exemplo, buffer livre de 10.000 bytes
    p.window = 0;

    // fid e fo: 0
    p.fid = 0;
    p.fo = 0;

    // data: inexistente
    p.data_len = 0;
}