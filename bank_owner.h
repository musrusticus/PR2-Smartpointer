#ifndef BANK_OWNER_H
#define BANK_OWNER_H

#include <string>
#include <memory>
#include <ostream>
#include "bank.h"

class Bank_Owner {
public:
    Bank_Owner(std::string name);

    bool create_bank(std::string name);
    bool transfer_bank(Bank_Owner& target);

    const std::map<unsigned, std::shared_ptr<Customer>>& get_customers() const;

    void create_customer(const std::string& name, const std::string& acc_name,
                         int dispo, int amount, Account_Type type = Account_Type::STANDARD, int fee = 0);

    friend std::ostream& operator<<(std::ostream&, const Bank_Owner&);

private:
    std::string name;
    std::unique_ptr<Bank> bank;
};

#endif // BANK_OWNER_H

