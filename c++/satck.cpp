#include <iostream>
#include <sstream>

using namespace std;

// 链栈节点
struct Node {
    int data;
    Node* next;
};

// 函数：创建新节点
Node* createNode(int value) {
    Node* newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;
    return newNode;
}

// 函数：判断字符是否为操作符
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// 函数：执行运算
int performOperation(char operation, int operand1, int operand2) {
    switch (operation) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            return operand1 / operand2;
        default:
            return 0;
    }
}

// 函数：后缀表达式求值
int evaluatePostfixExpression(const string& postfixExpression) {
    Node* operandStack = nullptr;

    // 用字符串流逐个读取后缀表达式的各个项
    istringstream iss(postfixExpression);
    string item;

    while (iss >> item) {
        // 如果是操作数，将其转换为整数并压入操作数栈
        if (!isOperator(item[0])) {
            int operand = stoi(item);
            Node* newNode = createNode(operand);
            newNode->next = operandStack;
            operandStack = newNode;
        } else {
            // 如果是操作符，弹出操作数栈的两个操作数进行运算，并将结果压入操作数栈
            int operand2 = operandStack->data;
            Node* temp = operandStack;
            operandStack = operandStack->next;
            delete temp;

            int operand1 = operandStack->data;
            temp = operandStack;
            operandStack = operandStack->next;
            delete temp;

            int result = performOperation(item[0], operand1, operand2);
            Node* newNode = createNode(result);
            newNode->next = operandStack;
            operandStack = newNode;
        }
    }

    // 返回最终的计算结果
    return operandStack->data;
}

int main() {
    string postfixExpression;
    cout << "请输入后缀表达式（后缀表达式长度不超过一行，各项之间用空格分隔，仅考虑四则运算，表达式以#键结束）：";
    getline(cin, postfixExpression, '#');

    int result = evaluatePostfixExpression(postfixExpression);
    cout << "计算结果为：" << result << endl;

    return 0;
}