#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iostream>

class BitEncodedSeq
{
public:
    using word_t = std::uint64_t; // 64-bit block
    static constexpr int BITS_PER_BASE = 2;
    static constexpr int BITS_PER_WORD = 64;
    static constexpr int BASES_PER_WORD = BITS_PER_WORD / BITS_PER_BASE; // 32

    // constructor from plain string
    explicit BitEncodedSeq(const std::string &dna);

    // metadata
    std::uint32_t size() const noexcept;
    bool empty() const noexcept;

    // random access (O(1))
    char at(std::uint32_t idx) const;

    // in-place mutation (O(1))
    void set(std::uint32_t idx, char base);

    // Comparator
    bool operator==(const BitEncodedSeq &other) const noexcept;

    // expose raw blocks if an algorithm wants bit-parallel tricks
    const std::vector<word_t> &blocks() const noexcept;

    void printSeq();

    void printEncoded();

private:
    std::vector<word_t> data_; // packed 64-bit blocks
    std::uint32_t length_;     // **true number of bases**

    //-----------------------------------------
    static std::uint8_t encodeBase(char b);
    static char decodeBase(std::uint8_t x);

    // (idx / 32 , (31-idx%32)*2) : left-aligned bits, MSB first
    static std::pair<std::size_t, std::uint32_t> wordShift(std::uint32_t idx);
    //-----------------------------------------
    void encode(const std::string &s);
};
