#ifndef ROOMPROTOCOL_H
#define ROOMPROTOCOL_H

namespace RoomProtocol {
// Client requests
constexpr int RequestOnRoom = 1001;
constexpr int RequestBeReady = 1002;
constexpr int RequestCallLandlord = 1003;
constexpr int RequestDiscard = 1004;
constexpr int RequestCreateRoom = 1005;
constexpr int RequestJoinRoom = 1006;
constexpr int RequestReconnect = 1007;

// Server responses
constexpr int ResponseInitUserOk = 2000;
constexpr int ResponseRoomInfo = 2001;
constexpr int ResponseNewPlayerJoin = 20011;
constexpr int ResponseReady = 2002;
constexpr int ResponseDealPoker = 2003;
constexpr int ResponseToCallLandlord = 2005;
constexpr int ResponseLandlordAndLastCard = 2006;
constexpr int ResponseDiscard = 2007;
constexpr int ResponseGameOver = 2008;
constexpr int ResponseCreateRoomOk = 2009;
constexpr int ResponseReconnectOk = 2010;

constexpr const char *VictoryLandlord = "Landlord";
constexpr const char *VictoryFarmer = "Farmer";

constexpr const char *BidCall = "call";
constexpr const char *BidPass = "pass";
constexpr const char *BidGrab = "grab";
constexpr const char *BidSkip = "skip";
}

#endif // ROOMPROTOCOL_H
