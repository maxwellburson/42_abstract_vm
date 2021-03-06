#include <abstract_vm.h>

#include <ioperand.h> //eOperandType, IOperand
#include <operand.h> //Operand
#include <operand_factory.h> //OperandFactory
#include <parser.h> //Parser, eInstructionType
#include <vm_exceptions.h> //VMException

#include <algorithm> //for_each
#include <vector> //vector
#include <iostream> //cout cerr

void (AbstractVM::* const AbstractVM::_instructions[])(void) = {
    &AbstractVM::_push,
    &AbstractVM::_pop,
    &AbstractVM::_dump,
    &AbstractVM::_assert,
    &AbstractVM::_add,
    &AbstractVM::_sub,
    &AbstractVM::_mul,
    &AbstractVM::_div,
    &AbstractVM::_mod,
    &AbstractVM::_print,
};

void AbstractVM::_push(void) {
    this->_vector.push_back(this->_parser.getOperand());
}

void AbstractVM::_pop(void) {
    if (this->_vector.empty())
        throw PopException();

    delete this->_vector.back();
    this->_vector.pop_back();
}

void AbstractVM::_assert(void) {
    IOperand const *op = this->_parser.getOperand();

    if (this->_vector.empty())
        throw AssertException();

    if (this->_vector.back()->toString() != op->toString()) {
        delete op;
        throw AssertException();
    }
    delete op;
}

void AbstractVM::_add(void) {
    IOperand const *first;
    IOperand const *second;

    if (this->_vector.size() < 2)
        throw StackSizeException("Add", this->_vector.size());

    first = this->_vector.back();
    this->_vector.pop_back();
    second = this->_vector.back();
    this->_vector.pop_back();

    this->_vector.push_back((*second + *first));
}

void AbstractVM::_sub(void) {
    IOperand const *first;
    IOperand const *second;

    if (this->_vector.size() < 2)
        throw StackSizeException("Sub", this->_vector.size());

    first = this->_vector.back();
    this->_vector.pop_back();
    second = this->_vector.back();
    this->_vector.pop_back();

    this->_vector.push_back((*second - *first));
}

void AbstractVM::_mul(void) {
    IOperand const *first;
    IOperand const *second;

    if (this->_vector.size() < 2)
        throw StackSizeException("Mul", this->_vector.size());

    first = this->_vector.back();
    this->_vector.pop_back();
    second = this->_vector.back();
    this->_vector.pop_back();

    this->_vector.push_back((*second * *first));
}

void AbstractVM::_div(void) {
    IOperand const *first;
    IOperand const *second;

    if (this->_vector.size() < 2)
        throw StackSizeException("Div", this->_vector.size());

    first = this->_vector.back();
    this->_vector.pop_back();
    second = this->_vector.back();
    this->_vector.pop_back();

    this->_vector.push_back((*second / *first));
}

void AbstractVM::_mod(void) {
    IOperand const *first;
    IOperand const *second;

    if (this->_vector.size() < 2)
        throw StackSizeException("Mod", this->_vector.size());

    first = this->_vector.back();
    this->_vector.pop_back();
    second = this->_vector.back();
    this->_vector.pop_back();

    this->_vector.push_back((*second % *first));
}

void AbstractVM::_print(void) {
    auto operand = this->_vector.back();

    if (operand->getType() != Int8)
        throw PrintException();

    std::cout << (*(Operand<int8_t>*)operand).getValue();
}

void AbstractVM::_dump(void) {
    auto j = this->_vector.begin();
    auto i = this->_vector.end();
    while (i != j) {
        i--;
        std::cout << **i << "\n";
    }
}

AbstractVM::AbstractVM(std::istream &instr_input) :
    _vector(),
    _parser(instr_input)
{}


AbstractVM::AbstractVM(void) :
    _vector(),
    _parser()
{}

AbstractVM::AbstractVM(AbstractVM const &target) :
    _vector(target._vector),
    _parser(target._parser)
{}

AbstractVM::~AbstractVM(void) {
    for_each(this->_vector.begin(), this->_vector.end(), [](IOperand const *op) {
        delete op;
    });
}

AbstractVM &AbstractVM::operator=(AbstractVM const &target) {
    this->_vector = target._vector;

    return *this;
}

int AbstractVM::run(void)
{
    eInstructionType type;

    try {
        while (Exit != (type = this->_parser.nextInstructionType())) {
            (this->*AbstractVM::_instructions[type])();
        }
    } catch(VMException &e) {
        std::cerr << "Line "
            << std::to_string(this->_parser.getLineNumber())
            << " : Error : " << e.what() << "\n";
        return 1;
    }
    return 0;
}
