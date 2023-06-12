#ifndef YADAW_SRC_UTIL_SAMPLEFORMAT
#define YADAW_SRC_UTIL_SAMPLEFORMAT

#include "native/Native.hpp"
#include "util/Endian.hpp"

#include <cstdint>
#include <type_traits>

namespace YADAW::Util
{
namespace Impl
{
// This class is ABI compatible with basic data types. So code like
// auto* data = reinterpret_cast<TypeWithReverseByteOrder<int>*>(pointer);
// for(int i = 0; i < 10; ++i) { data[i] = i; }
// Is acceptable.
template<typename T>
class TypeWithReverseByteOrder
{
    static_assert(!std::is_reference<T>::value, "Error: TypeWithReverseByteOrder<Reference> is not allowed.");
    static_assert(std::is_arithmetic<T>::value,
        "Error: TypeWithReverseByteOrder<NonArithmeticType> is not allowed in this library.");
    static_assert((!std::is_same<std::decay_t<T>, std::uint8_t>::value) &&
                  (!std::is_same<std::decay_t<T>, std::int8_t>::value) &&
                  (!std::is_same<std::decay_t<T>, char>::value),
        "Error: TypeWithReverseByteOrder with 8 bit data is pointless.");
    static_assert(!std::is_const<T>::value,
        "Error: TypeWithReverseByteOrder<const Type> is not allowed. Use const TypeWithReverseByteOrder<Type> instead.");
public:
    using Type = T;
    using Self = TypeWithReverseByteOrder<T>;
public:
    TypeWithReverseByteOrder(): data_(T()) {}
    TypeWithReverseByteOrder(T data): data_(data)
    {
        convertEndian();
    }
    TypeWithReverseByteOrder(const void* memory): data_(*reinterpret_cast<const T*>(memory)) {}
    TypeWithReverseByteOrder(const Self& rhs): data_(rhs.data_) {}
    TypeWithReverseByteOrder(Self&& rhs) noexcept: data_(std::move(rhs.data_)) {}
    Self& operator=(const T& data)
    {
        data_ = data;
        convertEndian();
        return *this;
    }
    Self& operator=(T&& data)
    {
        data_ = std::move(data);
        convertEndian();
        return *this;
    }
    Self& operator=(const Self rhs)
    {
        data_ = rhs.data_;
        return *this;
    }
public:
    Self& operator+=(T rhs)
    {
        return this->operator=(T(*this) + rhs);
    }

