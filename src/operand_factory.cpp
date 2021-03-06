#include <operand_factory.h>
#include <operand.h> //Operand
#include <ioperand.h> //IOperand, eOperandtype
#include <vm_exceptions.h>

#include <cstdint> //int8_t, int16_t, int32_t
#include <string> //string, stod, stof
#include <sstream> //stringstream

IOperand const *OperandFactory::createInt8(
    std::string const &value
) const {
    return this->createSpecific<int8_t>(value);
}

IOperand const *OperandFactory::createInt16(
    std::string const &value
) const {
    return this->createSpecific<int16_t>(value);
}

IOperand const *OperandFactory::createInt32(
    std::string const &value
) const {
    return this->createSpecific<int32_t>(value);
}

IOperand const *OperandFactory::createFloat(
    std::string const &value
) const {
    return this->createSpecific<float>(value);
}

IOperand const *OperandFactory::createDouble(
    std::string const &value
) const {
    return this->createSpecific<double>(value);
}

IOperand const * OperandFactory::createOperand(
    eOperandType type,
    std::string const &value
) const {
    //todo: use array of func pointers
    switch (type) {
        case Int8:
            return this->createInt8(value);
        case Int16:
            return this->createInt16(value);
        case Int32:
            return this->createInt32(value);
        case Float:
            return this->createFloat(value);
        case Double:
            return this->createDouble(value);
        default:
            throw VMException("Bad operand type");
    }
}
