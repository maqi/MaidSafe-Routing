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

#ifndef MAIDSAFE_ROUTING_TESTS_ROUTING_NETWORK_H_
#define MAIDSAFE_ROUTING_TESTS_ROUTING_NETWORK_H_

#include <chrono>
#include <future>
#include <string>
#include <vector>
#include <algorithm>

#include "boost/asio/ip/udp.hpp"
#include "boost/thread/future.hpp"

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/rudp/nat_type.h"

#include "maidsafe/routing/api_config.h"
#include "maidsafe/routing/node_info.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/return_codes.h"

namespace args = std::placeholders;

namespace maidsafe {

namespace routing {

class Routing;
namespace protobuf { class Message; }

namespace test {

struct NodeInfoAndPrivateKey;

#ifdef FAKE_RUDP
  const uint32_t kClientSize(8);
  const uint32_t kServerSize(8);
#else
  const uint32_t kClientSize(2);
  const uint32_t kServerSize(6);
#endif

const uint32_t kNetworkSize = kClientSize + kServerSize;

class GenericNetwork;

class GenericNode {
 public:
  explicit GenericNode(bool client_mode = false);
  GenericNode(bool client_mode, const rudp::NatType& nat_type);
  GenericNode(bool client_mode, const NodeInfoAndPrivateKey& node_info);
  virtual ~GenericNode();
  int GetStatus() const;
  NodeId node_id() const;
  size_t id() const;
  NodeId connection_id() const;
  boost::asio::ip::udp::endpoint endpoint() const;
  std::shared_ptr<Routing> routing() const;
  NodeInfo node_info() const;
  void set_joined(const bool node_joined);
  bool joined() const;
  bool IsClient() const;
  void set_client_mode(const bool& client_mode);
  int expected();
  void set_expected(const int& expected);
  int ZeroStateJoin(const boost::asio::ip::udp::endpoint& peer_endpoint,
                    const NodeInfo& peer_node_info);
  void Join(const std::vector<boost::asio::ip::udp::endpoint>& peer_endpoints =
                std::vector<boost::asio::ip::udp::endpoint>());
  void Send(const NodeId& destination_id,
            const NodeId& group_claim,
            const std::string& data,
            const ResponseFunctor& response_functor,
            const boost::posix_time::time_duration& timeout,
            bool direct,
            bool cache);
  void SendToClosestNode(const protobuf::Message& message);
  void RudpSend(const NodeId& peer_endpoint,
                const protobuf::Message& message,
                rudp::MessageSentFunctor message_sent_functor);
  void PrintRoutingTable();
  bool RoutingTableHasNode(const NodeId& node_id);
  bool NonRoutingTableHasNode(const NodeId& node_id);
  testing::AssertionResult DropNode(const NodeId& node_id);
  std::vector<NodeInfo> RoutingTable() const;
  std::vector<NodeId> RandomNodeVector();
  NodeId GetRandomExistingNode();
  void AddExistingRandomNode(const NodeId& node_id);

  static size_t next_node_id_;
  size_t MessagesSize() const;
  void ClearMessages();

  friend class GenericNetwork;
  Functors functors_;

 protected:
  size_t id_;
  std::shared_ptr<NodeInfoAndPrivateKey> node_info_plus_;
  std::shared_ptr<Routing> routing_;
  std::mutex mutex_;
  bool client_mode_;
  bool anonymous_;
  bool joined_;
  int expected_;
  rudp::NatType nat_type_;
  boost::asio::ip::udp::endpoint endpoint_;
  std::vector<std::string> messages_;
};

class GenericNetwork : public testing::Test {
 public:
  typedef std::shared_ptr<GenericNode> NodePtr;
  GenericNetwork();
  ~GenericNetwork();

  std::vector<NodePtr> nodes_;

 protected:
  virtual void SetUp();
  virtual void TearDown();
  virtual void SetUpNetwork(const size_t& non_client_size, const size_t& client_size = 0);
  void AddNode(const bool& client_mode, const NodeId& node_id, bool anonymous = false);
  void AddNode(const bool& client_mode, const rudp::NatType& nat_type);
  bool RemoveNode(const NodeId& node_id);
  virtual void Validate(const NodeId& node_id, GivePublicKeyFunctor give_public_key);
  virtual void SetNodeValidationFunctor(NodePtr node);
  void PrintRoutingTables();
  bool ValidateRoutingTables();

 private:
  uint16_t NonClientNodesSize() const;
  void AddNodeDetails(NodePtr node);

  std::vector<boost::asio::ip::udp::endpoint> bootstrap_endpoints_;
  fs::path bootstrap_path_;
  std::mutex mutex_;
};

}  // namespace test

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_TESTS_ROUTING_NETWORK_H_