// Copyright (c) 2018-2020 The iBTC Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <iostream>

#include "variable_block_reward.h"
#include <amount.h>
#include <consensus/params.h>
#include <primitives/block.h>

#include <arith_uint256.h>

static uint32_t get_num_consecutive_ones(uint32_t a, uint32_t b);
static uint32_t get_max_extra_multiply_allowed(uint32_t num_consecutive_ones);

CAmount GetBlockSubsidyVBR(int nHeight, const Consensus::Params& consensusParams, const CBlock &block, bool print)
{
    int halvings = nHeight / consensusParams.nSubsidyHalvingInterval;
    // Force block reward to zero when right shift is undefined.
    if (halvings >= 64)
        return 0;

    CAmount nSubsidy = 0;        

    // Premine 25Million so we can provide 16 Million swap to exchanges.  dev team gets 9 Million for future listings
    if ( 1 == nHeight )
    {
        nSubsidy = 25000000 * COIN;
        return nSubsidy;        
    }

    // Only allow VBR every 64th block
    if ( nHeight % 64 )
    {
        nSubsidy = 50 * COIN;

        // Subsidy is cut in half every XXXX blocks which will occur approximately every 4 years.
        nSubsidy >>= halvings;
        return nSubsidy;
    }
    

    uint32_t prev_hash_0 = get_32bit_word_from_uint256(block.hashPrevBlock);

    uint32_t merkle_root_0 = get_32bit_word_from_uint256(block.hashMerkleRoot);

    // count num of matching bits starting from right.
    uint32_t consec_ones = get_num_consecutive_ones(prev_hash_0, merkle_root_0);

    uint32_t extra_multiply = get_max_extra_multiply_allowed(consec_ones);

    if (print)
    {
      std::cout << "block.hashPrevBlock: " << block.hashPrevBlock.ToString() << std::endl;
      std::cout << "prev_hash_0: " << std::hex << prev_hash_0 << std::endl;   

      std::cout << "block.hashMerkleRoot: " << block.hashMerkleRoot.ToString() << std::endl;
      std::cout << "merkle_root_0: " << std::hex << merkle_root_0 << std::endl; 

      std::cout << "# bits match: " << consec_ones << std::endl;
    }

    nSubsidy = (50 * extra_multiply) * COIN;

    // Subsidy is cut in half every XXXX blocks which will occur approximately every 4 years.
    nSubsidy >>= halvings;
    return nSubsidy;
}

// count num of matching bits starting from right.
uint32_t get_num_consecutive_ones(uint32_t a, uint32_t b)
{
    // xnor gives a '1' if they match
    uint32_t reg = ~(a ^ b);

    uint32_t num = 0;
    for (int n=0; n<32; n++)
    {
        if ( (reg & (1<<n)) > 0 )
        {
            num++;
        }
        else
        {
            break;
        }
    }

    return num;
}

// The more the consecutive bits, the bigger the BlockReward
uint32_t get_max_extra_multiply_allowed(uint32_t num_consecutive_ones)
{
    if ( num_consecutive_ones <= 1)
    {
        return 1;
    }

    uint64_t max_allowed = 1 << (num_consecutive_ones);

    // max extra reward is 64 times regular block reward
    if ( max_allowed >= MAX_VBR_MULTIPLY )
    {
        max_allowed = MAX_VBR_MULTIPLY;
    }

    return max_allowed;
}

// Find nearest lower power of 2
uint32_t floor_power_2_vbr (int32_t x)
{
    if ( x < 1) {
        return 1;
    }
    if ( x > MAX_VBR_MULTIPLY ) {
        return MAX_VBR_MULTIPLY;
    }

    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x - (x >> 1);
}
