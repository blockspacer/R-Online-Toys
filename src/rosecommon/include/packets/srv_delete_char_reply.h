#pragma once


#include "packetfactory.h"
#include <string>

namespace RoseCommon {
namespace Packet {

class SrvDeleteCharReply : public CRosePacket {
    public:
        SrvDeleteCharReply();
        SrvDeleteCharReply(CRoseReader reader);
        SrvDeleteCharReply(SrvDeleteCharReply&&) = default;
        SrvDeleteCharReply& operator=(SrvDeleteCharReply&&) = default;
        ~SrvDeleteCharReply() = default;
        
        static constexpr size_t size();
        
        
        
        void set_remainingTime(const uint32_t);
        uint32_t get_remainingTime() const;
        void set_name(const std::string&);
        const std::string& get_name() const;
        
        
        static SrvDeleteCharReply create(const uint32_t&, const std::string&);
        static SrvDeleteCharReply create(const uint8_t*);
    
    protected:
        virtual void pack(CRoseBasePolicy&) const override;
    
    private:
        uint32_t remainingTime;
        std::string name;
};

}
}
