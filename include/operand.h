#ifndef INT8_H_
# define INT8_H_

# include <ioperand.h> // IOperand, eOperandtype
# include <string> // string
# include <cstdint> // int8_t
# include <convert.h> // toString()
# include <algorithm> // max()
# include <vm_exceptions.h>
# include <limits> // max(), min(), is_integer, is_singed
# include <cmath> // fmodf()

template <typename T>
T convertOperand(IOperand const * op);

template <typename T>
IOperand const *add(IOperand const &left, IOperand const &right);

template <typename T>
IOperand const *subtract(IOperand const &left, IOperand const &right);

template <typename T>
IOperand const *multiply(IOperand const &left, IOperand const &right);

template <typename T>
IOperand const *divide(IOperand const &left, IOperand const &right);

template <typename T>
IOperand const *modulus(IOperand const &left, IOperand const &right);

template <typename T>
class Operand : public IOperand
{
private:
    T _value;
    std::string _string;
    Operand<T>(void) :
        _value(0),
        _string("0")
    {}

public:
    Operand<T>(Operand<T> const &target) :
        _value(target._value),
        _string(target._string)
    {}
    Operand<T>(T const value) :
        _value(value),
        _string(to_string(value))
    {}
    virtual ~Operand<T>(void) {}
    Operand<T> &operator=(Operand<T> const &target) {
        this->_value = target._value;
        return *this;
    }

    int getPrecision(void) const
    {
        return this->getType();
    }
    eOperandType getType(void) const;
    T getValue(void) const {
        return this->_value;
    }

    IOperand const * operator+(IOperand const & rhs) const {
        auto precision = std::max(this->getPrecision(), rhs.getPrecision());

        switch (precision) {
            case Int8:
                return add<int8_t>(*this, rhs);
            case Int16:
                return add<int16_t>(*this, rhs);
            case Int32:
                return add<int32_t>(*this, rhs);
            case Float:
                return add<float>(*this, rhs);
            case Double:
                return add<double>(*this, rhs);
            default:
                throw CorruptIOperandException();
        }
    }
    IOperand const * operator-(IOperand const & rhs) const {
        auto precision = std::max(this->getPrecision(), rhs.getPrecision());

        switch (precision) {
            case Int8:
                return subtract<int8_t>(*this, rhs);
            case Int16:
                return subtract<int16_t>(*this, rhs);
            case Int32:
                return subtract<int32_t>(*this, rhs);
            case Float:
                return subtract<float>(*this, rhs);
            case Double:
                return subtract<double>(*this, rhs);
            default:
                throw CorruptIOperandException();
        }
    }
    IOperand const * operator*( IOperand const & rhs ) const {
        auto precision = std::max(this->getPrecision(), rhs.getPrecision());

        switch (precision) {
            case Int8:
                return multiply<int8_t>(*this, rhs);
            case Int16:
                return multiply<int16_t>(*this, rhs);
            case Int32:
                return multiply<int32_t>(*this, rhs);
            case Float:
                return multiply<float>(*this, rhs);
            case Double:
                return multiply<double>(*this, rhs);
            default:
                throw CorruptIOperandException();
        }
    }
    IOperand const * operator/( IOperand const & rhs ) const {
        auto precision = std::max(this->getPrecision(), rhs.getPrecision());

        switch (precision) {
            case Int8:
                return divide<int8_t>(*this, rhs);
            case Int16:
                return divide<int16_t>(*this, rhs);
            case Int32:
                return divide<int32_t>(*this, rhs);
            case Float:
                return divide<float>(*this, rhs);
            case Double:
                return divide<double>(*this, rhs);
            default:
                throw CorruptIOperandException();
        }
    }
    IOperand const * operator%( IOperand const & rhs ) const {
        auto precision = std::max(this->getPrecision(), rhs.getPrecision());

        switch (precision) {
            case Int8:
                return modulus<int8_t>(*this, rhs);
            case Int16:
                return modulus<int16_t>(*this, rhs);
            case Int32:
                return modulus<int32_t>(*this, rhs);
            case Float:
                return modulus<float>(*this, rhs);
            case Double:
                return modulus<double>(*this, rhs);
            default:
                throw CorruptIOperandException();
        }
    }

    std::string const &toString(void) const {
        return this->_string;
    }
    T getValue(void) {
        return this->_value;
    }
};

template <>
inline eOperandType Operand<int8_t>::getType(void) const {
    return Int8;
}