    Self& operator-=(T rhs)
    {
        return this->operator=(T(*this) - rhs);
    }
    Self& operator*=(T rhs)
    {
        return this->operator=(T(*this) * rhs);
    }
    Self& operator/=(T rhs)
    {
        return this->operator=(T(*this) / rhs);
    }
    Self& operator%=(T rhs)
    {
        return this->operator=(T(*this) % rhs);
    }
    Self& operator&=(T rhs)
    {
        return this->operator=(T(*this) & rhs);
    }
    Self& operator|=(T rhs)
    {
        return this->operator=(T(*this) | rhs);
    }
    Self& operator^=(T rhs)
    {
        return this->operator=(T(*this) ^ rhs);
    }
    Self& operator>>=(int digit)
    {
        T data(*this);
        data >>= digit;
        this->operator=(data);
        return *this;
    }
    Self& operator<<=(int digit)
    {
        T data(*this);
        data <<= digit;
        this->operator=(data);
        return *this;
    }
    bool operator==(const Self& rhs) const
    {
        return data_ == rhs.data_;
    }
    bool operator!=(const Self& rhs) const
    {
        return data_ != rhs.data_;
    }
    Self& operator++()
    {
        return this->operator=(++T(data_));
    }
    Self operator++(int)
    {
        TypeWithReverseByteOrder ret = *this;
        this->operator++();
        return ret;
    }
    Self& operator--()
    {
        return this->operator=(--T(data_));
    }
    Self operator--(int)
    {
        TypeWithReverseByteOrder ret = *this;
        this->operator--();
        return ret;
    }
    operator T() const
    {
        Self copy = *this;
        copy.convertEndian();
        return copy.data_;
    }
protected:
    void convertEndian()
    {
        std::array<std::int8_t, sizeof(data_)>* thisAsArray = reinterpret_cast<decltype(thisAsArray)>(this);
        std::reverse(thisAsArray->begin(), thisAsArray->end());
    }
private:
    T data_;
};

template<typename T>
std::istream& operator>>(std::istream& is, TypeWithReverseByteOrder<T>& data)
{
    T temp;
    is >> temp;
    data = temp;
    return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const TypeWithReverseByteOrder<T>& type)
{
    os << T(type);
    return os;
}
}

#if(Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
using Int16LE = std::int16_t;
using UInt16LE = std::uint16_t;
using Int32LE = std::int32_t;
using UInt32LE = std::uint32_t;
using Int64LE = std::int64_t;
using UInt64LE = std::uint64_t;
using FloatLE = float;
using DoubleLE = double;
using LongDoubleLE = long double;

using Int16BE = Impl::TypeWithReverseByteOrder<std::int16_t>;
using UInt16BE = Impl::TypeWithReverseByteOrder<std::uint16_t>;
using Int32BE = Impl::TypeWithReverseByteOrder<std::int32_t>;
using UInt32BE = Impl::TypeWithReverseByteOrder<std::uint32_t>;
using Int64BE = Impl::TypeWithReverseByteOrder<std::int64_t>;
using UInt64BE = Impl::TypeWithReverseByteOrder<std::uint64_t>;
using FloatBE = Impl::TypeWithReverseByteOrder<float>;
using DoubleBE = Impl::TypeWithReverseByteOrder<double>;
using LongDoubleBE = Impl::TypeWithReverseByteOrder<long double>;

using Int16 = Int16LE;
using UInt16 = UInt16LE;
using Int32 = Int32LE;
using UInt32 = UInt32LE;
using Int64 = Int64LE;
using UInt64 = UInt64LE;
using Float = FloatLE;
using Double = DoubleLE;
using LongDouble = LongDoubleLE;

using Int16RE = Int16BE;
using UInt16RE = UInt16BE;
using Int32RE = Int32BE;
using UInt32RE = UInt32BE;
using Int64RE = Int64BE;
using UInt64RE = UInt64BE;
using FloatRE = FloatBE;
using DoubleRE = DoubleBE;
using LongDoubleRE = LongDoubleBE;
#else
using Int16BE = std::int16_t;
using UInt16BE = std::uint16_t;
using Int32BE = std::int32_t;
using UInt32BE = std::uint32_t;
using Int64BE = std::int64_t;
using UInt64BE = std::uint64_t;
using FloatBE = float;
using DoubleBE = double;
using LongDoubleBE = long double;

using Int16LE = Impl::TypeWithReverseByteOrder<std::int16_t>;
using UInt16LE = Impl::TypeWithReverseByteOrder<std::uint16_t>;
using Int32LE = Impl::TypeWithReverseByteOrder<std::int32_t>;
using UInt32LE = Impl::TypeWithReverseByteOrder<std::uint32_t>;
using Int64LE = Impl::TypeWithReverseByteOrder<std::int64_t>;
using UInt64LE = Impl::TypeWithReverseByteOrder<std::uint64_t>;
using FloatLE = Impl::TypeWithReverseByteOrder<float>;
using DoubleLE = Impl::TypeWithReverseByteOrder<double>;
using LongDoubleLE = Impl::TypeWithReverseByteOrder<long double>;

using Int16 = Int16BE;
using UInt16 = UInt16BE;
using Int32 = Int32BE;
using UInt32 = UInt32BE;
using Int64 = Int64BE;
using UInt64 = UInt64BE;
using Float = FloatBE;
using Double = DoubleBE;
using LongDouble = LongDoubleBE;

using Int16RE = Int16LE;
using UInt16RE = UInt16LE;
using Int32RE = Int32LE;
using UInt32RE = UInt32LE;
using Int64RE = Int64LE;
using UInt64RE = UInt64LE;
using FloatRE = FloatLE;
using DoubleRE = DoubleLE;
using LongDoubleRE = LongDoubleLE;
#endif
}

#endif // YADAW_SRC_UTIL_SAMPLEFORMAT
