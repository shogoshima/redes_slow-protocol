#include "slow_packet.h"
#include <arpa/inet.h> // htonl, htons, ...
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <iomanip>

std::vector<uint8_t> serialize(const SlowPacket &p) {
    constexpr size_t HEADER_SIZE = offsetof(SlowPacket, data); // = 32 bytes
    size_t total = HEADER_SIZE + p.data_len;
    std::vector<uint8_t> buf(total);

    // Começamos com o ponteiro de escrita
    uint8_t *dst = buf.data();

    // Copiando os dados da struct e criando o pacote
    memcpy(dst + offsetof(SlowPacket, sid), p.sid, 16);
    std::cout << "sid enviado: ";
    std::cout << std::hex << std::setfill('0');
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            std::cout << '-';
        std::cout << std::setw(2) << static_cast<int>(*(dst + i));
    }
    std::cout << std::dec << std::endl;

    memcpy(dst + offsetof(SlowPacket, sttl_and_flags), &p.sttl_and_flags, 4);
    memcpy(dst + offsetof(SlowPacket, seqnum), &p.seqnum, 4);
    memcpy(dst + offsetof(SlowPacket, acknum), &p.acknum, 4);
    memcpy(dst + offsetof(SlowPacket, window), &p.window, 2);
    buf[offsetof(SlowPacket, fid)] = p.fid;
    buf[offsetof(SlowPacket, fo)] = p.fo;
    memcpy(dst + HEADER_SIZE, p.data, p.data_len);

    return buf;
}

SlowPacket deserialize(const uint8_t *buf, size_t len) {
    SlowPacket p{};
    if (len < offsetof(SlowPacket, data)) {
        throw std::runtime_error("Pacote muito pequeno");
    }

    // SID
    memcpy(p.sid, buf + offsetof(SlowPacket, sid), 16);
    std::cout << "sid: ";
    std::cout << std::hex << std::setfill('0');
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            std::cout << '-';
        std::cout << std::setw(2) << static_cast<int>(p.sid[i]);
    }
    std::cout << std::dec << std::endl;

    // STTL + flags  (raw little endian)
    uint32_t saf_raw;
    memcpy(&saf_raw, buf + offsetof(SlowPacket, sttl_and_flags), 4);
    p.sttl_and_flags = saf_raw;
    uint32_t sttl = p.sttl_and_flags >> 5;
    uint8_t flags = p.sttl_and_flags & 0x1F;
    std::cout << "sttl: " << sttl << ", flags: 0x" << std::hex << (int)flags << std::dec << std::endl;

    // seqnum
    memcpy(&p.seqnum, buf + offsetof(SlowPacket, seqnum), 4);
    std::cout << "seqnum: " << p.seqnum << std::endl;

    // acknum
    memcpy(&p.acknum, buf + offsetof(SlowPacket, acknum), 4);
    std::cout << "acknum: " << p.acknum << std::endl;

    // window
    memcpy(&p.window, buf + offsetof(SlowPacket, window), 2);
    std::cout << "window: " << p.window << std::endl;

    // fid, fo
    p.fid = buf[offsetof(SlowPacket, fid)];
    p.fo = buf[offsetof(SlowPacket, fo)];
    std::cout << "fid: " << (int)p.fid << ", fo: " << (int)p.fo << std::endl;

    // payload
    p.data_len = len - offsetof(SlowPacket, data);
    if (p.data_len > SLOW_MAX_DATA) {
        throw std::runtime_error("Payload muito grande");
    }
    memcpy(p.data, buf + offsetof(SlowPacket, data), p.data_len);
    std::cout << "data_len: " << p.data_len << " bytes" << std::endl;

    return p;
}