template <>
inline eOperandType Operand<int16_t>::getType(void) const {
    return Int16;
}

template <>
inline eOperandType Operand<int32_t>::getType(void) const {
    return Int32;
}

template <>
inline eOperandType Operand<float>::getType(void) const {
    return Float;
}

template <>
inline eOperandType Operand<double>::getType(void) const {
    return Double;
}

template <typename T>
T convertOperand(IOperand const * op)
{
    switch (op->getPrecision())
    {
        case Int8:
            return ((Operand<int8_t>*)op)->getValue();
        case Int16:
            return ((Operand<int16_t>*)op)->getValue();
        case Int32:
            return ((Operand<int32_t>*)op)->getValue();
        case Float:
            return ((Operand<float>*)op)->getValue();
        case Double:
            return ((Operand<double>*)op)->getValue();
        default:
            throw CorruptIOperandException();
    }
}

template <typename T>
IOperand const *multiply(IOperand const &left, IOperand const &right) {
    T l = convertOperand<T>(&left);
    T r = convertOperand<T>(&right);

    bool left_positive =  l > 0;
    bool right_positive = r > 0;

    if (l != 0) {
        if (left_positive == right_positive) {
            // max < r * l == true
            if (std::numeric_limits<T>::max() / l < r)
                throw OverflowException();
        } else {
            // min > r * l == true
            if (std::numeric_limits<T>::min() / l > r)
                throw UnderflowException();
        }
    }

    return new Operand<T>(l * r);
}

template <>
inline IOperand const *multiply<float>(IOperand const &left, IOperand const &right) {
    float result = convertOperand<float>(&left) * convertOperand<float>(&right);

    if (isinf(result))
        throw OverflowException();
    return new Operand<float>(result);
}

template <>
inline IOperand const *multiply<double>(IOperand const &left, IOperand const &right) {
    double result = convertOperand<double>(&left) * convertOperand<double>(&right);

    if (isinf(result))
        throw OverflowException();
    return new Operand<double>(result);
}

template <typename T>
IOperand const *divide(IOperand const &left, IOperand const &right) {
    T right_val = convertOperand<T>(&right);

    if (right_val == 0)
        throw OperationByZeroException("Divide");

    return new Operand<T>(convertOperand<T>(&left) / right_val);
}

template <typename T>
IOperand const *add(IOperand const &left, IOperand const &right) {
    T l = convertOperand<T>(&left);
    T r = convertOperand<T>(&right);

    if (// if type can overflow
        std::numeric_limits<T>::is_signed
        && std::numeric_limits<T>::is_integer
    ) {
        if (r > 0) {
            // max < l + r == true
            if (std::numeric_limits<T>::max() - r < l)
                throw OverflowException();
        } else {
            // min > l + r == true
            if (std::numeric_limits<T>::min() - r > l)
                throw UnderflowException();
        }
    }

    return new Operand<T>(l + r);
}

template <typename T>
IOperand const *subtract(IOperand const &left, IOperand const &right) {
    T l = convertOperand<T>(&left);
    T r = convertOperand<T>(&right);

    if (// if type can overflow
        std::numeric_limits<T>::is_signed
        && std::numeric_limits<T>::is_integer
    ) {
        if (r > 0) {
            // min > l - r == true
            if (std::numeric_limits<T>::min() + r > l)
                throw OverflowException();
        } else {
            // max < l + r == true
            if (std::numeric_limits<T>::max() - r < l)
                throw UnderflowException();
        }
    }

    return new Operand<T>(l + r);
}

template <typename T>
IOperand const *modulus(IOperand const &left, IOperand const &right) {
    T right_val = convertOperand<T>(&right);

    if (right_val == 0)
        throw OperationByZeroException("Modulus");

    return new Operand<T>(
        convertOperand<T>(&left) % right_val
    );
}

template <>
inline IOperand const *modulus<float>(IOperand const &left, IOperand const &right) {
    float right_val = convertOperand<float>(&right);

    if (right_val == 0)
        throw OperationByZeroException("Modulus");

    return new Operand<float>(
        fmodf(convertOperand<float>(&left), right_val)
    );
}

template <>
inline IOperand const *modulus<double>(IOperand const &left, IOperand const &right) {
    double right_val = convertOperand<double>(&right);

    if (right_val == 0)
        throw OperationByZeroException("Modulus");

    return new Operand<double>(
        fmod(convertOperand<double>(&left), right_val)
    );
}

#endif
