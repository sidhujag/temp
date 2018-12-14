// Copyright (c) 2017-2018 The Syscoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ASSETALLOCATION_H
#define ASSETALLOCATION_H

#include "rpc/server.h"
#include "dbwrapper.h"
#include "primitives/transaction.h"
#include <services/ranges.h>
#include <unordered_map>
#include "services/graph.h"
class CTransaction;
class CReserveKey;
class CCoinsViewCache;
class CBlock;
class CAsset;

bool DecodeAssetAllocationTx(const CTransaction& tx, int& op, std::vector<std::vector<unsigned char> >& vvch);
bool DecodeAndParseAssetAllocationTx(const CTransaction& tx, int& op, std::vector<std::vector<unsigned char> >& vvch, char& type);
bool DecodeAssetAllocationScript(const CScript& script, int& op, std::vector<std::vector<unsigned char> > &vvch);
bool IsAssetAllocationOp(int op);
void AssetAllocationTxToJSON(const int op, const std::vector<unsigned char> &vchData, const std::vector<unsigned char> &vchHash, UniValue &entry);
std::string assetAllocationFromOp(int op);
bool RemoveAssetAllocationScriptPrefix(const CScript& scriptIn, CScript& scriptOut);
class CAssetAllocationTuple {
public:
	std::vector<unsigned char> vchAsset;
	std::string vchAddress;
	ADD_SERIALIZE_METHODS;

	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action) {
		READWRITE(vchAsset);
		READWRITE(vchAddress);
	}
	CAssetAllocationTuple(const std::vector<unsigned char> &asset, const std::string &vchAddress_) {
		vchAsset = asset;
		vchAddress = vchAddress_;
	}
	CAssetAllocationTuple(const std::vector<unsigned char> &asset) {
		vchAsset = asset;
		vchAddress.clear();
	}
	CAssetAllocationTuple() {
		SetNull();
	}
	inline CAssetAllocationTuple operator=(const CAssetAllocationTuple& other) {
		this->vchAsset = other.vchAsset;
		this->vchAddress = other.vchAddress;
		return *this;
	}
	inline bool operator==(const CAssetAllocationTuple& other) const {
		return this->vchAsset == other.vchAsset && this->vchAddress == other.vchAddress;
	}
	inline bool operator!=(const CAssetAllocationTuple& other) const {
		return (this->vchAsset != other.vchAsset || this->vchAddress != other.vchAddress);
	}
	inline bool operator< (const CAssetAllocationTuple& right) const
	{
		return ToString() < right.ToString();
	}
	inline void SetNull() {
		vchAsset.clear();
		vchAddress.clear();
	}
	std::string ToString() const;
	inline bool IsNull() {
		return (vchAsset.empty() && vchAddress.empty());
	}
};
typedef std::pair<std::string, std::vector<CRange> > InputRanges;
typedef std::vector<InputRanges> RangeInputArrayTuples;
typedef std::vector<std::pair<std::string, CAmount > > RangeAmountTuples;
typedef std::map<uint256, int64_t> ArrivalTimesMap;
typedef std::map<std::string, std::string> AssetAllocationIndexItem;
typedef std::map<int, AssetAllocationIndexItem> AssetAllocationIndexItemMap;
extern AssetAllocationIndexItemMap AssetAllocationIndex;
static const int ZDAG_MINIMUM_LATENCY_SECONDS = 10;
static const int MAX_MEMO_LENGTH = 128;
static const int ONE_YEAR_IN_BLOCKS = 525600;
static const int ONE_HOUR_IN_BLOCKS = 60;
static const int ONE_MONTH_IN_BLOCKS = 43800;
static sorted_vector<CAssetAllocationTuple> assetAllocationConflicts;
static CCriticalSection cs_assetallocation;
static CCriticalSection cs_assetallocationindex;
enum {
	ZDAG_NOT_FOUND = -1,
	ZDAG_STATUS_OK = 0,
	ZDAG_MINOR_CONFLICT,
	ZDAG_MAJOR_CONFLICT
};

