/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_ROUTING_MESSAGE_HANDLER_H_
#define MAIDSAFE_ROUTING_MESSAGE_HANDLER_H_

#include <string>

#include "maidsafe/rudp/managed_connections.h"
#include "maidsafe/routing/cache_manager.h"
#include "maidsafe/routing/routing_api.h"

namespace maidsafe {

namespace routing {

class RoutingTable;
class Timer;
class CacheManager;

namespace protobuf {
class Message;
}  // namespace protobuf

class MessageHandler {
 public:
  MessageHandler(RoutingTable &routing_table,
                 rudp::ManagedConnections &rudp,
                 Timer &timer_ptr,
                 NodeValidationFunctor node_validation_functor);
  void ProcessMessage(protobuf::Message &message);
  void DirectMessage(protobuf::Message &message);
  void RoutingMessage(protobuf::Message &message);
  void CloseNodesMessage(protobuf::Message &message);
  bool CheckCacheData(protobuf::Message &message);
  bool CheckAndSendToLocalClients(protobuf::Message &message);
  void Send(protobuf::Message &message);
  boost::signals2::signal<void(int, std::string)> &MessageReceivedSignal();

 private:
  MessageHandler(const MessageHandler&);  // no copy
  MessageHandler(const MessageHandler&&);  // no move
  MessageHandler& operator=(const MessageHandler&);  // no assign
  RoutingTable &routing_table_;
  rudp::ManagedConnections &rudp_;
  Timer &timer_ptr_;
  CacheManager cache_manager_;
  boost::signals2::signal<void(int, std::string)> message_received_signal_;
  NodeValidationFunctor node_validation_functor_;
};

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_MESSAGE_HANDLER_H_
