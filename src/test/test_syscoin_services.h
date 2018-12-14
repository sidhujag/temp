// Copyright (c) 2016-2018 The Syscoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SYSCOIN_TEST_TEST_SYSCOIN_SERVICES_H
#define SYSCOIN_TEST_TEST_SYSCOIN_SERVICES_H

#include <stdio.h>
#include <univalue.h>
#include <map>
using namespace std;
/** Testing syscoin services setup that configures a complete environment with 3 nodes.
 */
UniValue CallRPC(const string &dataDir, const string& commandWithArgs, bool regTest = true, bool readJson = true);
UniValue CallExtRPC(const string &node, const string& command, const string& args="", bool readJson = true);
void StartNode(const string &dataDir, bool regTest = true, const string& extraArgs="");
void StopNode(const string &dataDir="node1");
void StartNodes();
void StartMainNetNodes();
void StopMainNetNodes();
void StopNodes();
void GenerateBlocks(int nBlocks, const string& node="node1");
void GenerateSpendableCoins();
string GetNewFundedAddress(const string &node);
void GenerateMainNetBlocks(int nBlocks, const string& node);
string CallExternal(string &cmd);
void SetSysMocktime(const int64_t& expiryTime);
void SleepFor(const int& seconds, bool actualSleep=false);
void CheckRangeSubtract(const string& originalRanges, const string& subtractRanges, const string& expectedOutputRanges);
void CheckRangeMerge(const string& originalRanges, const string& newRanges, const string& expectedOutputRanges);
bool DoesRangeContain(const string& parentRange, const string& childRange);
void GetOtherNodes(const string& node, string& otherNode1, string& otherNode2);
string AssetNew(const string& node, const string& name, const string& address, const string& pubdata = "''", const string& contract="''", const string& precision="8", const string& useinputranges="false", const string& supply = "1", const string& maxsupply = "10", const string& interestrate = "0", const string& updateflags = "63", const string& witness = "''");
void AssetUpdate(const string& node, const string& name, const string& pubdata = "''", const string& supply = "''", const string& interest = "''", const string& blacklist = "[]", const string& updateflags = "63", const string& witness = "''");
void AssetTransfer(const string& node, const string &tonode, const string& name, const string& toaddress, const string& witness = "''");
void BurnAssetAllocation(const string& node, const string &guid, const string &address,const string &amount, bool confirm=true);
string AssetSend(const string& node, const string& name, const string& inputs, const string& memo = "''", const string& witness = "''", bool completetx=true);
string AssetAllocationTransfer(const bool usezdag, const string& node, const string& name, const string& fromaddress, const string& inputs, const string& memo = "''", const string& witness = "''");
void AssetClaimInterest(const string& node, const string& name, const string& address, const string& witness = "''");

// SYSCOIN testing setup
struct SyscoinTestingSetup {
    SyscoinTestingSetup();
    ~SyscoinTestingSetup();
};
struct BasicSyscoinTestingSetup {
    BasicSyscoinTestingSetup();
    ~BasicSyscoinTestingSetup();
};
struct SyscoinMainNetSetup {
	SyscoinMainNetSetup();
	~SyscoinMainNetSetup();
};
#endif
