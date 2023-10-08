#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <unordered_map>
#include <assert.h>

/**
 * Тестовое задание
 * Рюмин Сергей Павлович
 * Дата выполнения - 08.10.2023
 * Затрачено времени ~ 3 часа
 */

/**
 * 1. Напишите функцию, которая принимает на вход знаковое целое
 * число и печатает его двоичное представление как в памяти.
 */

void printBinary(unsigned int num) {
    int intBitsSize = sizeof(num) * 8;
    unsigned int mask = 1 << (intBitsSize - 1);
    for (int i = 0; i < intBitsSize; i++) {
        if (num & mask)
            std::cout << "1";
        else
            std::cout << "0";
        mask >>= 1;
    }
}

void testPrintBynary() {
    std::cout << "test conversion in to bynary" << std::endl;
    std::vector<int> testData{-10000, -1000, -127, -1, 0,
                              1, 127, 1000, 10000};
    for (int i: testData) {
        std::cout << i << " to binaty -> ";
        printBinary(i);
        std::cout << std::endl;
    }
}

/**
 * 2. Напишите функцию, удаляющую последовательно дублирующиеся символы в строке
 * void RemoveDups(char* str);
 * // пример использования
 * char data[] = "AAA BBB   AAA";
 * RemoveDups(data);
 * printf("%s\n", data); // "A B A"
 */
void removeDups(char* str) {
    assert(str);
    int len = strlen(str);
    if (len < 2) return;
    int i = 1;
    int j = 1;
    while (i < len) {
        j++;
        if (str[i] != str[i-1]) {
            i++;
        }
        str[i] = str[j];
    }
    str[i] = '\0';
}

void testRemoveDups() {
    std::map<const char*, const char*> testData{
        {"AAA BBB AAA cc     ###    %%%   D", "A B A c # % D"},
        {"", ""},
        {"a", "a"},
        {"abcde", "abcde"},
        {"aaabbbcccdddeee", "abcde"},
        {"111222333", "123"},
        {"!@#$%^&*()", "!@#$%^&*()"}
    };
    for (auto& data : testData) {
        char dest[1024];
        strcpy(dest, data.first);
        removeDups(dest);
        assert(strcmp(dest, data.second) == 0);
    }
    std::cout << "removeDups test passed" << std::endl;
}

/**
 * 3. Реализуйте функции сериализации и десериализации двусвязного списка.
 * Данные должны быть записаны в бинарном формате.
 * Ожидаемая алгоритмическая сложность – меньше квадратичной.
 */


struct ListNode {
    ListNode* prev = nullptr;
    ListNode* next = nullptr;
    ListNode* rand = nullptr;
    std::string data;
};

class List {
public:
    ~List() {
        dropData();
    }

    const ListNode* getHead() const {
        return head;
    }

    ListNode* getHead() {
        return head;
    }

    const ListNode* getTail() const {
        return tail;
    }

    ListNode* getTail() {
        return tail;
    }


    int getCount() const {
        return count;
    }

    ListNode * addNode(const std::string & data, ListNode * rand = nullptr) {
        ListNode* node = new ListNode();
        node->data = data;
        node->rand = rand;
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        count++;
        return node;
    }

    /**
     * сохранение списка в файл, файл открыт с помощью `fopen(path, "wb")`
     */
    void serialize(FILE* file) {
        fwrite(&count, sizeof(int), 1, file);
        std::vector<ListNode*> nodes(count);
        std::unordered_map<ListNode*,int> nodeIndexes;
        ListNode* current = head;
        for (int i = 0; i < count; i++) {
            nodes[i] = current;
            nodeIndexes.insert(std::make_pair(current, i));
            current = current->next;
        }
        for (int i = 0; i < count; i++) {
            ListNode * node = nodes[i];
            int dataLength = node->data.length();
            fwrite(&dataLength, sizeof(int), 1, file);
            fwrite(node->data.c_str(), sizeof(char), dataLength, file);
            int randIndex = -1;
            if (node->rand) {
                auto it = nodeIndexes.find(node->rand);
                if (it != nodeIndexes.end()) {
                    randIndex = it->second;
                }
            }
            fwrite(&randIndex, sizeof(int), 1, file);
        }
    }

