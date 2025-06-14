#pragma once
#include <cstdint>
#include <vector>

// Tamanho máximo de payload
constexpr size_t SLOW_MAX_DATA = 1440;

#pragma pack(push,1)
struct SlowPacket {
    uint8_t  sid[16];    // UUIDv8 ou nil UUID
    // sttl (27 bits) + flags (5 bits)
    uint32_t sttl_and_flags;
    uint32_t seqnum;
    uint32_t acknum;
    uint16_t window;
    uint8_t  fid;
    uint8_t  fo;
    uint8_t  data[SLOW_MAX_DATA];

    // Auxiliar: tamanho real do campo data
    uint16_t data_len = 0;
};
#pragma pack(pop)

// Flags (bit positions dentro de `flags`)
enum SlowFlags : uint8_t {
    SLOW_FLAG_CONNECT = 1 << 4,   // C
    SLOW_FLAG_REVIVE  = 1 << 3,   // R
    SLOW_FLAG_ACK     = 1 << 2,   // A
    SLOW_FLAG_AR      = 1 << 1,   // Accept/Reject
    SLOW_FLAG_MB      = 1 << 0    // More Bits
};

// Função para Serialização
// Aceita um pacote SlowPacket, e retorna um vetor pronto para ser enviado
std::vector<uint8_t> serialize(const SlowPacket& p);

// Função para Desselialização
// Aceita um vector (buffer) e tamanho do buffer, e converte para SlowPacket
SlowPacket         deserialize(const uint8_t* buf, size_t len);