class CAssetAllocation {
public:
	std::vector<unsigned char> vchAsset;
	std::string vchAddress;
	uint256 txHash;
	unsigned int nHeight;
	unsigned int nLastInterestClaimHeight;
	// if allocations are tracked by individual inputs
	std::vector<CRange> listAllocationInputs;
	RangeInputArrayTuples listSendingAllocationInputs;
	RangeAmountTuples listSendingAllocationAmounts;
	CAmount nBalance;
	double nAccumulatedBalanceSinceLastInterestClaim;
	float fAccumulatedInterestSinceLastInterestClaim;
	float fInterestRate;
	std::vector<unsigned char> vchMemo;
	std::vector<unsigned char> vchExtra;
	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action) {
		READWRITE(vchAsset);
		READWRITE(vchAddress);
		READWRITE(txHash);
		READWRITE(VARINT(nHeight));
		READWRITE(VARINT(nLastInterestClaimHeight));
		READWRITE(listAllocationInputs);
		READWRITE(listSendingAllocationInputs);
		READWRITE(listSendingAllocationAmounts);
		READWRITE(nBalance);
		READWRITE(nAccumulatedBalanceSinceLastInterestClaim);
		READWRITE(fAccumulatedInterestSinceLastInterestClaim);
		READWRITE(fInterestRate);
		READWRITE(vchMemo);
		READWRITE(vchExtra);
	}
	CAssetAllocation() {
		SetNull();
	}
	CAssetAllocation(const CTransaction &tx) {
		SetNull();
		UnserializeFromTx(tx);
	}
	inline void ClearAssetAllocation()
	{
		vchMemo.clear();
		listAllocationInputs.clear();
		listSendingAllocationInputs.clear();
		listSendingAllocationAmounts.clear();
		vchAddress.clear();
		vchAsset.clear();
	}
	ADD_SERIALIZE_METHODS;

	inline friend bool operator==(const CAssetAllocation &a, const CAssetAllocation &b) {
		return (a.vchAsset == b.vchAsset && a.vchAddress == b.vchAddress
			);
	}

	inline CAssetAllocation operator=(const CAssetAllocation &b) {
		vchAsset = b.vchAsset;
		vchAddress = b.vchAddress;
		txHash = b.txHash;
		nHeight = b.nHeight;
		nLastInterestClaimHeight = b.nLastInterestClaimHeight;
		listAllocationInputs = b.listAllocationInputs;
		listSendingAllocationInputs = b.listSendingAllocationInputs;
		listSendingAllocationAmounts = b.listSendingAllocationAmounts;
		nBalance = b.nBalance;
		nAccumulatedBalanceSinceLastInterestClaim = b.nAccumulatedBalanceSinceLastInterestClaim;
		fAccumulatedInterestSinceLastInterestClaim = b.fAccumulatedInterestSinceLastInterestClaim;
		vchMemo = b.vchMemo;
		fInterestRate = b.fInterestRate;
		return *this;
	}

	inline friend bool operator!=(const CAssetAllocation &a, const CAssetAllocation &b) {
		return !(a == b);
	}
	inline void SetNull() { vchExtra.clear(); fInterestRate = 0; fAccumulatedInterestSinceLastInterestClaim = 0; nAccumulatedBalanceSinceLastInterestClaim = 0; vchMemo.clear(); nLastInterestClaimHeight = 0; nBalance = 0; listSendingAllocationAmounts.clear();  listSendingAllocationInputs.clear(); listAllocationInputs.clear(); vchAsset.clear(); vchAddress.clear(); nHeight = 0; txHash.SetNull(); }
	inline bool IsNull() const { return (vchAsset.empty()); }
	bool UnserializeFromTx(const CTransaction &tx);
	bool UnserializeFromData(const std::vector<unsigned char> &vchData, const std::vector<unsigned char> &vchHash);
	void Serialize(std::vector<unsigned char>& vchData);
};