    /**
     * восстановление списка из файла, файл открыт с помощью `fopen(path, "rb")`
     */
    void deserialize(FILE* file) {
        int new_count;
        fread(&new_count, sizeof(int), 1, file);
        std::vector<ListNode*> nodes(new_count);
        std::unordered_map<ListNode*,int> nodeRandIndexes;
        for (int i = 0; i < new_count; i++) {
            int dataLength;
            fread(&dataLength, sizeof(int), 1, file);

            char data[dataLength + 1];
            fread(data, sizeof(char), dataLength, file);
            data[dataLength] = '\0';

            ListNode* node = new ListNode();
            node->data = std::string(data);
            nodes[i] = node;

            int randIndex;
            fread(&randIndex, sizeof(int), 1, file);
            nodeRandIndexes.insert(std::make_pair(node, randIndex));
        }
        for (int i = 0; i < nodes.size(); i++) {
            ListNode * node = nodes[i];
            if (i > 0) {
                node->prev = nodes[i-1];
            }
            if (i < nodes.size() - 1) {
                node->next = nodes[i + 1];
            }
            int randIndex = nodeRandIndexes[node];
            if (randIndex != -1) {
                node->rand = nodes[randIndex];
            }
        }
        dropData();
        if (new_count >= 1) {
            count = new_count;
            head = nodes[0];
            tail = nodes[new_count - 1];
        }
    }

private:
    void dropData() {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

private:
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    int count = 0;
};

bool operator==(const List& lhs, const List& rhs) {
    if (lhs.getCount() != rhs.getCount()) {
        return false;
    }
    int size = lhs.getCount();

    std::vector<const ListNode*> leftNodes(size);
    std::vector<const ListNode*> rightNodes(size);

    std::unordered_map<const ListNode*,int> leftIndexes;
    std::unordered_map<const ListNode*,int> rightIndexes;

    const ListNode* leftCurrent = lhs.getHead();
    const ListNode* rightCurrent = rhs.getHead();

    for ( int i = 0; i < size; i++ ) {
        leftNodes[i] = leftCurrent;
        leftIndexes.insert(std::make_pair(leftCurrent, i));
        leftCurrent = leftCurrent->next;

        rightNodes[i] = rightCurrent;
        rightIndexes.insert(std::make_pair(rightCurrent, i));
        rightCurrent = rightCurrent->next;
    }

    for (int i = 0; i < leftNodes.size(); i++ ) {
        const ListNode * leftNode = leftNodes[i];
        const ListNode * rightNode = rightNodes[i];
        if (leftNode->data != rightNode->data) {
            return false;
        }
        if (!leftNode->rand) {
            if (rightNode->rand) {
                return false;
            }
        } else {
            if (!rightNode->rand) {
                return false;
            }
            const ListNode * leftRand = leftNode->rand;
            const ListNode * rightRand = rightNode->rand;
            if (leftRand->data != rightRand->data) {
                return false;
            }
            int leftRandIndex = leftIndexes[leftRand];
            int rightRandIndex = rightIndexes[rightRand];
            if (leftRandIndex != rightRandIndex) {
                return false;
            }
        }
    }
    return true;
}

void testList() {
    List list;
    ListNode* node1 = list.addNode("data1");
    ListNode* node2 = list.addNode("data2", node1);
    ListNode* node3 = list.addNode("data3", node2);
    node1->rand = node3;
    node2->rand = node1;
    node3->rand = node2;

    FILE* file = fopen("test.bin", "wb");
    list.serialize(file);
    fclose(file);

    List newList;
    file = fopen("test.bin", "rb");
    newList.deserialize(file);
    fclose(file);

    assert(list == newList);
    node2->rand = node3;
    assert(!(list == newList));
    node2->rand = node1;
    assert(list == newList);
    node1->data = "data2";
    assert(!(list == newList));
    node1->data = "data1";
    assert(list == newList);
    std::cout << "serialize list test passed" << std::endl;
}


int main()
{
    testPrintBynary();
    testRemoveDups();
    testList();
    return 0;
}
