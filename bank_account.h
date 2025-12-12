#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

#include <string>
#include <map>
#include <memory>
#include <ostream>

class Customer; // forward

enum class Account_Type { STANDARD, SPECIAL };

std::ostream& operator<<(std::ostream&, Account_Type);

class Account {
public:
    Account(const std::string name, int dispo, int amount, std::shared_ptr<Customer> owner);
    virtual ~Account();

    virtual int withdraw(int x);
    int deposit(int x);

    unsigned get_id() const;
    unsigned owner_count() const;

    bool share_account(std::shared_ptr<Customer> new_owner);
    bool remove_owner(unsigned id);

    virtual std::string additional_output() const = 0;

    // getters
    const std::string get_name() const;
    int get_amount() const;
    int get_dispo() const;
    const std::map<unsigned, std::weak_ptr<Customer>>& get_owners() const;

    friend std::ostream& operator<<(std::ostream&, const Account&);

private:
    unsigned id;
    std::string name;
    int dispo;
    int amount;
    std::map<unsigned, std::weak_ptr<Customer>> owners;

    static unsigned next_id;
};

class Standard_Account : public Account {
public:
    Standard_Account(std::string name, int dispo, int amount, std::shared_ptr<Customer> owner);
    std::string additional_output() const override;
};

class Special_Account : public Account {
public:
    Special_Account(std::string name, int dispo, int amount,
                    std::shared_ptr<Customer> owner, int fee);

    int withdraw(int x) override;
    std::string additional_output() const override;

private:
    int fee;
};

#endif // BANK_ACCOUNT_H
