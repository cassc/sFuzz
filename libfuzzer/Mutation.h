#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <libdevcore/CommonIO.h>
#include <libethereum/Block.h>
#include <libethereum/ChainParams.h>
#include <libethereum/Executive.h>
#include <libethashseal/GenesisInfo.h>
#include <libethereum/LastBlockHashesFace.h>
#include "TargetContainer.h"
#include "Fuzzer.h"
#include "Dictionary.h"

using namespace dev;
using namespace eth;
using namespace std;

namespace fuzzer {
  using OnMutateFunc = function<FuzzItem (bytes b)>;
  class Mutation {
    FuzzItem curFuzzItem;
    Dictionary dict;
    int dataSize;
    int effCount;
    /* TODO: update later */
    u32 spliceCycle;
    u8 doingDet;
    u32 perfScore;
    h256 prevCksum;
    bytes eff;
    void flipbit(int pos);
    public:
      Mutation(FuzzItem item, Dictionary dict);
      void singleWalkingBit(OnMutateFunc cb);
      void twoWalkingBit(OnMutateFunc cb);
      void fourWalkingBit(OnMutateFunc cb);
      void singleWalkingByte(OnMutateFunc cb);
      void twoWalkingByte(OnMutateFunc cb);
      void fourWalkingByte(OnMutateFunc cb);
      void singleArith(OnMutateFunc cb);
      void twoArith(OnMutateFunc cb);
      void fourArith(OnMutateFunc cb);
      void singleInterest(OnMutateFunc cb);
      void twoInterest(OnMutateFunc cb);
      void fourInterest(OnMutateFunc cb);
      void overwriteWithDictionary(OnMutateFunc cb);
      void insertWithDictionary(OnMutateFunc cb);
      void havoc(OnMutateFunc cb);
      void splice(OnMutateFunc cb);
      static int havocDiv;
  };
}