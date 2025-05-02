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
    explicit BitEncodedSeq(const std::string &dna) { encode(dna); }

    // metadata
    std::uint32_t size() const noexcept { return length_; }
    bool empty() const noexcept { return length_ == 0; }

    // random access (O(1))
    char at(std::uint32_t idx) const
    {
        if (idx >= length_)
            throw std::out_of_range("idx");
        auto [wBit, shift] = wordShift(idx);
        word_t bits = (data_[wBit] >> shift) & 0b11;
        return decodeBase(bits);
    }

    // in-place mutation (O(1))
    void set(std::uint32_t idx, char base)
    {
        if (idx >= length_)
            throw std::out_of_range("idx");
        word_t mask = word_t(0b11) << wordShift(idx).second;
        word_t value = word_t(encodeBase(base)) << wordShift(idx).second;
        data_[wordShift(idx).first] =
            (data_[wordShift(idx).first] & ~mask) | value;
    }

    // expose raw blocks if an algorithm wants bit-parallel tricks
    const std::vector<word_t> &blocks() const noexcept { return data_; }

    void printSeq()
    {
        for (std::uint32_t i = 0; i < size(); ++i)
        {
            std::cout << at(i);
        }
        std::cout << '\n';
    }

    void printEncoded()
    {
        for (std::uint32_t i = 0; i < size(); ++i)
        {
            switch (at(i))
            {
            case 'A':
                std::cout << "00";
                break;
            case 'C':
                std::cout << "01";
                break;
            case 'T':
                std::cout << "10";
                break;
            case 'G':
                std::cout << "11";
                break;
            default:
                std::cout << "??"; // should never happen
            }
        }
        std::cout << '\n';
    }

private:
    std::vector<word_t> data_; // packed 64-bit blocks
    std::uint32_t length_;     // **true number of bases**

    //-----------------------------------------
    static std::uint8_t encodeBase(char b)
    {
        switch (b)
        { // upper-case only
        case 'A':
            return 0b00;
        case 'C':
            return 0b01;
        case 'T':
            return 0b10;
        case 'G':
            return 0b11;
        default:
            throw std::invalid_argument("bad base");
        }
    }
    static char decodeBase(std::uint8_t x)
    {
        static const char LUT[4] = {'A', 'C', 'T', 'G'};
        return LUT[x];
    }

    // (idx / 32 , (31-idx%32)*2) : left-aligned bits, MSB first
    static std::pair<std::size_t, std::uint32_t>
    wordShift(std::uint32_t idx)
    {
        std::size_t word = idx / BASES_PER_WORD;
        std::uint32_t pos = BASES_PER_WORD - 1 - (idx % BASES_PER_WORD);
        return {word, pos * BITS_PER_BASE};
    }
    //-----------------------------------------
    void encode(const std::string &s)
    {
        length_ = static_cast<std::uint32_t>(s.size());
        std::size_t words = (length_ + BASES_PER_WORD - 1) / BASES_PER_WORD;
        data_.assign(words, 0);

        for (std::uint32_t i = 0; i < length_; ++i)
        {
            set(i, s[i]);
        }
        // unused tail bits already zero -- harmless padding,
        // algorithms must iterate only up to length_
    }
};
