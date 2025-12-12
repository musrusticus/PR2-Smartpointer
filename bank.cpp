// bank.cpp
#include "bank.h"

#include <stdexcept>

Bank::Bank(std::string name_) : name(name_) {
    if (name_.empty()) throw std::runtime_error("Bank: name mustnt be empty");
}

const std::map<unsigned, std::shared_ptr<Customer>>& Bank::get_customers() const {
    return customers;
}

// Create customer + initial account and insert into customers
void Bank::create_customer(std::string name, std::string acc_name, int dispo, int amount, Account_Type type, int fee)
{
    // make_shared ensures Customer lives inside a shared_ptr so shared_from_this() works
    std::shared_ptr<Customer> c = std::make_shared<Customer>(name);
    
    (void)c->create_account(acc_name, dispo, amount, type, fee);
    customers[c->get_id()] = c;
}


std::ostream& operator<<(std::ostream& o, const Bank& b) {
    o << "[" << b.name << ", {";

    bool first = true;
    for (const auto& kv : b.customers) {
        if (!first) o << ", ";
        o << *(kv.second);
        first = false;
    }

    o << "}]";
    return o;
}
