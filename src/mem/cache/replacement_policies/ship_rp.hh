/**
 * @file
 * Declaration of a SHIP Policy
 * Signature Based Hit Predictor for High performance caching.
 * 
 */
#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SHIP_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SHIP_RP_HH__

#include "base/sat_counter.hh"
#include "mem/cache/replacement_policies/base.hh"

struct SHIPRPParams;

class SHIPRP : public BaseReplacementPolicy
{

 
  protected:
    /** SHIP-specific implementation of replacement data. */
    struct SHIPReplData : ReplacementData
    {
        
        /**
         * Re-Reference Interval Prediction Value.
         * A value equal to max_rrpv + 1 indicates an invalid entry.
         */
        SatCounter rrpv;
        
        bool valid;
        
        bool outcome;

        /** Signature field , for every memory region and PC , which is used to index in SHCT Table **/
        ushort signature;

        /**
         * Default constructor. Invalidate data.
         */
        SHIPReplData(const int num_bits) 
          :rrpv(num_bits),
           valid(false),
           outcome(false),
           signature(0)
        {
        }
    };

    /** Signature type to imdex Signature History Counter Table (SHCT): Memory, Program Counter
    * 0-> MEM, 
    * 1-> PC
    **/
    bool signature_type; 

    /**
     * SHCT saturating counter width
    */ 
    const unsigned numSHCTBits; 
     
     
     /**
     * Maximum Re-Reference Prediction Value possible. An entry with this
     * value as the rrpv has the longest possible re-reference interval,
     * that is, it is likely not to be used in the near future, and is
     * among the best eviction candidates.
     * A num_bits of 1 implies in a NRU.
     */
    const unsigned numRRPVBits;

    /**
     * The hit priority (HP) policy replaces entries that do not receive cache
     * hits over any cache entry that receives a hit, while the frequency
     * priority (FP) policy replaces infrequently re-referenced entries.
     */
    const bool hitPriority;

    /**
     * Bimodal throtle parameter. Value in the range [0,100] used to decide
     * if a new entry is inserted with long or distant re-reference.
     */
    const unsigned btp;

  private:
    /**
     * SHCT table of 16K entries
    **/
    mutable int signature_history_counter_array[16384];

  public:
    /** Convenience typedef. */
    typedef SHIPRPParams Params;

    /**
     * Construct and initiliaze this replacement policy.
     */
    SHIPRP(const Params *p);

    /**
     * Destructor.
     */
    ~SHIPRP() {}

    private:

    /**
     * Create a 14bit hashed PC as per SHiP paper. 
     * Used only for "Program Counter(PC)" type signature
     * Least significant 14bit Memory address tag is used for "Memory" type signature
    */
    ushort hash_function(Addr address_64_bit) const;
    
    /**
     * Invalidate replacement data to set it as the next probable victim.
     * Set RRPV as the the most distant re-reference.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                              const override;

    /**
     * Touch an entry to update its replacement data.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Reset replacement data. Used when an entry is inserted.
     * Set RRPV according to the insertion policy used.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Find replacement victim using rrpv.
     *
     * @param cands Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /**
     * Instantiate a replacement data entry.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_BRRIP_RP_HH__
