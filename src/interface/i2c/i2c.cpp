#include "global.h"
#include "interface/i2c/i2c.h"
#include "sys/util.h"


enum I2C::Result I2C::Bus::readRegs(int address, int reg, void* buf, int len)
{
    if (address < 0 || address > 1023) return RESULT_INVALID_ARGUMENT;
    if (reg < 0 || reg > 255) return RESULT_INVALID_ARGUMENT;
    struct __attribute__((packed,aligned(4)))
    {
        Transaction info;
        Transaction::Transfer transfers[2];
    } txn =
    {
        Transaction(address, ARRAYLEN(txn.transfers)),
        {
            Transaction::Transfer(Transaction::Transfer::TYPE_TX, 1, &reg),
            Transaction::Transfer(Transaction::Transfer::TYPE_RX, len, buf),
        }
    };
    return this->txn(&txn.info);
}

enum I2C::Result I2C::Bus::writeRegs(int address, int reg, const void* buf, int len)
{
    if (address < 0 || address > 1023) return RESULT_INVALID_ARGUMENT;
    if (reg < 0 || reg > 255) return RESULT_INVALID_ARGUMENT;
    struct __attribute__((packed,aligned(4)))
    {
        Transaction info;
        Transaction::Transfer transfers[2];
    } txn =
    {
        Transaction(address, ARRAYLEN(txn.transfers)),
        {
            Transaction::Transfer(Transaction::Transfer::TYPE_TX, 1, &reg),
            Transaction::Transfer(Transaction::Transfer::TYPE_CONT, len, buf),
        }
    };
    return this->txn(&txn.info);
}

enum I2C::Result I2C::LLBus::txn(const I2C::Transaction* txn)
{
    enum Result rc;
    uint8_t byte;
    bool rx = false;
    for (int i = 0; i < txn->transferCount; i++)
    {
        if (txn->transfers[i].type == Transaction::Transfer::TYPE_TX) rx = false;
        else if (txn->transfers[i].type == Transaction::Transfer::TYPE_RX) rx = true;
        if (txn->transfers[i].type != Transaction::Transfer::TYPE_CONT)
        {
            int addr = txn->address;
            if ((rc = start()) != RESULT_OK) return rc;
            if (addr > 0x77)
            {
                byte = (0x78 | (addr >> 8)) << 1;
                if (rx)
                {
                    if (!i)
                    {
                        if ((rc = this->tx(&byte, 1)) != RESULT_OK) return rc;
                        if ((rc = this->tx(&addr, 1)) != RESULT_OK) return rc;
                    }
                    byte |= 1;
                    if ((rc = this->tx(&byte, 1))) return rc;
                }
                else
                {
                    if ((rc = this->tx(&byte, 1))) return rc;
                    if ((rc = this->tx(&addr, 1)) != RESULT_OK) return rc;
                }
            }
            else
            {
                byte = txn->address << 1;
                if (rx) byte |= 1;
                if ((rc = this->tx(&byte, 1)) != RESULT_OK) return rc;
            }
        }
        int len = txn->transfers[i].len;
        if (rx)
        {
            bool last = i == txn->transferCount - 1 || txn->transfers[i + 1].type == Transaction::Transfer::TYPE_CONT;
            if ((rc = this->rx(txn->transfers[i].rxbuf, len, last)) != RESULT_OK) return rc;
        }
        else
        {
            if ((rc = this->tx(txn->transfers[i].txbuf, len)) != RESULT_OK) return rc;
        }
    }
    return stop();
}
