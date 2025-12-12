// bank_owner.cpp
#include "bank_owner.h"
#include "bank.h"

#include <stdexcept>
#include <utility>
#include <ostream>

Bank_Owner::Bank_Owner(std::string name_) : name(name_), bank(nullptr) {
    if (name_.empty()) throw std::runtime_error("Bank_Owner: name mustnt be empty");
}

bool Bank_Owner::create_bank(std::string name_) {
    if (bank) return false; // already has a bank
    bank = std::make_unique<Bank>(name_);
    return true;
}

bool Bank_Owner::transfer_bank(Bank_Owner& target) {
    if (!bank) return false;          // this has no bank
    if (target.bank) return false;    // target already has a bank
    target.bank = std::move(bank);
    return true;
}

const std::map<unsigned, std::shared_ptr<Customer>>& Bank_Owner::get_customers() const {
    if (!bank) throw std::runtime_error("Bank_Owner::get_customers: no bank avaliable");
    return bank->get_customers();
}

void Bank_Owner::create_customer(const std::string& name_, const std::string& acc_name, int dispo, int amount, Account_Type type, int fee)
{
    if (!bank) throw std::runtime_error("Bank_Owner::create_customer: no bank avaliable");
    bank->create_customer(name_, acc_name, dispo, amount, type, fee);
}

std::ostream& operator<<(std::ostream& o, const Bank_Owner& bo) {
    o << "[" << bo.name;
    if (bo.bank) {
        o << ", " << *bo.bank;
    } else {
        o << "";
    }
    o << "]";
    return o;
}