class CAssetAllocationDB : public CDBWrapper {
public:
	CAssetAllocationDB(size_t nCacheSize, bool fMemory, bool fWipe) : CDBWrapper(GetDataDir() / "assetallocations", nCacheSize, fMemory, fWipe, false, true) {}

    bool WriteAssetAllocation(const CAssetAllocation& assetallocation, const CAmount& nSenderBalance, const CAmount& nAmount, const CAsset& asset, const int64_t& arrivalTime, const std::string& strSender, const std::string& strReceiver, const bool& fJustCheck, const bool &bMiner=false) {
		const CAssetAllocationTuple allocationTuple(assetallocation.vchAsset, assetallocation.vchAddress);
		bool writeState = false;
		{
			writeState = Write(make_pair(std::string("assetallocationi"), allocationTuple), assetallocation);
			if (!fJustCheck){
                if(bMiner)
                    writeState = writeState && WriteLastAssetAllocationMiner(allocationTuple);
				writeState = writeState && WriteLastAssetAllocation(allocationTuple, assetallocation);
            }
			else  {
				if (arrivalTime < INT64_MAX) {
					ArrivalTimesMap arrivalTimes;
					ReadISArrivalTimes(allocationTuple, arrivalTimes);
					arrivalTimes.emplace(assetallocation.txHash, arrivalTime);
					writeState = writeState && Write(make_pair(std::string("assetallocationa"), allocationTuple), arrivalTimes);
				}
			}
		}
		if(writeState && !strReceiver.empty())
			WriteAssetAllocationIndex(assetallocation, asset, nSenderBalance, nAmount, strSender, strReceiver);
        return writeState;
    }
	bool EraseAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple, bool cleanup = false) {
		bool eraseState = Erase(make_pair(std::string("assetallocationi"), assetAllocationTuple));
		if (eraseState) {
			Erase(make_pair(std::string("assetp"), assetAllocationTuple));
			EraseISArrivalTimes(assetAllocationTuple);
		}
		return eraseState;
	}
    bool ReadAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple, CAssetAllocation& assetallocation) {
        return Read(make_pair(std::string("assetallocationi"), assetAllocationTuple), assetallocation);
    }
	bool ReadLastAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple, CAssetAllocation& assetallocation) {
		return Read(make_pair(std::string("assetallocationp"), assetAllocationTuple), assetallocation);
	}
    bool EraseLastAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple) {
        if(ExistsLastAssetAllocation(assetAllocationTuple))
            return Erase(make_pair(std::string("assetallocationp"), assetAllocationTuple));
        return true;
    }    
    bool ExistsLastAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple) {
        return Exists(make_pair(std::string("assetallocationp"), assetAllocationTuple));
    }      
    bool WriteLastAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple, const CAssetAllocation& assetallocation) {
        return Write(make_pair(std::string("assetallocationp"), assetAllocationTuple), assetallocation);
    }
    bool ReadLastAssetAllocationMiner(const CAssetAllocationTuple& assetAllocationTuple, CAssetAllocation& assetallocation) {
        return Read(make_pair(std::string("assetallocationpm"), assetAllocationTuple), assetallocation);
    }
    bool WriteLastAssetAllocationMiner(const CAssetAllocationTuple& assetAllocationTuple) {
        // the last allocation (last pow) must exist and the last miner state shouldn't exist (only save it the first time to avoid subsequent updates from overwriting this data)
        if(ExistsLastAssetAllocation(assetAllocationTuple) && !ExistsLastAssetAllocationMiner(assetAllocationTuple)){
            CAssetAllocation assetallocation;
            if(ReadLastAssetAllocation(assetAllocationTuple, assetallocation))
                return Write(make_pair(std::string("assetallocationpm"), assetAllocationTuple), assetallocation);
        }
        return true;
    }
    bool ExistsLastAssetAllocationMiner(const CAssetAllocationTuple& assetAllocationTuple) {
        return Exists(make_pair(std::string("assetallocationpm"), assetAllocationTuple));
    }
    bool EraseLastAssetAllocationMiner(const CAssetAllocationTuple& assetAllocationTuple) {
        if(ExistsLastAssetAllocationMiner(assetAllocationTuple))
            return Erase(make_pair(std::string("assetallocationpm"), assetAllocationTuple));
        return true;
    }    
    bool ReadISArrivalTimes(const CAssetAllocationTuple& assetAllocationTuple, ArrivalTimesMap& arrivalTimes) {
        return Read(make_pair(std::string("assetallocationa"), assetAllocationTuple), arrivalTimes);
    }    
	bool EraseISArrivalTime(const CAssetAllocationTuple& assetAllocationTuple, const uint256& txid) {
		ArrivalTimesMap arrivalTimes;
		ReadISArrivalTimes(assetAllocationTuple, arrivalTimes);
		ArrivalTimesMap::const_iterator it = arrivalTimes.find(txid);
		if (it != arrivalTimes.end())
			arrivalTimes.erase(it);
		if (arrivalTimes.size() > 0)
			return Write(make_pair(std::string("assetallocationa"), assetAllocationTuple), arrivalTimes);
		else
			return Erase(make_pair(std::string("assetallocationa"), assetAllocationTuple));
	}
	bool EraseISArrivalTimes(const CAssetAllocationTuple& assetAllocationTuple) {
		return Erase(make_pair(std::string("assetallocationa"), assetAllocationTuple));
	}
	void WriteAssetAllocationIndex(const CAssetAllocation& assetAllocationTuple, const CAsset& asset, const CAmount& nSenderBalance, const CAmount& nAmount, const std::string& strSender, const std::string& strReceiver);
	bool ScanAssetAllocations(const int count, const int from, const UniValue& oOptions, UniValue& oRes);
};
class CAssetAllocationTransactionsDB : public CDBWrapper {
public:
	CAssetAllocationTransactionsDB(size_t nCacheSize, bool fMemory, bool fWipe) : CDBWrapper(GetDataDir() / "assetallocationtransactions", nCacheSize, fMemory, fWipe, false, true) {
		ReadAssetAllocationWalletIndex(AssetAllocationIndex);
	}

