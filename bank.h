#ifndef BANK_H
#define BANK_H

#include <map>
#include <string>
#include <memory>
#include <ostream>

#include "bank_customer.h"
#include "bank_account.h"

class Bank {
public:
    Bank(std::string name);

    const std::map<unsigned, std::shared_ptr<Customer>>& get_customers() const;

    void create_customer(std::string name, std::string acc_name,
                         int dispo, int amount, Account_Type type = Account_Type::STANDARD, int fee = 0);

    friend std::ostream& operator<<(std::ostream&, const Bank&);

private:
    std::string name;
    std::map<unsigned, std::shared_ptr<Customer>> customers;
};

#endif // BANK_H
