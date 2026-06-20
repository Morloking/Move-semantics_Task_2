#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>


class BigNumsClass {
public:
    //simple constructor
    BigNumsClass() :
        negative(false),
        data{ 0 }
    {};
    //base constructor
    BigNumsClass(const std::string& num) : 
        negative(false) {
        initData(num);
        normalize();
    };

    void normalize() {
        while (data.size() > 1 && data.back() == 0) {
            data.pop_back();
        }
        if (data.empty()) {
            data.push_back(0);
        }
        if (data.size() == 1 && data[0] == 0)
            negative = false;  
    }

    //move constructor
    BigNumsClass(BigNumsClass&& moved) noexcept :
        data(std::move(moved.data)), negative(moved.negative) 
    {}

    //move assignment operator
    BigNumsClass& operator=(BigNumsClass&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            negative = other.negative;
            
        }
        return *this;
    }
    //addition operator
    BigNumsClass operator+(const BigNumsClass& other) const {
        BigNumsClass result;
        result.data.clear();
        //-- or ++
        if (negative == other.negative) { 
            

            int numToCarry{ 0 }; // 0/1
            size_t n = std::max(data.size(), other.data.size()); // max number of digits between two numbers
            for (size_t i = 0; i < n; ++i) {
                int sum = numToCarry;
                if (i < data.size()) sum += data[i];
                if (i < other.data.size()) sum += other.data[i];
                result.data.push_back(sum % 10); // if  > 10 -> remainder go to numToCarry
                numToCarry = sum / 10; //if sum>10->1 else ->0
            }
            if (numToCarry) result.data.push_back(numToCarry);
            result.negative = negative;
            result.normalize();
            return result;
        }
        //-+ or +-
        bool thisBigger{ false };
        if (data.size() != other.data.size()) {
            thisBigger = data.size() > other.data.size();
        }
        else {
            for (size_t i = data.size(); i > 0; --i) {
                size_t index = i - 1;
                if (data[index] != other.data[index]) {
                    thisBigger = data[index] > other.data[index];
                    break;
                }
            }
        }

        const BigNumsClass* big = thisBigger ? this : &other;
        const BigNumsClass* small = thisBigger ? &other : this;

        int numToBorrow{ 0 };
        for (size_t i = 0; i < big->data.size(); ++i) {
            int sub = (i < small->data.size()) ?
                small->data[i] : 0;
            int currentSubResult = big->data[i] - sub - numToBorrow;
            
            if (currentSubResult < 0) {
                currentSubResult += 10;
                numToBorrow = 1;
            }
            else {
                numToBorrow = 0;
            }
            result.data.push_back(currentSubResult);
        }
        result.negative = thisBigger ? negative : other.negative;
        result.normalize();
        return result;
    }
    //multiplication operator (2 variants - class*class and class*int)
    //var 1 class*class
    BigNumsClass operator*(const BigNumsClass& other) const {
        BigNumsClass result;
        result.data.clear();
        result.data.resize(data.size() + other.data.size(),0);
        

        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < other.data.size(); ++j) {
                result.data[i + j] += data[i] * other.data[j];
            }  
        }
        //carry
        int numToCarry{ 0 };
        for (size_t i = 0; i < result.data.size(); ++i) {
            int currentResult = result.data[i] + numToCarry;
            result.data[i] = currentResult % 10;
            numToCarry = currentResult / 10;
        }
        while (numToCarry) {
            result.data.push_back(numToCarry % 10);
            numToCarry /= 10;
        }
        if (negative != other.negative) { 
            result.negative = true;
        }
        else {
            result.negative = false;
        }
        result.normalize();
        return result;
    }
    //var 2 class*int
    BigNumsClass operator*(int num) const {
        if (num == 0) return BigNumsClass();
        
        BigNumsClass result;
        result.data.clear();
        bool numIsNegative = num < 0;
        if (negative != numIsNegative) {
            result.negative = true;
        }
        else {
            result.negative = false;
        }
        int multiplier = (numIsNegative ? -num : num);

        int numToCarry{ 0 };
        for (size_t i = 0; i < data.size(); ++i) {
            int currentMultiplyResult = data[i] * multiplier + numToCarry;
            result.data.push_back(currentMultiplyResult % 10);
            numToCarry = currentMultiplyResult / 10;
        }
        while (numToCarry != 0) {
            result.data.push_back(numToCarry % 10);
            numToCarry /= 10;
        }
        
        
        result.normalize();
        return result;
    }

    friend std::ostream& operator <<(std::ostream& os, const BigNumsClass& num) {
        if (num.negative) os << '-';
        for (auto it = num.data.rbegin(); it != num.data.rend(); ++it) {
            os << *it;
        }
        return os;
    }

private:
    void initData(const std::string& num) {
        size_t start = 0;
        if (!num.empty() && num[0] == '-') {
            negative = true;
            start = 1;
        }
        for (auto it = num.rbegin(); it != num.rend()-start; ++it ) {
            data.push_back(static_cast<int>(*it-'0'));
        }
    }


    std::vector<int> data;
    bool negative;
};


int main() {
    auto number1 = BigNumsClass("114575");
    auto number2 = BigNumsClass("78524");
    auto res = number1 + number2;
    std::cout << res; 
    return 0;
}

