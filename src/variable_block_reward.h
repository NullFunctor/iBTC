// Copyright (c) 2018-2020 The iBTC Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#include <amount.h>
#include <consensus/params.h>
#include <primitives/block.h>

#define MAX_VBR_MULTIPLY    64

CAmount GetBlockSubsidyVBR(int nHeight, const Consensus::Params& consensusParams, const CBlock &block, bool print);
uint32_t floor_power_2_vbr (int32_t x);
