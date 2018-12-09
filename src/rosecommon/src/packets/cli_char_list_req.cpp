#include "cli_char_list_req.h"

using namespace RoseCommon;
using namespace RoseCommon::Packet;



CliCharListReq::CliCharListReq() : CRosePacket(ePacketType::PAKCS_CHAR_LIST_REQ) {}

CliCharListReq::CliCharListReq(CRoseReader reader) : CRosePacket(reader) {
}

CliCharListReq CliCharListReq::create() {
    CliCharListReq packet;
    return packet;
}

CliCharListReq CliCharListReq::create(const uint8_t* buffer) {
    CRoseReader reader(buffer, CRosePacket::size(buffer));
    return CliCharListReq(reader);
}

std::unique_ptr<CliCharListReq> CliCharListReq::allocate(const uint8_t* buffer) {
    CRoseReader reader(buffer, CRosePacket::size(buffer));
    return std::make_unique<CliCharListReq>(reader);
}

void CliCharListReq::pack(CRoseBasePolicy&) const {
}

constexpr size_t CliCharListReq::size() {
    size_t size = 0;
    return size;
}

