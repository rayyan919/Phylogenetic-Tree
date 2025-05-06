#include "../include/BitEncodedSeq.h"

BitEncodedSeq::BitEncodedSeq(const std::string &dna) { encode(dna); }

std::uint32_t BitEncodedSeq::size() const noexcept { return length_; }
bool BitEncodedSeq::empty() const noexcept { return length_ == 0; }

char BitEncodedSeq::at(std::uint32_t idx) const
{
    if (idx >= length_)
        throw std::out_of_range("idx");
    auto [wBit, shift] = wordShift(idx);
    word_t bits = (data_[wBit] >> shift) & 0b11;
    return decodeBase(bits);
}

void BitEncodedSeq::set(std::uint32_t idx, char base)
{
    if (idx >= length_)
        throw std::out_of_range("idx");
    word_t mask = word_t(0b11) << wordShift(idx).second;
    word_t value = word_t(encodeBase(base)) << wordShift(idx).second;
    data_[wordShift(idx).first] =
        (data_[wordShift(idx).first] & ~mask) | value;
}

bool BitEncodedSeq::operator==(const BitEncodedSeq &other) const noexcept
{
    if (length_ != other.length_)
        return false;
    return std::equal(data_.begin(), data_.end(), other.data_.begin());
}
const std::vector<BitEncodedSeq::word_t> &BitEncodedSeq::blocks() const noexcept { return data_; }

void BitEncodedSeq::printSeq()
{
    for (std::uint32_t i = 0; i < size(); ++i)
    {
        std::cout << at(i);
    }
    std::cout << '\n';
}

void BitEncodedSeq::printEncoded()
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

std::uint8_t BitEncodedSeq::encodeBase(char b)
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

char BitEncodedSeq::decodeBase(std::uint8_t x)
{
    static const char LUT[4] = {'A', 'C', 'T', 'G'};
    return LUT[x];
}

std::pair<std::size_t, std::uint32_t> BitEncodedSeq::wordShift(std::uint32_t idx)
{
    std::size_t word = idx / BASES_PER_WORD;
    std::uint32_t pos = BASES_PER_WORD - 1 - (idx % BASES_PER_WORD);
    return {word, pos * BITS_PER_BASE};
}

void BitEncodedSeq::encode(const std::string &s)
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