	bool WriteAssetAllocationWalletIndex(const AssetAllocationIndexItemMap &valueMap) {
		return Write(std::string("assetallocationtxi"), valueMap, true);
	}
	bool ReadAssetAllocationWalletIndex(AssetAllocationIndexItemMap &valueMap) {
		return Read(std::string("assetallocationtxi"), valueMap);
	}
	bool ScanAssetAllocationIndex(const int count, const int from, const UniValue& oOptions, UniValue& oRes);
};
bool CheckAssetAllocationInputs(const CTransaction &tx, const CCoinsViewCache &inputs, int op, const std::vector<std::vector<unsigned char> > &vvchArgs, bool fJustCheck, int nHeight, sorted_vector<CAssetAllocationTuple> &revertedAssetAllocations, std::string &errorMessage, bool bSanityCheck = false, bool bMiner = false);
bool GetAssetAllocation(const CAssetAllocationTuple& assetAllocationTuple,CAssetAllocation& txPos);
bool BuildAssetAllocationJson(CAssetAllocation& assetallocation, const CAsset& asset, const bool bGetInputs, UniValue& oName);
bool BuildAssetAllocationIndexerJson(const CAssetAllocation& assetallocation, const CAsset& asset, const CAmount& nSenderBalance, const CAmount& nAmount, const std::string& strSender, const std::string& strReceiver, bool &isMine, UniValue& oAssetAllocation);
bool AccumulateInterestSinceLastClaim(CAssetAllocation & assetAllocation, const int& nHeight);
bool RevertAssetAllocationMiner(const std::vector<CTransactionRef>& blockVtx);
#endif // ASSETALLOCATION_H
