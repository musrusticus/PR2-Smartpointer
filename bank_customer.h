#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <string>
#include <map>
#include <memory>
#include <ostream>

class Account;
class Standard_Account;
class Special_Account;
enum class Account_Type;

class Customer : public std::enable_shared_from_this<Customer> {
public:
    Customer(std::string name);

    unsigned create_account(std::string acc_name, int dispo, int amount,
                            Account_Type type, int fee = 0);

    int total_amount() const;

    unsigned get_id() const;
    const std::string& get_name() const;
    const std::map<unsigned, std::shared_ptr<Account>>& get_accounts() const;

    bool share_account(unsigned aid, std::shared_ptr<Customer> new_owner);

    bool transfer(int amount, unsigned source_id, std::shared_ptr<Customer> target, unsigned target_id);

    friend std::ostream& operator<<(std::ostream&, const Customer&);

private:
    unsigned id;
    std::string name;
    std::map<unsigned, std::shared_ptr<Account>> accounts;

    static unsigned next_id;
};

#endif // BANK_CUSTOMER_H
