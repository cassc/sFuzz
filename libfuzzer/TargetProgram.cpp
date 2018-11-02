#include <libdevcore/CommonIO.h>
#include <libdevcore/SHA3.h>
#include <libethashseal/Ethash.h>
#include <libethashseal/GenesisInfo.h>
#include <libethcore/SealEngine.h>
#include <libethereum/Block.h>
#include <libethereum/ChainParams.h>
#include <libethereum/Executive.h>
#include <libethereum/LastBlockHashesFace.h>
#include <libevm/VMFactory.h>
#include "TargetProgram.h"
#include "Util.h"

using namespace dev;
using namespace eth;


namespace fuzzer {
  TargetProgram::TargetProgram(): state(State(0)), sender(Address(69)), contractAddress(Address(100)) {
    Network networkName = Network::MainNetworkTest;
    LastBlockHashes lastBlockHashes;
    BlockHeader blockHeader;
    s64 maxGasLimit = ChainParams(genesisInfo(networkName))
      .maxGasLimit.convert_to<s64>();
    // add value
    blockHeader.setGasLimit(maxGasLimit);
    blockHeader.setTimestamp(0);
    gas = maxGasLimit;
    Ethash::init();
    NoProof::init();
    SealEngineFace *se = ChainParams(genesisInfo(networkName)).createSealEngine();
    EnvInfo envInfo(blockHeader, lastBlockHashes, 0);
    executive = new Executive(state, envInfo, *se);
    nonce = 0;
  }

  void TargetProgram::deploy(bytes code) {
    state.setCode(contractAddress, bytes{code});
  }
  
  ExecutionResult TargetProgram::invoke(int type, bytes data, OnOpFunc onOp) {
    switch (type) {
      case CONTRACT_CONSTRUCTOR: {
        bytes code = state.code(contractAddress);
        code.insert(code.end(), data.begin(), data.end());
        state.setCode(contractAddress, bytes{code});
        ExecutionResult res = invoke(data, onOp);
        state.setCode(contractAddress, bytes{res.output});
        return res;
      }
      case CONTRACT_FUNCTION: {
        return invoke(data, onOp);
      }
      default: {
        throw "Unknown invoke type";
      }
    }
  }
  
  ExecutionResult TargetProgram::invoke(bytes data, OnOpFunc onOp) {
    ExecutionResult res;
    u256 value = 0;
    u256 gasPrice = 0;
    Transaction t = Transaction(value, gasPrice, gas, data, nonce);
    t.forceSender(sender);
    executive->setResultRecipient(res);
    executive->initialize(t);
    executive->call(contractAddress, sender, value, gasPrice, &data, gas);
    executive->go(onOp);
    executive->finalize();
    nonce ++;
    return res;
  }
  
  void TargetProgram::reset() {
    state.clearStorage(contractAddress);
  }
  
  TargetProgram::~TargetProgram() {
    delete executive;
  }
}
