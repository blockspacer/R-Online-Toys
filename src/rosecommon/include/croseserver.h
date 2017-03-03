// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _CROSE_NETWORK_H_
#define _CROSE_NETWORK_H_

#include <list>
#include <forward_list>
#include "inetwork.h"
#include "croseclient.h"
#include "croseisc.h"

namespace RoseCommon
{
  class CRoseServer {
  public:
    CRoseServer(bool _iscServer = false);
    virtual ~CRoseServer();

    bool IsISCServer() { return isc_server_; }

    static std::forward_list<CRoseClient*>& GetClientList() {
      return client_list_;
    }

    static std::forward_list<CRoseClient*>& GetISCList() { return isc_list_; }
    static std::mutex& GetClientListMutex() { return client_list_mutex_; }
    static std::mutex& GetISCListMutex() { return isc_list_mutex_; }

    enum class eSendType : uint8_t {
      EVERYONE,
      EVERYONE_BUT_ME,
      NEARBY,
      NEARBY_BUT_ME,
    };

    static void SendPacket(const CRoseClient* sender, eSendType type, CRosePacket& _buffer);

    std::shared_ptr<spdlog::logger> logger_;
  protected:
    // Callback functions
    virtual bool OnConnect();
    virtual void OnConnected();
    virtual bool OnListen();
    virtual void OnListening();
    virtual bool OnDisconnect();
    virtual void OnDisconnected();
    virtual bool OnAccept();
    virtual void OnAccepted(Core::INetwork* _sock);

    bool isc_server_;
    static std::forward_list<CRoseClient*> client_list_;
    static std::forward_list<CRoseClient*> isc_list_;
    static std::mutex client_list_mutex_;
    static std::mutex isc_list_mutex_;

    Core::INetwork* socket_;
  };
}

#endif
